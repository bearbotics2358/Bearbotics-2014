#include "HotGoalDetector.h"
#include "Prefs.h"
#include "Logger.h"

#include <WPILib.h>
#include <networktables2/util/System.h>

#include <math.h>
#include <iostream>
#include <stdexcept>
#include <sstream>

using std::cout;

HotGoalDetector::HotGoalDetector(Logger *const logger)
	: ap_camera(&AxisCamera::GetInstance("10.23.58.11")),
	  a_leds(CAMERA_LED_PORT),
	  ap_logger(logger)
{
	a_leds.Set(1);
}

void HotGoalDetector::CheckIMAQError(int rval, std::string desc)
{
	if(rval == 0)
	{
		int error = imaqGetLastError();

		std::ostringstream errorDesc;
		errorDesc << error << " "
			      << desc;

		throw std::runtime_error(errorDesc.str());
	}
}

std::vector<Particle> HotGoalDetector::GenerateParticleReport(Image *image)
{
	int rval, numParticles;
	rval = imaqCountParticles(image, 1, &numParticles);
	CheckIMAQError(rval, "imaqCountParticles");

	const int kNumMeasures = 7;
	MeasurementType measures[kNumMeasures] =
	{
		IMAQ_MT_CENTER_OF_MASS_X,
		IMAQ_MT_CENTER_OF_MASS_Y,
		IMAQ_MT_BOUNDING_RECT_LEFT,
		IMAQ_MT_BOUNDING_RECT_TOP,
		IMAQ_MT_BOUNDING_RECT_WIDTH,
		IMAQ_MT_BOUNDING_RECT_HEIGHT,
		IMAQ_MT_AREA,
	};

	MeasureParticlesReport *particleReport =
		imaqMeasureParticles(image,
			                 IMAQ_CALIBRATION_MODE_PIXEL,
							 measures,
							 kNumMeasures);
	if(particleReport == NULL)
	{
		CheckIMAQError(0, "imaqMeasureParticles");
	}

	std::vector<Particle> particles;
	for(int i = 0; i < numParticles; i++)
	{
		Particle p;
		p.id         = i;
		p.cx         = particleReport->pixelMeasurements[i][0];
		p.cy         = particleReport->pixelMeasurements[i][1];
		p.left       = particleReport->pixelMeasurements[i][2];
		p.top        = particleReport->pixelMeasurements[i][3];
		p.width      = particleReport->pixelMeasurements[i][4];
		p.height     = particleReport->pixelMeasurements[i][5];
		p.area       = particleReport->pixelMeasurements[i][6];
		p.horizontal = (p.width > p.height);
		particles.push_back(p);
	}

	// Cleanup
	rval = imaqDispose(particleReport);
	CheckIMAQError(rval, "imaqDispose(particleReport)");

	return particles;
}

void HotGoalDetector::FilterParticles(std::vector<Particle> &particles)
{
	vector<Particle>::iterator it;
	for(it = particles.begin(); it != particles.end(); )
	{
		Particle p = *it;
		if(p.area < 50.0)
		{
			it = particles.erase(it);
		}
		else if(fabs(p.width - p.height) < 100.0)
		{
			it = particles.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void HotGoalDetector::PrintParticles(const std::vector<Particle> &particles)
{
	vector<Particle>::const_iterator it;
	for(it = particles.begin(); it != particles.end(); ++it)
	{
		Particle p = *it;
		p.Print();
	}
}

void HotGoalDetector::LogParticles(const std::vector<Particle> &particles)
{
	vector<Particle>::const_iterator it;
	for(it = particles.begin(); it != particles.end(); ++it)
	{
		Particle p = *it;
		p.Log(ap_logger);
	}
}

void HotGoalDetector::SnapImage(void)
{
	if(!ap_camera->IsFreshImage())
	{
		throw std::runtime_error("No fresh image");
	}
	a_leds.Set(0);
	sleep_ms(250);
	a_image.reset(ap_camera->GetImage());
	sleep_ms(50);
	a_leds.Set(1);
}

bool HotGoalDetector::DetectHotGoal(bool snapImage, bool saveImage, bool logReport)
{
	bool rv = false;
	int rval;
	int width, height;

	if(snapImage)
	{
		SnapImage();
	}

	std::auto_ptr<MonoImage> mono(a_image->GetLuminancePlane());
	std::auto_ptr<Image> imaqImage(mono->GetImaqImage());
	Image *image = imaqImage.get();

	rval = imaqGetImageSize(image, &width, &height);
	CheckIMAQError(rval, "imaqGetImageSize");

	rval = imaqThreshold(image, image, 140, 255, 1, 255);
	CheckIMAQError(rval, "imaqThreshold");

	rval = imaqConvexHull(image, image, 1);
	CheckIMAQError(rval, "imaqConvexHull");

	StructuringElement element;
	element.matrixRows = 5;
	element.matrixCols = 5;
	element.hexa = FALSE;

	rval = imaqMorphology(image, image, IMAQ_ERODE, &element);
	CheckIMAQError(rval, "imaqMorphology (IMAQ_ERODE)");

	rval = imaqMorphology(image, image, IMAQ_DILATE, &element);
	CheckIMAQError(rval, "imaqMorphology (IMAQ_DILATE)");

	rval = imaqEqualize(image, image, 0, 255, NULL);
	CheckIMAQError(rval, "imaqEqualize");

	// Write image to file system
	if(saveImage)
	{
		imaqWriteJPEGFile(image, "processed-image.jpg", 750, NULL);
	}

	std::vector<Particle> particles = GenerateParticleReport(image);
	FilterParticles(particles);
	if(logReport)
	{
		PrintParticles(particles);
		LogParticles(particles);
	}

	vector<Particle>::const_iterator it;
	for(it = particles.begin(); it != particles.end(); ++it)
	{
		Particle p = *it;

		// Only look for vertical (static) targets
		if(p.horizontal)
		{
			continue;
		}

		// The targets should be pretty long and thin
		double vRectPct = p.width / p.height;
		if(vRectPct > 0.30)
		{
			continue;
		}
		
		vector<Particle>::const_iterator it2;
		for(it2 = particles.begin(); it2 != particles.end(); ++it2)
		{
			Particle p2 = *it2;

			// Only examine horizontal targets
			if(!p2.horizontal)
			{
				continue;
			}

			// The targets should be pretty long and thin
			double hRectPct = p.height / p.width;
			if(hRectPct > 0.35)
			{
				continue;
			}

			// The vision targets are very close in
			// terms of their vertical tops
			double heightDiff = fabs(p2.top - p.top);
			if(heightDiff > 15.0)
			{
				continue;
			}
			
			// They should also be pretty close (though
			// not as close) in terms of their leftmost
			// pixels
			double leftDiff = fabs(p2.left - p.left);
			if(leftDiff > 35.0)
			{
				continue;
			}

			// Fairly certain of a match at this point
			rv = true;
		}
	}

	return rv;
}


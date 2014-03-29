#include "HotGoalDetector.h"
#include "Prefs.h"

#include <WPILib.h>
#include <nivision.h>
#include <networktables2/util/System.h>

#include <math.h>
#include <iostream>
#include <stdexcept>
#include <sstream>

using std::cout;

HotGoalDetector::HotGoalDetector(void)
	: ap_camera(&AxisCamera::GetInstance("10.23.58.11")),
	  a_leds(CAMERA_LED_PORT)
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

void HotGoalDetector::SnapImage(void)
{
	if(!ap_camera->IsFreshImage())
	{
		throw std::runtime_error("No fresh image");
	}
	a_leds.Set(0);
	sleep_ms(500);
	a_image.reset(ap_camera->GetImage());
	sleep_ms(500);
	a_leds.Set(1);
}

bool HotGoalDetector::DetectHotGoal(bool snapImage)
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
	// TODO: flag for this to happen?
	imaqWriteJPEGFile(image, "processed-image.jpg", 750, NULL);

	int numParticles;
	rval = imaqCountParticles(image, 1, &numParticles);
	CheckIMAQError(rval, "imaqCountParticles");

	const int kNumMeasures = 5;
	MeasurementType measures[kNumMeasures] =
	{
		IMAQ_MT_CENTER_OF_MASS_X,
		IMAQ_MT_CENTER_OF_MASS_Y,
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
		p.x      = particleReport->pixelMeasurements[i][0];
		p.y      = particleReport->pixelMeasurements[i][1];
		p.width  = particleReport->pixelMeasurements[i][2];
		p.height = particleReport->pixelMeasurements[i][3];
		p.area   = particleReport->pixelMeasurements[i][4];
		particles.push_back(p);
	}
	FilterParticles(particles);
	PrintParticles(particles);

	// Cleanup
	rval = imaqDispose(particleReport);
	CheckIMAQError(rval, "imaqDispose(particleReport)");

	return rv;
}


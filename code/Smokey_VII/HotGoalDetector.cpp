#include "HotGoalDetector.h"
#include "Prefs.h"

#include <WPILib.h>
#include <nivision.h>

#include <iostream>
#include <stdexcept>
#include <sstream>

using std::cout;

HotGoalDetector::HotGoalDetector(void)
	: ap_camera(&AxisCamera::GetInstance()),
	  a_leds(CAMERA_LED_PORT)
{
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

void HotGoalDetector::SnapImage(void)
{
	if(!ap_camera->IsFreshImage())
	{
		throw new std::runtime_error("No fresh image");
	}
	a_leds.Set(1);
	a_image.reset(ap_camera->GetImage());
	a_leds.Set(0);
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

	MeasureParticlesReport *particles =
		imaqMeasureParticles(image,
			                 IMAQ_CALIBRATION_MODE_PIXEL,
							 measures,
							 kNumMeasures);
	if(particles == NULL)
	{
		CheckIMAQError(0, "imaqMeasureParticles");
	}

	for(int i = 0; i < numParticles; i++)
	{
		double x = particles->pixelMeasurements[i][0];
		double y = particles->pixelMeasurements[i][1];
		double w = particles->pixelMeasurements[i][2];
		double h = particles->pixelMeasurements[i][3];
		double a = particles->pixelMeasurements[i][4];

		// Get rid of tiny stuff
		if(a > 50.0)
		{
			cout << "=== PARTICLE " << i << std::endl;
			cout << "center: (" << x << ", " << y << ")" << std::endl;
			cout << "width/height: (" << w << ", " << h << ")" << std::endl;
			cout << "area: " << a << endl;
			cout << std::endl;
		}
	}

	// Cleanup
	rval = imaqDispose(particles);
	CheckIMAQError(rval, "imaqDispose(particles)");

	return rv;
}


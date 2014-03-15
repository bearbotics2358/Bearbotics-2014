#ifndef _HOT_GOAL_DETECTOR_H_
#define _HOT_GOAL_DETECTOR_H_

#include <memory>
#include <vector>
#include <string>

#include <DigitalOutput.h>

class AxisCamera;
class DigitalOutput;
class HSLImage;

class HotGoalDetector
{
public:
	void SnapImage(void);
	HotGoalDetector(void);

	bool DetectHotGoal(bool snapImage = false);

private:
	static void CheckIMAQError(int rval, std::string desc);

	AxisCamera *ap_camera;
	DigitalOutput a_leds;

	std::auto_ptr<HSLImage> a_image;
};

#endif // _HOT_GOAL_DETECTOR_H_


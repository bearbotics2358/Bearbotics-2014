#ifndef _HOT_GOAL_DETECTOR_H_
#define _HOT_GOAL_DETECTOR_H_

#include <memory>
#include <vector>
#include <string>

class AxisCamera;
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
	std::auto_ptr<HSLImage> a_image;
};

#endif // _HOT_GOAL_DETECTOR_H_


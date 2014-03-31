#ifndef _HOT_GOAL_DETECTOR_H_
#define _HOT_GOAL_DETECTOR_H_

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include <DigitalOutput.h>

class AxisCamera;
class DigitalOutput;
class HSLImage;

struct Particle
{
	int id;
	double x, y;
	double width, height;
	double area;

	void Print(void)
	{
		std::cout << "=== PARTICLE " << id << std::endl;
		std::cout << "center: (" << x << ", " << y << ")" << std::endl;
		std::cout << "width/height: (" << width << ", " << height << ")" << std::endl;
		std::cout << "area: " << area << std::endl;
		std::cout << std::endl;
	}
};

class HotGoalDetector
{
public:
	HotGoalDetector(void);

	void SnapImage(void);
	bool DetectHotGoal(bool snapImage = false);

private:
	static void CheckIMAQError(int rval, std::string desc);
	static void FilterParticles(std::vector<Particle> &particles);
	static void PrintParticles(const std::vector<Particle> &particles);

	AxisCamera *ap_camera;
	DigitalOutput a_leds;

	std::auto_ptr<HSLImage> a_image;
};

#endif // _HOT_GOAL_DETECTOR_H_


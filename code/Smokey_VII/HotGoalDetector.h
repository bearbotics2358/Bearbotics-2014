#ifndef _HOT_GOAL_DETECTOR_H_
#define _HOT_GOAL_DETECTOR_H_

#include "Logger.h"

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

#include <DigitalOutput.h>
#include <nivision.h>

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

	void Log(Logger *const logger)
	{
		std::ostringstream ss;
		ss << "=== PARTICLE " << id << std::endl;
		ss << "center: (" << x << ", " << y << ")" << std::endl;
		ss << "width/height: (" << width << ", " << height << ")" << std::endl;
		ss << "area: " << area << std::endl;
		ss << std::endl;
		logger->Log(ss.str());
	}
};

class HotGoalDetector
{
public:
	HotGoalDetector(Logger *const logger);

	void SnapImage(void);
	bool DetectHotGoal(bool snapImage = false);

private:
	static void CheckIMAQError(int rval, std::string desc);
	static std::vector<Particle> GenerateParticleReport(Image *image);
	void FilterParticles(std::vector<Particle> &particles);
	void PrintParticles(const std::vector<Particle> &particles);
	void LogParticles(const std::vector<Particle> &particles);

	AxisCamera *ap_camera;
	DigitalOutput a_leds;
	Logger *const ap_logger;

	std::auto_ptr<HSLImage> a_image;
};

#endif // _HOT_GOAL_DETECTOR_H_


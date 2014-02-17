/* Sonar.h - receive ultrasonic ranging measurements from sensors and
	 control which are active using Ardunio based ultrasonic multiplexer

	 created 2/16/14 BD

*/

#ifndef H_SONAR
#define H_SONAR

#include <SerialPort.h>

#define BUFF_SIZE 100
#define LEFT_FRONT 0
#define RIGHT_FRONT 1
#define REAR 2

class Sonar
{
 public:

	Sonar(int baud);
	~Sonar() {}

	periodic();
	int GetCM(int port);
	float GetFeet(int port);
	float GetDistanceGoal();
	float GetDistanceDriver();
	EnablePort(int port);
	DisablePort(int port);
	EnableFrontOnly();
	EnableRearOnly();

 private:
	SerialPort sport;
	char rx_buff[BUFF_SIZE];
	int range[4];
} ;

#endif

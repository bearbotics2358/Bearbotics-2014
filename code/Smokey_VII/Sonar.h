/* Sonar.h - receive ultrasonic ranging measurements from sensors and
	 control which are active using Ardunio based ultrasonic multiplexer

	 created 2/16/14 BD

*/

#ifndef H_SONAR
#define H_SONAR

#include <SerialPort.h>

#define SONAR_BUFF_SIZE 100

class Sonar
{
 public:

	typedef enum{
		kLeftFront = 0,
		kRightFront = 1,
		kRear = 2,
		kNone = 3,
	} SensorEnum;
	Sonar(int baud);
	~Sonar() {}

	void periodic();
	int GetCM(SensorEnum port);
	float GetFeet(SensorEnum port);
	float GetDistanceFront();
	float GetDistanceRear();
	void EnablePort(SensorEnum port);
	void DisablePort(SensorEnum port);
	void EnableFrontOnly();
	void EnableRearOnly();
	void RxFlush();
	
 private:
	SerialPort* sport;
	char rx_buff[SONAR_BUFF_SIZE];
	int range[4];
	int rx_index;
} ;

#endif

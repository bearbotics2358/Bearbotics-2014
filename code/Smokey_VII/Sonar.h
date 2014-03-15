/* Sonar.h - receive ultrasonic ranging measurements from sensors and
	 control which are active using Ardunio based ultrasonic multiplexer

	 created 2/16/14 BD

	 updated 3/11/14 BD
	 - added past values of range to permit filtering out bad samples
*/

#ifndef H_SONAR
#define H_SONAR

#include <SerialPort.h>

#define SONAR_BUFF_SIZE 100
#define SONAR_HISTORY_SIZE 3
#define SONAR_HISTORY_MAX 3
#define SONAR_UNITS 4

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
	int GetCMsafe(SensorEnum port);
 private:
	SerialPort* sport;
	char rx_buff[SONAR_BUFF_SIZE];
	int range[SONAR_UNITS][SONAR_HISTORY_SIZE]; // 1st index is unit, 2nd is sample, 0 being newest
	int rx_index;
} ;

#endif

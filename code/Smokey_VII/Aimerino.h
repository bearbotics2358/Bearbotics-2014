#ifndef AIMERINO_H
#define AIMERINO_H

#include "Potentiometer.h"

class Talon;
class PIDController;

class Aimerino{
public:
	
	static const double UP			= 90.0;
	static const double PARALLEL	= 0.0;
	static const double DOWN		= -32.0; //comp = -36
	static const double SHOOT		= 45.0;
	static const double BELOWSHOOT	= 30.0;
	static const double ABOVESHOOT	= 50.0;
				
	Aimerino(int motorPort, int potPort, double scale, double offset);
	virtual ~Aimerino(void);
	double getAngle(void);
	void setAngle(double angle, double speed);
	void setEnabled(bool enable);
	void setPID(double p, double i, double d);
	void printPID(void);
	
private:
	
	bool a_enabled;
	PIDController* ap_PID;
	Potentiometer* ap_Pot;
	Talon* ap_Motor;
};

#endif

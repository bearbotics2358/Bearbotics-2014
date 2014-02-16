#ifndef _POTENTIOMETER_H_
#define _POTENTIOMETER_H_

#include "AnalogChannel.h"

class Potentiometer : public AnalogChannel{
public:
	
	//@param scale: amount of degrees per volt read by analog channel
	Potentiometer(uint8_t channel, double scale, double startingVoltage);
	virtual ~Potentiometer();
	void SetScale(double scale);
	void SetStartingVoltage(double startingVoltage);
	double GetStartingVoltage(void);
	double GetScale(void);
	double GetScaledValue(void);
	double PIDGet(void);
	
private:
	double a_startingVoltage;
	double a_scale;
};

#endif

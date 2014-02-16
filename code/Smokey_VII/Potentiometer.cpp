#include "Potentiometer.h"

Potentiometer::Potentiometer(uint8_t channel, double scale, double startingVoltage)
:AnalogChannel(channel)
{
	a_startingVoltage = startingVoltage;
	a_scale = scale;
}

Potentiometer::~Potentiometer()
{
	a_scale = 0.0;
}

void Potentiometer::SetScale(double scale)
{
	a_scale = scale;
}

void Potentiometer::SetStartingVoltage(double startingVoltage)
{
	a_startingVoltage = startingVoltage;
}

double Potentiometer::GetStartingVoltage(void)
{
	return a_startingVoltage;
}

double Potentiometer::GetScale(void)
{
	return a_scale;
}

double Potentiometer::GetScaledValue(void)
{
	return (GetVoltage() - a_startingVoltage)* a_scale;
}

double Potentiometer::PIDGet(void)
{
	return GetScaledValue();
}

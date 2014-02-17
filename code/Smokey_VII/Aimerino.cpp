#include "Talon.h"
#include "PIDController.h"

#include "Aimerino.h"

Aimerino::Aimerino(int motorPort, int potPort, double scale, double offset)
{
	ap_Pot = new Potentiometer(potPort, scale, offset);
	ap_Motor = new Talon(motorPort);
	ap_PID = new PIDController(0.1, 0.0, 0.0, ap_Pot, ap_Motor);
	ap_PID->SetOutputRange(-0.6, 0.6);
	a_enabled = false;
}

Aimerino::~Aimerino()
{
	delete ap_PID;
	ap_PID = NULL;
	delete ap_Pot;
	ap_Pot = NULL;
	delete ap_Motor;
	ap_Motor = NULL;
}

void Aimerino::setPID(double p, double i, double d)
{
	ap_PID->SetPID(p,i,d);
}

double Aimerino::getAngle()
{
	printf("volt: %f\n", ap_Pot->GetVoltage());
	return ap_Pot->GetScaledValue();
}

void Aimerino::setAngle(double angle)
{
	ap_PID->SetSetpoint(angle);
}

void Aimerino::setEnabled(bool enable)
{
	a_enabled = enable;
	(enable) ? ap_PID->Enable() : ap_PID->Disable();
}

void Aimerino::printPID(void)
{
	printf("P: %f\tI: %f\tD: %f\n", 
			ap_PID->GetP(), 
			ap_PID->GetI(), 
			ap_PID->GetD());
}

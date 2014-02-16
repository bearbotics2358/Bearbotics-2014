#include "Shooter.h"

#include <DigitalInput.h>
#include <Joystick.h>
#include <Talon.h>

Shooter::Shooter(unsigned int motorPort, unsigned int stopSensorPort)
	: a_state(SHOOTER_STATE_IDLE), a_reArm(true)
{
	ap_motor = new Talon(motorPort);
	ap_stopSensor = new DigitalInput(stopSensorPort);
}

Shooter::~Shooter(void)
{
	delete ap_motor;
	ap_motor = NULL;

	delete ap_stopSensor;
	ap_stopSensor = NULL;
}

void Shooter::Update(Joystick &stick)
{
	ShooterState_t nextState = a_state;

	switch(a_state)
	{
		case SHOOTER_STATE_IDLE:
			ap_motor->Set(0.0);
			if(stick.GetRawButton(1))
			{
				nextState = SHOOTER_STATE_ARMING;
			}
			break;
		case SHOOTER_STATE_ARMING:
			ap_motor->Set(1.0);
			if(ap_stopSensor->Get())
			{
				nextState = SHOOTER_STATE_ARMED;
			}
			break;
		case SHOOTER_STATE_ARMED:
			ap_motor->Set(0.0);
			if(stick.GetRawButton(1))
			{
				nextState = SHOOTER_STATE_SHOOTING;
				a_reArm = true;
			}
			if(stick.GetRawButton(4))
			{
				nextState = SHOOTER_STATE_SHOOTING;
				a_reArm = false;
			}
			break;
		case SHOOTER_STATE_SHOOTING:
			ap_motor->Set(1.0);
			if(!ap_stopSensor->Get())
			{
				if(a_reArm)
				{
					nextState = SHOOTER_STATE_ARMING;
				}
				else
				{
					nextState = SHOOTER_STATE_IDLE;
				}
			}
			break;
	}

	a_state = nextState;

	// Logging
	if(a_verbose)
	{
		printf("Arm sensor: %d\n", ap_stopSensor->Get());
	}
}

bool Shooter::GetVerbose(void)
{
	return a_verbose;
}

void Shooter::SetVerbose(bool verbose)
{
	a_verbose = verbose;
}


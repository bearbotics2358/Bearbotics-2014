#include "Shooter.h"

#include <DigitalInput.h>
#include <Joystick.h>
#include <Talon.h>
#include <Counter.h>

Shooter::Shooter(unsigned int motorPort, unsigned int stopSensorPort)
	: a_state(SHOOTER_STATE_IDLE), a_stopSensorState(true), a_reArm(true)
{
	ap_motor = new Talon(motorPort);
	ap_stopSensor = new DigitalInput(stopSensorPort);
	ap_counter = new Counter(ap_stopSensor);
}

Shooter::~Shooter(void)
{
	delete ap_motor;
	ap_motor = NULL;

	delete ap_stopSensor;
	ap_stopSensor = NULL;

	delete ap_counter;
	ap_counter = NULL;
}

void Shooter::Init(bool enable)
{
	enable ? ap_counter->Start() : ap_counter->Stop();
}

void Shooter::SetEnabled(bool enable)
{
	a_enabled = enable;
}

void Shooter::UpdateControlLogic(bool shoot)
{
	if(a_enabled){
	ShooterState_t nextState = a_state;

	switch(a_state)
	{
		case SHOOTER_STATE_IDLE:
			ap_motor->Set(0.0);
			if(shoot) {
				ap_counter->Reset();
				nextState = SHOOTER_STATE_ARMING;
			}
			break;
		case SHOOTER_STATE_ARMING:
			ap_motor->Set(1.0);
			if(ap_counter->Get() > 0) {
				nextState = SHOOTER_STATE_IDLE;
			}
			break;
	}

	a_state = nextState;
	
	printf("State: %d\n", ap_counter->Get());
	} else { //disabled
		ap_motor->Set(0.0);
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


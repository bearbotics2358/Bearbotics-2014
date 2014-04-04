#ifndef _SHOOTER_H_
#define _SHOOTER_H_

class Talon;
class DigitalInput;
class Joystick;
class Counter;
class Timer;

typedef enum
{
	SHOOTER_STATE_IDLE,
	SHOOTER_STATE_ARMING,
	SHOOTER_STATE_NO_REARM,
} ShooterState_t;

class Shooter
{
public:
	Shooter(unsigned int motorPort, unsigned int stopSensorPort);
	~Shooter(void);

	void Init(bool enable);
	void UpdateControlLogic(bool shoot, bool noRearm, bool backDrive);

	bool GetVerbose(void);
	void SetVerbose(bool verbose);

	void SetEnabled(bool enable);
	ShooterState_t GetState(void){ return a_state; }
	
private:
	Talon *ap_motor;
	DigitalInput *ap_stopSensor;
	Counter *ap_counter;
	Timer *ap_timer;
	
	ShooterState_t a_state;
	bool a_stopSensorState;
	bool a_reArm;

	bool a_verbose;
	bool a_enabled;
};

#endif // _SHOOTER_H_


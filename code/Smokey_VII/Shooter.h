#ifndef _SHOOTER_H_
#define _SHOOTER_H_

class Talon;
class DigitalInput;
class Joystick;

typedef enum
{
	SHOOTER_STATE_IDLE,
	SHOOTER_STATE_ARMING,
	SHOOTER_STATE_ARMED,
	SHOOTER_STATE_SHOOTING
} ShooterState_t;

class Shooter
{
public:
	Shooter(unsigned int motorPort, unsigned int stopSensorPort);
	~Shooter(void);

	void Update(Joystick &stick);

	bool GetVerbose(void);
	void SetVerbose(bool verbose);

private:
	Talon *ap_motor;
	DigitalInput *ap_stopSensor;

	ShooterState_t a_state;
	bool a_reArm;

	bool a_verbose;
};

#endif // _SHOOTER_H_


#ifndef SMOKEY_VII_H
#define SMOKEY_VII_H

#define SMOKEY_MAJOR_VERSION 7
#define SMOKEY_MINOR_VERSION 3
#define SMOKEY_MICRO_VERSION 0

class Talon;
class Joystick;
class Gyro;
class RobotBase;
class IterativeRobot;
class DigitalInput;
class Sonar;
class Timer;

class LEDIndicator;
class Logger;
class Aimerino;
class Shooter;

enum AutonState{
	kAutonNULL = 0,
	kAutonDriveAndTilt = 1,
	kAutonShoot = 2,
};

class Smokey_VII : public IterativeRobot
{
public:
	
	Smokey_VII(void);
	~Smokey_VII(void);

	void RobotInit(void);

	void TeleopInit(void);
	void TeleopPeriodic(void);

	void DisabledInit(void);
	
	void AutonomousInit(void);
	void AutonomousPeriodic(void);
	
	void DisabledPeriodic(void);

private:
	int a_currentState;
	bool a_fieldOrientated;
	AutonState ap_states[2];

	Joystick joystick_;
	Joystick hedgyStick_;
	Gyro gyro_;
	Talon* ap_FLmotor;
	Talon* ap_FRmotor;
	Talon* ap_BLmotor;
	Talon* ap_BRmotor;
	Talon* ap_CollectorMotor;
	RobotDrive* ap_Drive;
	Aimerino* ap_Aimer;
	Shooter shooter_;
	Sonar sonars_;
	LEDIndicator indicator_;
	Logger log_;
};

#endif 

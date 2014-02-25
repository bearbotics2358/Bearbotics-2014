#ifndef SMOKEY_VII_H
#define SMOKEY_VII_H

#define SMOKEY_MAJOR_VERSION 7
#define SMOKEY_MINOR_VERSION 1
#define SMOKEY_MICRO_VERSION 0

class Talon;
class Joystick;
class Gyro;
class RobotBase;
class IterativeRobot;
class DigitalInput;
class Sonar;

class Aimerino;
class Shooter;

class Smokey_VII : public IterativeRobot
{
public:
	
	typedef enum{
		kAutonIdle,
		kAutonDriveForwards,
		kAutonShoot,
	} AutonState;
	
	
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
	int time;
	double twist;
	Gyro* ap_Gyro;
	Joystick* ap_Joystick;
	Joystick* ap_HedgyStick;
	Talon* ap_FLmotor;
	Talon* ap_FRmotor;
	Talon* ap_BLmotor;
	Talon* ap_BRmotor;
	Talon* ap_CollectorMotor;
	RobotDrive* ap_Drive;
	Aimerino* ap_Aimer;
	Shooter* ap_Shooter;
	Talon* ap_CallibratingMotor;
	Sonar* ap_Sonars;
	AutonState ap_states[];
	int a_currentState;
};

#endif 

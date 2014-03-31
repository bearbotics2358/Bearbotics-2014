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

class LEDIndicator_I2C;
class Logger;
class Aimerino;
class Shooter;
class HotGoalDetector;

enum AutonState{
	kAutonNULL = 0,
	kAutonDriveAndTilt = 1,
	kAutonShoot = 2,
	kAutonDelay = 3,
	kAutonWaitForHotZone = 4,
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
	bool a_fieldOrientated;
	AutonState m_currentState;

	Talon* ap_FLmotor;
	Talon* ap_FRmotor;
	Talon* ap_BLmotor;
	Talon* ap_BRmotor;
	Talon* ap_CollectorMotor;
	RobotDrive* ap_Drive;
	Aimerino* ap_Aimer;
	Joystick joystick_;
	Joystick hedgyStick_;
	Gyro gyro_;
	Shooter shooter_;
	Sonar sonars_;
	LEDIndicator_I2C m_indicator;
	Logger log_;
	Timer m_timer;
	//HotGoalDetector detector_;
};

#endif 

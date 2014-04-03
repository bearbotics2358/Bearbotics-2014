#include <IterativeRobot.h>
#include <Gyro.h>
#include <Talon.h>
#include <Joystick.h>
#include <RobotDrive.h>
#include <DigitalInput.h>
#include <SmartDashboard/SmartDashboard.h>
#include <math.h>
#include <Timer.h>
#include <cstdio>
#include <stdexcept>

#include "LEDIndicator_I2C.h"
#include "Logger.h"
#include "CompPrefs.h"
#include "Aimerino.h"
#include "Shooter.h"
#include "Sonar.h"
#include "HotGoalDetector.h"

#include "Smokey_VII.h"

Smokey_VII::Smokey_VII(void)
	:joystick_	(JOYSTICK_PORT),
	hedgyStick_	(HEDGYSTICK_PORT),
	gyro_		(GYRO_PORT),
	shooter_	(SHOOTER_PORT, MAG_SENSOR_PORT),
	sonars_		(SONAR_BAUD_RATE),
	m_indicator	(1),
	log_		("/ni-rt/system/FRC_UserFiles/kiiiiiiiwi.log"),
	m_timer		(),
	detector_		(&log_)
{
	ap_FLmotor = new Talon(FL_PORT);
	ap_FRmotor = new Talon(FR_PORT);
	ap_BLmotor = new Talon(BL_PORT);
	ap_BRmotor = new Talon(BR_PORT);
	ap_Drive = new RobotDrive(ap_FLmotor, ap_BLmotor, ap_FRmotor, ap_BRmotor);
	ap_Drive->SetInvertedMotor(ap_Drive->kFrontRightMotor, true);
	ap_Drive->SetInvertedMotor(ap_Drive->kRearRightMotor, true);
	ap_Drive->SetInvertedMotor(ap_Drive->kFrontLeftMotor, false);

	ap_CollectorMotor = new Talon(COLLECTOR_PORT);
	ap_Aimer = new Aimerino(AIMER_PORT, POT_PORT, AIM_OFFSET, AIM_SCALE);
	ap_Aimer->setPID(0.054, 0.0, 0.015);

	shooter_.SetVerbose(true);
	a_fieldOrientated = false;
}

Smokey_VII::~Smokey_VII(void)
{
	delete ap_FLmotor;
	ap_FLmotor = NULL;
	delete ap_FRmotor;
	ap_FRmotor = NULL;
	delete ap_BLmotor;
	ap_BLmotor = NULL;
	delete ap_BRmotor;
	ap_BRmotor = NULL;
	delete ap_Drive;
	ap_Drive = NULL;
	delete ap_CollectorMotor;
	ap_CollectorMotor = NULL;
	delete ap_Aimer;
	ap_Aimer = NULL;
}

void Smokey_VII::RobotInit(void){
//	SmartDashboard::init();
	printf("Running Smokey %i.%i.%i\n", 
			SMOKEY_MAJOR_VERSION,
			SMOKEY_MINOR_VERSION,
			SMOKEY_MICRO_VERSION);	
}

void Smokey_VII::TeleopInit(void)
{
	printf("Teleop Init\n");
	ap_Aimer->setEnabled(true);
	ap_Aimer->setAngle(90.0, 1.0); // set this and static init in periodic the same
	shooter_.Init(true);
	sonars_.EnableFrontOnly();
	sonars_.DisablePort(Sonar::kRightFront);
	sonars_.RxFlush();
	log_.InitLogging();
	//	ap_Drive->SetInvertedMotor(ap_Drive->kFrontRightMotor, false);
//	ap_Drive->SetInvertedMotor(ap_Drive->kFrontRightMotor, false);
//	SmartDashboard::PutNumber("P", 0.054);
//	SmartDashboard::PutNumber("I", 0.0);
//	SmartDashboard::PutNumber("D", 0.01);
//	SmartDashboard::PutNumber("Speed", 1.0); 
}

void Smokey_VII::DisabledInit()
{
	printf("Disabled\n");
	log_.DisableLogging();
	shooter_.Init(false);
	sonars_.RxFlush();
	m_indicator.SetColor(0, 0, 100);
}

void Smokey_VII::TeleopPeriodic(void)
{
	log_.SetEnabled(true);
	char logTemp[1024];
	
	static int time = 0;
	static double angle = 90.0; // set this and static init in init the same
	bool IncreaseCollector = joystick_.GetRawButton(COLLECTOR_POSITIVE_BUTTON);
	bool DecreaseCollector = joystick_.GetRawButton(COLLECTOR_NEGATIVE_BUTTON);

	
	sonars_.periodic();
	double sonarDistance = sonars_.GetFeet(Sonar::kLeftFront);	

	const int maxColor = 150;
	static int r = 0;
	static int g = 50;
	static int b = 100;
	static int ri = 3;
	static int gi = 2;
	static int bi = 1;
	if(joystick_.GetRawButton(9)) {
		r += ri;
		g += gi;
		b += bi;
		if(r > maxColor || r < 5) ri *= -1;
		if(g > maxColor || g < 5) gi *= -1;
		if(b > maxColor || b < 5) bi *= -1;
		m_indicator.SetColor(r, g, b);
	} else if(sonarDistance > 6.5) m_indicator.SetColor(0,0,0);
	else if (sonarDistance > 4.0) m_indicator.SetColor(0,100,0);
	else m_indicator.SetColor(100,0,0);
	
	sprintf(logTemp,"Front Left Sonar %f ft\n", sonars_.GetFeet(Sonar::kLeftFront));
	log_.Log(logTemp);
	sprintf(logTemp, "gyro: %f\n", gyro_.GetAngle());
	log_.Log(logTemp);
	SmartDashboard::PutNumber("Front Left Sonar", sonars_.GetFeet(Sonar::kLeftFront));
	SmartDashboard::PutNumber("Gyro Angle", gyro_.GetAngle());
	
	if(joystick_.GetRawButton(11)) 		a_fieldOrientated = false;
	else if (joystick_.GetRawButton(12)) a_fieldOrientated = true;

	try
	{
		if(joystick_.GetRawButton(9)) detector_.DetectHotGoal(true, true, true);
	}
	catch(std::runtime_error &ex)
	{
		printf("runtime_error: %s\n", ex.what());
	}
	
	if(joystick_.GetRawButton(4)) gyro_.Reset();
	
	ap_Drive->MecanumDrive_Cartesian(
			1 * joystick_.GetX(),
			1 * joystick_.GetY(),
			1 * joystick_.GetZ(),
		a_fieldOrientated ? gyro_.GetAngle() : 0);
	
	if(hedgyStick_.GetRawButton(6) && time > 50){
		angle += 10;
		time = 0;
	}
	if(hedgyStick_.GetRawButton(7) && time > 50){
		angle -= 10;
		time = 0;
	}
	if(hedgyStick_.GetRawButton(2)) angle = (Aimerino::DOWN);
	if(hedgyStick_.GetRawButton(5)) angle = (Aimerino::PARALLEL);
	if(hedgyStick_.GetRawButton(3)) angle = (Aimerino::ABOVESHOOT);
	if(hedgyStick_.GetRawButton(4)) angle = (Aimerino::UP);
	
	if(angle < -40) angle = -36;
	if(angle > 95) angle = 90;	
	ap_Aimer->setAngle(angle, 1.0);
//	ap_Aimer->setAngle(angle, SmartDashboard::GetNumber("Speed"));
	
	sprintf(logTemp, "Set Angle: %f", angle);
	log_.Log(logTemp);
	sprintf(logTemp, "Angle: %f", ap_Aimer->getAngle());
	log_.Log(logTemp);
	
	if(IncreaseCollector == DecreaseCollector)	ap_CollectorMotor->Set(0);
	else if(IncreaseCollector)					ap_CollectorMotor->Set(1.0);	
	else if(DecreaseCollector)					ap_CollectorMotor->Set(-1.0);	

	(angle < 80) ? 	sprintf(logTemp, "Shooting Enabled") :
					sprintf(logTemp, "Shooting Disabled");
	log_.Log(logTemp);
	shooter_.SetEnabled(angle < 81);
	shooter_.UpdateControlLogic(joystick_.GetRawButton(1) && angle > -5,
									joystick_.GetRawButton(2) && angle > -5);

	time ++;
	
/* Calibration Routine
	if(joystick_.GetRawButton(1))
	{
		ap_CallibratingMotor->Set(0);
	}
	else
	{
		ap_CallibratingMotor->Set(joystick_.GetY());
	}
	printf("%f\n", ap_CallibratingMotor->Get());	
*/
}

void Smokey_VII::AutonomousInit(void){
	m_currentState = kAutonDelay;
	sonars_.RxFlush();
	sonars_.EnableFrontOnly();
	sonars_.DisablePort(Sonar::kRightFront);
	ap_Aimer->setEnabled(true);
	shooter_.Init(true);
	gyro_.Reset();

	// Start logging
	log_.InitLogging();

	// Turn off the LED at the beginning of autonomous
	m_indicator.SetColor(0, 0, 0);

	//	ap_Drive->SetInvertedMotor(ap_Drive->kFrontRightMotor, true);
//	ap_Drive->SetInvertedMotor(ap_Drive->kFrontRightMotor, true);
}

void Smokey_VII::AutonomousPeriodic(void){
	log_.SetEnabled(true);
	
	AutonState nextState;
	bool drivn = false;
	char logTemp[1024];
	sonars_.periodic();
	shooter_.UpdateControlLogic(false, false);
		
	switch(m_currentState){
	case kAutonDelay:
		if(detector_.DetectHotGoal(true, true, true) || true) {
			nextState = kAutonDriveAndTilt;
		} else {
			m_timer.Reset();
			m_timer.Start();
			nextState = kAutonWaitForHotZone;
		}
		break;
	case kAutonWaitForHotZone:
		if(m_timer.HasPeriodPassed(5.0)) {
			m_timer.Stop();
			m_timer.Reset();
			nextState = kAutonDriveAndTilt;
		}
		break;
	case kAutonDriveAndTilt:
	{
		ap_Aimer->setAngle(50, 1.0);
		sprintf(logTemp, "Sonars: %f", sonars_.GetDistanceFront());
		log_.Log(logTemp);
		if(sonars_.GetDistanceFront() > 12.2) { //8.7 = 6'4"
			double gyroOffset = gyro_.GetAngle() / 90.0;
			ap_Drive->SetLeftRightMotorOutputs(0.4 - gyroOffset, -1 * (0.4 + gyroOffset));
			drivn = true;
		} else {
			m_indicator.SetColor(0, 100, 0);
			if(fabs(ap_Aimer->getAngle() - 50) < 5) {
				nextState = kAutonShoot;
				shooter_.SetEnabled(true);
				shooter_.UpdateControlLogic(true, false);
				log_.Log("SHOOTING");
			}
		}
	}
		break;
	case kAutonShoot:
		if(shooter_.GetState() == SHOOTER_STATE_IDLE){
			nextState = kAutonNULL;
		}
		break;
		default:
	case kAutonNULL:
		break;
	}
	m_currentState = nextState;
	if(!drivn) ap_Drive->MecanumDrive_Cartesian(0,0,0);	
}

void Smokey_VII::DisabledPeriodic(){
	sonars_.periodic();
	//printf("Gyro: %f\n", gyro_.GetAngle());
}

START_ROBOT_CLASS(Smokey_VII);


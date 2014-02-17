#include <IterativeRobot.h>
#include <Gyro.h>
#include <Talon.h>
#include <Joystick.h>
#include <RobotDrive.h>
#include <DigitalInput.h>

#include "Smokey_VII.h"

#include "CompPrefs.h"
#include "Aimerino.h"
#include "Shooter.h"
#include "Sonar.h"

Smokey_VII::Smokey_VII(void){
	ap_Gyro = new Gyro(GYRO_PORT);
	ap_Joystick = new Joystick(JOYSTICK_PORT);
	ap_FLmotor = new Talon(FL_PORT);
	ap_FRmotor = new Talon(FR_PORT);
	ap_BLmotor = new Talon(BL_PORT);
	ap_BRmotor = new Talon(BR_PORT);
//	ap_Drive = new RobotDrive(ap_FLmotor, ap_BLmotor, ap_FRmotor, ap_BRmotor);
//	ap_Drive->SetInvertedMotor(ap_Drive->kFrontRightMotor, true);
//	ap_Drive->SetInvertedMotor(ap_Drive->kRearRightMotor, true);
	ap_CollectorMotor = new Talon(COLLECTOR_PORT);
	ap_Aimer = new Aimerino(AIMER_PORT, POT_PORT, 90.0 / (4.77 - 3.03), 3.03);

	ap_Shooter = new Shooter(SHOOTER_PORT, MAG_SENSOR_PORT);
	ap_Shooter->SetVerbose(true);
	ap_Sonars = new Sonar(SONAR_BAUD_RATE);

	ap_CallibratingMotor = new Talon(10);
}

Smokey_VII::~Smokey_VII(void)
{
	delete ap_Gyro;
	ap_Gyro = NULL;
	delete ap_Joystick;
	ap_Joystick = NULL;
	delete ap_FLmotor;
	ap_FLmotor = NULL;
	delete ap_FRmotor;
	ap_FRmotor = NULL;
	delete ap_BLmotor;
	ap_BLmotor = NULL;
	delete ap_BRmotor;
	ap_BRmotor = NULL;
//	delete ap_Drive;
//	ap_Drive = NULL;
	delete ap_CollectorMotor;
	ap_CollectorMotor = NULL;
	delete ap_Aimer;
	ap_Aimer = NULL;
	delete ap_Shooter;
	ap_Shooter = NULL;
	delete ap_CallibratingMotor;
	delete ap_Sonars;
	ap_Sonars = NULL;
}

void Smokey_VII::RobotInit(void){
	printf("Running Smokey %i.%i.%i\n", 
			SMOKEY_MAJOR_VERSION,
			SMOKEY_MINOR_VERSION,
			SMOKEY_MICRO_VERSION);	
}

void Smokey_VII::TeleopInit(void)
{
	printf("Teleop Init");
	ap_Aimer->setEnabled(true);
	//ap_Aimer->setAngle(45);
	ap_Shooter->Enable();
	ap_Sonars->EnableFrontOnly();
}

void Smokey_VII::TeleopPeriodic(void){

<<<<<<< HEAD
	// Calibration Routine
=======
	ap_Sonars->Periodic();
	printf("Front Left Sonar %f ft\n", ap_Sonars->GetFeet(kLeftFront));
/*	if(ap_Joystick->GetRawButton(4)) ap_Gyro->Reset();
	ap_Drive->MecanumDrive_Cartesian(
	   .5 * ap_Joystick->GetX(), 
	   .5 * ap_Joystick->GetY(),
       .5 * ap_Joystick->GetZ(), 
            ap_Gyro->GetAngle());
	*/
	
	/* Calibration Routine
>>>>>>> origin/master
	if(ap_Joystick->GetRawButton(1))
	{
		ap_CallibratingMotor->Set(0);
	}
	else
	{
		ap_CallibratingMotor->Set(ap_Joystick->GetY());
	}
	printf("%f\n", ap_CallibratingMotor->Get());
		
	
/*	if(ap_Joystick->GetRawButton(4)) ap_Gyro->Reset();
	ap_Drive->MecanumDrive_Cartesian(
	   .5 * ap_Joystick->GetX(), 
	   .5 * ap_Joystick->GetY(),
       .5 * ap_Joystick->GetZ(), 
            ap_Gyro->GetAngle());
	*/
	
/*	printf("Angle: %f\n", ap_Aimer->getAngle());
	if(ap_Joystick->GetRawButton(7)) ap_Aimer->setAngle(Aimerino::DOWN);
	if(ap_Joystick->GetRawButton(8)) ap_Aimer->setAngle(Aimerino::PARALLEL);
	if(ap_Joystick->GetRawButton(9)) ap_Aimer->setAngle(Aimerino::BELOWSHOOT);
	if(ap_Joystick->GetRawButton(10)) ap_Aimer->setAngle(Aimerino::SHOOT);
	if(ap_Joystick->GetRawButton(11)) ap_Aimer->setAngle(Aimerino::ABOVESHOOT);
	if(ap_Joystick->GetRawButton(12)) ap_Aimer->setAngle(Aimerino::UP);

	bool positive = ap_Joystick->GetRawButton(COLLECTOR_POSITIVE_BUTTON);
	bool negative = ap_Joystick->GetRawButton(COLLECTOR_NEGATIVE_BUTTON);
	
	if(positive && negative) ap_CollectorMotor->Set(0);
	else if(positive) ap_CollectorMotor->Set(0.6);	
	else if(negative) ap_CollectorMotor->Set(-0.6);	
	else ap_CollectorMotor->Set(0);
	
	// static bool shooting;
	// if(ap_MagSensor->Get() == 0) shooting = false;
	// if(ap_Joystick->GetRawButton(1)) shooting = true;
	// (shooting) ? ap_Shooter->Set(1.0) : ap_Shooter->Set(0.0);
	// printf("MagSensor: %d\n", ap_MagSensor->Get());
	ap_Shooter->UpdateControlLogic(*ap_Joystick);

	if(ap_Joystick->GetRawButton(2)) ap_Aimer->setEnabled(false);
	if(ap_Joystick->GetRawButton(6)) ap_Aimer->setEnabled(true);
*/}

void Smokey_VII::TeleopContinuous(void)
{
}

void Smokey_VII::TestPeriodic(void)
{

	
}

START_ROBOT_CLASS(Smokey_VII);


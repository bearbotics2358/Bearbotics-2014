#include <IterativeRobot.h>
#include <Gyro.h>
#include <Talon.h>
#include <Joystick.h>
#include <RobotDrive.h>
#include <DigitalInput.h>
#include <SmartDashboard/SmartDashboard.h>
#include <math.h>

#include "Smokey_VII.h"

#include "Prefs.h"
#include "Aimerino.h"
#include "Shooter.h"
#include "Sonar.h"

Smokey_VII::Smokey_VII(void)
{
	ap_Gyro = new Gyro(GYRO_PORT);
	ap_Joystick = new Joystick(JOYSTICK_PORT);
	ap_HedgyStick = new Joystick(HEDGYSTICK_PORT);
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
	ap_Shooter = new Shooter(SHOOTER_PORT, MAG_SENSOR_PORT);
	ap_Shooter->SetVerbose(true);
	ap_Sonars = new Sonar(SONAR_BAUD_RATE);
	ap_CallibratingMotor = new Talon(10);
	a_fieldOrientated = false;
	a_currentState = 0;
	ap_states[0] = kTestMoveTo60;
	ap_states[1] = kTestArm;
	ap_states[2] = kTestCollect;
	ap_states[3] = kTestShoot;

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
	delete ap_Drive;
	ap_Drive = NULL;
	delete ap_CollectorMotor;
	ap_CollectorMotor = NULL;
	delete ap_Aimer;
	ap_Aimer = NULL;
	delete ap_Shooter;
	ap_Shooter = NULL;
	delete ap_CallibratingMotor;
	ap_CallibratingMotor = NULL;
	delete ap_Sonars;
	ap_Sonars = NULL;
	delete ap_HedgyStick;
	ap_HedgyStick = NULL;
	
}

void Smokey_VII::RobotInit(void){
	SmartDashboard::init();
	printf("Running Smokey %i.%i.%i\n", 
			SMOKEY_MAJOR_VERSION,
			SMOKEY_MINOR_VERSION,
			SMOKEY_MICRO_VERSION);	
}

void Smokey_VII::TeleopInit(void)
{
	printf("Teleop Init\n");
	ap_Aimer->setEnabled(true);
	ap_Shooter->Init(true);
	ap_Sonars->EnableFrontOnly();
	ap_Sonars->RxFlush();
}

void Smokey_VII::DisabledInit()
{
	printf("Disabled\n");
	ap_Shooter->Init(false);
	ap_Sonars->RxFlush();
}

void Smokey_VII::TeleopPeriodic(void){
	static int time = 0;
	static double angle = 0;
	bool IncreaseCollector = ap_Joystick->GetRawButton(COLLECTOR_POSITIVE_BUTTON);
	bool DecreaseCollector = ap_Joystick->GetRawButton(COLLECTOR_NEGATIVE_BUTTON);

	ap_Sonars->periodic();
	printf("Front Left Sonar %f ft\n", ap_Sonars->GetFeet(Sonar::kLeftFront));
	printf("gyro: %f\n", ap_Gyro->GetAngle());
	
	if(ap_Joystick->GetRawButton(11)) 		a_fieldOrientated = false;
	else if (ap_Joystick->GetRawButton(12)) a_fieldOrientated = true;
	
	if(ap_Joystick->GetRawButton(4)) ap_Gyro->Reset();
	
	ap_Drive->MecanumDrive_Cartesian(
			1 * ap_Joystick->GetX(),
			1 * ap_Joystick->GetY(),
			1 * ap_Joystick->GetZ(),
		a_fieldOrientated ? ap_Gyro->GetAngle() : 0);
	
	if(ap_HedgyStick->GetRawButton(6) && time > 50){
		angle += 10;
		time = 0;
	}
	if(ap_HedgyStick->GetRawButton(7) && time > 50){
		angle -= 10;
		time = 0;
	}
	if(ap_HedgyStick->GetRawButton(2)) angle = (Aimerino::DOWN);
	if(ap_HedgyStick->GetRawButton(5)) angle = (Aimerino::PARALLEL);
	if(ap_HedgyStick->GetRawButton(3)) angle = (Aimerino::ABOVESHOOT);
	if(ap_HedgyStick->GetRawButton(4)) angle = (Aimerino::UP);
	
	if(angle < -40) angle = -36;
	if(angle > 95) angle = 90;	
	ap_Aimer->setAngle(angle);
	
	printf("Set Angle: %f\n", angle);
	printf("Angle: %f\n", ap_Aimer->getAngle());
	
	
	if(IncreaseCollector == DecreaseCollector)	ap_CollectorMotor->Set(0);
	else if(IncreaseCollector)					ap_CollectorMotor->Set(1.0);	
	else if(DecreaseCollector)					ap_CollectorMotor->Set(-1.0);	
-	

	(angle < 80) ? 	printf("Shooting Enabled\n") :
					printf("Shooting Disabled\n");
	ap_Shooter->SetEnabled(angle < 80);
	ap_Shooter->UpdateControlLogic(ap_Joystick->GetRawButton(1) && angle > -5,
									ap_Joystick->GetRawButton(2) && angle > -5);

	time ++;
	
	/* Calibration Routine
	if(ap_Joystick->GetRawButton(1))
	{
		ap_CallibratingMotor->Set(0);
	}
	else
	{
		ap_CallibratingMotor->Set(ap_Joystick->GetY());
	}
	printf("%f\n", ap_CallibratingMotor->Get());	
*/

}

void Smokey_VII::AutonomousInit(void){
	a_currentState = 0;
	ap_Sonars->RxFlush();
	 
}

void Smokey_VII::AutonomousPeriodic(void){
	static int time = 0;
	AutonState currentState;
	
	ap_Drive->MecanumDrive_Cartesian(0,0,0);
	ap_Shooter->UpdateControlLogic(false, false);
//	ap_Sonars->periodic();
	
	if(a_currentState < sizeof(ap_states)){
	
		currentState = ap_states[a_currentState];
		
	
		switch(currentState){
		case kAutonIdle:
			if(time > 50){
				a_currentState ++;
			}
			break;
		case kAutonDriveForwards:
			ap_Aimer->setAngle(60);
			ap_Drive->MecanumDrive_Cartesian(0, 0.5, 0, ap_Gyro->GetAngle());
			if(ap_Sonars->GetDistanceFront()<= 198.12){
				a_currentState ++;
			}	
			break;
		case kAutonShoot:
			ap_Shooter->UpdateControlLogic(true, false);
			a_currentState ++;
			break;
		case kTestMoveTo60:
			ap_Aimer->setAngle(60);
			if(fabs(ap_Aimer->getAngle() - 60) < 5){
				a_currentState ++;
				time = 0;
			}
			break;
		case kTestArm:
			ap_Shooter->UpdateControlLogic(true, false);
			if(ap_Shooter->GetState() == SHOOTER_STATE_IDLE){
				a_currentState ++;
				time = 0;
			}
			break;
		case kTestCollect:
			ap_CollectorMotor->Set(1);
			if(time > 100){
				time = 0;
				a_currentState ++;
			}
			break;
		case kTestShoot:
			ap_Shooter->UpdateControlLogic(true, false);
			if(ap_Shooter->GetState() == SHOOTER_STATE_IDLE){
				a_currentState ++;
				time = 0;
			}
			break;
		case kAutonNULL:
			break;
		}
	}	
	time ++;
}

void Smokey_VII::DisabledPeriodic(){
	ap_Sonars->periodic();
}

START_ROBOT_CLASS(Smokey_VII);


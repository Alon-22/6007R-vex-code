#pragma config(Sensor, in1,    pwrExpBat,      sensorAnalog)
#pragma config(Sensor, in2,    Gyro,           sensorGyro)
#pragma config(Sensor, dgtl1,  ,               sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  LeftDriveE,     sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  RightDriveE,    sensorQuadEncoder)
#pragma config(Sensor, dgtl11, trackingwheel,  sensorQuadEncoder)
#pragma config(Motor,  port2,           DriveL1,       tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           DriveL2,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           Descorer,      tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           Motor_FW1,     tmotorVex393HighSpeed_MC29, openLoop, encoderPort, dgtl1)
#pragma config(Motor,  port6,           Motor_FW2,     tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, dgtl1)
#pragma config(Motor,  port7,           Intake,        tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port8,           DriveR2,       tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, dgtl9)
#pragma config(Motor,  port9,           DriveR1,       tmotorVex393HighSpeed_MC29, openLoop, encoderPort, dgtl9)
#pragma config(Motor,  port10,          Indexer,       tmotorVex393TurboSpeed_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX2)

#pragma competitionControl(Competition)


#include "Vex_Competition_Includes.c"


// Custom Rolling Robots Functions
/*===========================================================
Flywheel Functions
===========================================================*/
//float rpm=0;
//float speed=0;
//float error=0;
//float FlyTarget=2400;
//float ticks2;
//task flyRpm()
//{
//	float ticks1;
//	while(true){
//		SensorValue(FlyEnc) = 0;

//		wait1Msec(20); // if this  changes change rpm equation
//		ticks2=SensorValue(FlyEnc);
//		rpm=(50/3)*(ticks2);  //50/3 converts to rpm 50/3)*
//}
//}

/*==========================================================
TAKE BACK HALF CODE
==========================================================*/
	// Update inteval (in mS) for the flywheel control loop
#define FW_LOOP_SPEED              25

// Maximum power we want to send to the flywheel motors
#define FW_MAX_POWER              127

// encoder counts per revolution depending on motor
#define MOTOR_TPR_269           240.448
#define MOTOR_TPR_393R          261.333
#define MOTOR_TPR_393S          392
#define MOTOR_TPR_393T          627.2
#define MOTOR_TPR_QUAD          360.0

// encoder tick per revolution
float           ticks_per_rev;          ///< encoder ticks per revolution

// Encoder
long            encoder_counts;         ///< current encoder count
long            encoder_counts_last;    ///< current encoder count

// velocity measurement
float           motor_velocity;         ///< current velocity in rpm
long            nSysTime_last;          ///< Time of last velocity calculation

// TBH control algorithm variables
long            target_velocity;        ///< target_velocity velocity
float           current_error;          ///< error between actual and target_velocity velocities
float           last_error;             ///< error last time update called
float           gain;                   ///< gain
float           drive;                  ///< final drive out of TBH (0.0 to 1.0)
float           drive_at_zero;          ///< drive at last zero crossing
long            first_cross;            ///< flag indicating first zero crossing
float           drive_approx;           ///< estimated open loop drive

// final motor drive
long            motor_drive;            ///< final motor control value

/*-----------------------------------------------------------------------------*/
/** @brief      Set the flywheen motors                                        */
/** @param[in]  value motor control value                                      */
/*-----------------------------------------------------------------------------*/
void
FwMotorSet( int value )
{
    motor[ Motor_FW1 ] = value;
    motor[ Motor_FW2 ] = value;
    //motor[ Motor_FW3 ] = value;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Get the flywheen motor encoder count                           */
/*-----------------------------------------------------------------------------*/
long
FwMotorEncoderGet()
{
    return( nMotorEncoder[ Motor_FW1 ] );
}

/*-----------------------------------------------------------------------------*/
/** @brief      Set the controller position                                    */
/** @param[in]  desired velocity                                               */
/** @param[in]  predicted_drive estimated open loop motor drive                */
/*-----------------------------------------------------------------------------*/
void
FwVelocitySet( int velocity, float predicted_drive )
{
    // set target_velocity velocity (motor rpm)
    target_velocity = velocity;

    // Set error so zero crossing is correctly detected
    current_error = target_velocity - motor_velocity;
    last_error    = current_error;

    // Set predicted open loop drive value
    drive_approx  = predicted_drive;
    // Set flag to detect first zero crossing
    first_cross   = 1;
    // clear tbh variable
    drive_at_zero = 0;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Calculate the current flywheel motor velocity                  */
/*-----------------------------------------------------------------------------*/
void
FwCalculateSpeed()
{
    int     delta_ms;
    int     delta_enc;

    // Get current encoder value
    encoder_counts = FwMotorEncoderGet();

    // This is just used so we don't need to know how often we are called
    // how many mS since we were last here
    delta_ms = nSysTime - nSysTime_last;
    nSysTime_last = nSysTime;

    // Change in encoder count
    delta_enc = (encoder_counts - encoder_counts_last);

    // save last position
    encoder_counts_last = encoder_counts;

    // Calculate velocity in rpm
    motor_velocity = (1000.0 / delta_ms) * delta_enc * 60.0 / ticks_per_rev;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Update the velocity tbh controller variables                   */
/*-----------------------------------------------------------------------------*/
void
FwControlUpdateVelocityTbh()
{
    // calculate error in velocity
    // target_velocity is desired velocity
    // current is measured velocity
    current_error = target_velocity - motor_velocity;

    // Calculate new control value
    drive =  drive + (current_error * gain);

    // Clip to the range 0 - 1.
    // We are only going forwards
    if( drive > 1 )
          drive = 1;
    if( drive < 0 )
          drive = 0;

    // Check for zero crossing
    if( sgn(current_error) != sgn(last_error) ) {
        // First zero crossing after a new set velocity command
        if( first_cross ) {
            // Set drive to the open loop approximation
            drive = drive_approx;
            first_cross = 0;
        }
        else
            drive = 0.5 * ( drive + drive_at_zero );

        // Save this drive value in the "tbh" variable
        drive_at_zero = drive;
    }

    // Save last error
    last_error = current_error;
}

/*-----------------------------------------------------------------------------*/
/** @brief     Task to control the velocity of the flywheel                    */
/*-----------------------------------------------------------------------------*/
task
FwControlTask()
{
    // Set the gain
    gain = 0.00025;

    // We are using Speed geared motors
    // Set the encoder ticks per revolution
    ticks_per_rev = MOTOR_TPR_393S;

    while(1)
        {
        // Calculate velocity
        FwCalculateSpeed();

        // Do the velocity TBH calculations
        FwControlUpdateVelocityTbh() ;

        // Scale drive into the range the motors need
        motor_drive  = (drive * FW_MAX_POWER) + 0.5;

        // Final Limit of motor values - don't really need this
        if( motor_drive >  127 ) motor_drive =  127;
        if( motor_drive < -127 ) motor_drive = -127;

        // and finally set the motor control value
        FwMotorSet( motor_drive );

        // Run at somewhere between 20 and 50mS
        wait1Msec( FW_LOOP_SPEED );
        }
      }


/*===========================================================
trueSpeed Functions
===========================================================*/
/**
* Converts from output power to PWM value, in order to linearize motor output.
*
* @param power  Desired percentage of max power, between -1 and 1.
* @return PWM value that will come closest to achieving the desired percentage.
*/
int powerToPwm(float power) {
	if (power == 0) {
		return 0;
	}
	float p = fabs(power);
	if (p >= 1) {
		return 127 * sgn(power);
	}
	return ((((((((-44128.10541 * p + 178572.6802) * p - 297071.4563) * p
	+ 262520.7547) * p - 132692.6561) * p + 38464.48054) * p - 6049.717501) * p
	+ 476.2279947) * p - 1.233957961) * sgn(power);
}
//LINEAR DRIVE VARIABLES
int threshold = 15;
int speedLeft;
int speedRight;
string rightDrive= "rightDrive";
string leftDrive = "leftDrive";
//SETSPEED FUNCTION
void setSpeed(char* motorName,int speed){
	int absSpeed = powerToPwm(speed / 127.0);
	if(motorName == "rightDrive") {
		motor[DriveR1] = absSpeed;
		motor[DriveR2] = absSpeed;
	}
	else if(motorName == "leftDrive"){
		motor[DriveL1] = absSpeed;
		motor[DriveL2] = absSpeed;
	}
}
//LEFTDRIVEMOTORS FUNCTION
void leftDriveMotors(int speed){

	setSpeed("leftDrive",speed);

}

//RIGHTDRIVEMOTORS FUNCTION
void rightDriveMotors(int speed){

	setSpeed("rightDrive",speed);

}

void move(int speed, int rotate, int dime, int offset)
{

	if(speed <=0)
	{
		motor[DriveR1] = speed+offset;
		motor[DriveR2] = speed+offset;
		motor[DriveL1] = speed;
		motor[DriveL2] = speed;
	}
	else if(speed >= 0)
	{
		motor[DriveR1] = speed+offset;
		motor[DriveR2] = speed+offset;
		motor[DriveL1] = speed;
		motor[DriveL2] = speed;
	}
	//while(abs(SensorValue[RightDriveE]) <= rotate){} //&& (abs(SensorValue[LeftDriveE]) <= rotate ){}
	//if(speed > 0 && dime == 1)
	//{
	//	motor[DriveR1] = 30;
	//	motor[DriveR2] = 30;
	//	motor[DriveL1] = 30;
	//	motor[DriveL2] = 30;
	//	wait1Msec(200);
	//	motor[DriveR1] = 0;
	//	motor[DriveR2] = 0;
	//	motor[DriveL1] = 0;
	//	motor[DriveL2] = 0;
	//}
	//else if(speed < 0 && dime == 1)
	//{
	//	motor[DriveR1] = 30;
	//	motor[DriveR2] = 30;
	//	motor[DriveL1] = 30;
	//	motor[DriveL2] = 30;
	//	wait1Msec(200);
	//	motor[DriveR1] = 0;
	//	motor[DriveR2] = 0;
	//	motor[DriveL1] = 0;
	//	motor[DriveL2] = 0;
	//}
	//else if(dime ==0)
	//{
	//	motor[DriveR1] = 0;
	//	motor[DriveR2] = 0;
	//	motor[DriveL1] = 0;
	//	motor[DriveL2] = 0;
	//}
}
void rotate(bool right, float degrees){
	if(right == true){
		motor[DriveR1] = 127;
		motor[DriveR2] = 127;
		motor[DriveL1] = -127;
		motor[DriveL2] = -127;
		waitUntil(SensorValue[Gyro]==degrees);
		motor[DriveR1] = 0;
		motor[DriveR2] = 0;
		motor[DriveL1] = 0;
		motor[DriveL2] = 0;

		}if(right == false){
		motor[DriveR1] = -127;
		motor[DriveR2] = -127;
		motor[DriveL1] = 127;
		motor[DriveL2] = 127;
		waitUntil(SensorValue[Gyro]==degrees);
		motor[DriveR1] = 0;
		motor[DriveR2] = 0;
		motor[DriveL1] = 0;
		motor[DriveL2] = 0;
	}
}
void rotateL(int speed, int rotation)
{


	motor[DriveR1] = speed;
	motor[DriveR2] = speed;
	motor[DriveL1] = -speed;
	motor[DriveL2] = -speed;

	while(abs(SensorValue[RightDriveE]) <= rotation){}
	motor[DriveR1] = -20;
	motor[DriveR2] = -20;
	motor[DriveL1] = 20;
	motor[DriveL2] = 20;
	wait1Msec(100);
	motor[DriveR1] = 0;
	motor[DriveR2] = 0;
	motor[DriveL1] = 0;
	motor[DriveL2] = 0;
}

void rotateR(int speed, int rotation)
{

	motor[DriveR1] = -speed;
	motor[DriveR2] = -speed;
	motor[DriveL1] = speed;
	motor[DriveL2] = speed;

	while(abs(SensorValue[RightDriveE]) <= rotation){}
	motor[DriveR1] = -20;
	motor[DriveR2] = -20;
	motor[DriveL1] = 20;
	motor[DriveL2] = 20;
	wait1Msec(100);
	motor[DriveR1] = 0;
	motor[DriveR2] = 0;
	motor[DriveL1] = 0;
	motor[DriveL2] = 0;
}
/*===========================================================
ticks to feet functions
===========================================================*/
#define wheelDiameter            3.25  // Inches.
#define ticksPerRev              216   // Quadrature Encoder.
#define inchesToTicks(RightdriveE)      ((RightdriveE) * (ticksPerRev / (PI * wheelDiameter)))
#define driveDiameter         17.5  // Inches.
#define degreesToTicks(RightdriveE)     inchesToTicks((RightdriveE) * ((driveDiameter / 2.0) * (PI / 180.0)))
#define ticksToInches(RightdriveE)      ((RightdriveE) / (ticksPerRev / (PI * wheelDiameter)))
#define ticksToDegrees(RightdriveE)     ticksToInches(RightdriveE) / ((driveDiameter / 2.0) * (PI / 180.0))
/*==========================================================
LCD CODE
==========================================================*/
#define RED		0
#define BLUE	1

#define FRONT		0
#define BACK		1

#define ALLIANCE_COLOR_SCREEN		0
#define STARTING_TILE_SCREEN		1
#define BATTERY_SCREEN       		2
#define AUTONOMOUS_MENU      		3
#define CONFIRMATION_SCREEN  		4

const string autonRoutines[5] = {"Big Boi", "Smol boi", "11 point boi", "back to front boi", "testing boi"};

int allianceColor = -1;
int startingTile = -1;
int autonomousRoutine = -1;

int allianceColorScreen() {
	int newScreen;

	displayLCDString(0, 0, " Alliance Color ");
	displayLCDString(1, 0, "Red   Back  Blue");

	switch (nLCDButtons) {
	case kButtonLeft:
		allianceColor = RED;
		newScreen = STARTING_TILE_SCREEN;
		break;

	case kButtonCenter:
		newScreen = BATTERY_SCREEN;
		break;

	case kButtonRight:
		allianceColor = BLUE;
		newScreen = STARTING_TILE_SCREEN;
		break;

	default:
		newScreen = ALLIANCE_COLOR_SCREEN;
		break;
	}
	while (nLCDButtons != kButtonNone) {
		wait1Msec(20);
	}
	return newScreen;
}

int startingTileScreen() {
	int newScreen;

	displayLCDString(0, 0, " Starting  Tile ");
	displayLCDString(1, 0, "Left  Back Right");

	switch (nLCDButtons) {
	case kButtonLeft:
		startingTile = FRONT;
		newScreen = AUTONOMOUS_MENU;
		break;

	case kButtonCenter:
		newScreen = ALLIANCE_COLOR_SCREEN;
		break;

	case kButtonRight:
		startingTile = BACK;
		newScreen = AUTONOMOUS_MENU;
		break;

	default:
		newScreen = STARTING_TILE_SCREEN;
		break;
	}
	while (nLCDButtons != kButtonNone) {
		wait1Msec(20);
	}
	return newScreen;
}

int batteryScreen() {
	int newScreen;
	string str0, str1;

	sprintf(str0, "Crtx:%.1fV TV:%lfV", nAvgBatteryLevel / 1000.0, target_velocity,);
	sprintf(str1, "PE:%.1fV  AV:%.1fV", SensorValue[pwrExpBat] / 1000.0, motor_velocity,);

	displayLCDString(0, 0, str0);
	displayLCDString(1, 0, str1);

	switch (nLCDButtons) {
	case kButtonLeft:
		newScreen = ALLIANCE_COLOR_SCREEN;
		break;

	case kButtonCenter:
		newScreen = ALLIANCE_COLOR_SCREEN;
		break;

	case kButtonRight:
		newScreen = ALLIANCE_COLOR_SCREEN;
		break;

	default:
		newScreen = BATTERY_SCREEN;
		break;
	}
	while (nLCDButtons != kButtonNone) {
		wait1Msec(20);
	}
	return newScreen;
}

int autonomousMenu() {
	const int maxAutonNum = sizeof(autonRoutines) / sizeof(autonRoutines[0]) - 1;

	int autonNum = -1;

	while (true) {
		if (autonNum == -1) {
			clearLCDLine(0);
			displayLCDString(1, 0, " <    Back    > ");
			} else {
			displayLCDCenteredString(0, autonRoutines[autonNum]);
			displayLCDString(1, 0, " <   Select   > ");
		}

		switch (nLCDButtons) {
		case kButtonLeft:
			if (autonNum == -1) {
				autonNum = maxAutonNum;
				} else {
				autonNum--;
			}
			break;

		case kButtonCenter:
			if (autonNum == -1) {
				while (nLCDButtons != kButtonNone) {
					wait1Msec(20);
				}
				return STARTING_TILE_SCREEN;
				} else {
				autonomousRoutine = autonNum;

				while (nLCDButtons != kButtonNone) {
					wait1Msec(20);
				}
				return CONFIRMATION_SCREEN;
			}
			break;

		case kButtonRight:
			if (autonNum == maxAutonNum) {
				autonNum = -1;
				} else {
				autonNum++;
			}
			break;

		default:
			break;
		}
		while (nLCDButtons != kButtonNone) {
			wait1Msec(20);
		}
		wait1Msec(20);
	}
}

int confirmationScreen() {
	int newScreen;
	string s, colorString, tileString;

	if (allianceColor == RED) {
		colorString = "Red ";
		} else {
		colorString = "Blue";
	}
	if (startingTile == FRONT) {
		tileString = " Left";
		} else {
		tileString = "Right";
	}
	sprintf(s, "%s  Back %s", colorString, tileString);

	displayLCDCenteredString(0, autonRoutines[autonomousRoutine]);
	displayLCDString(1, 0, s);

	switch (nLCDButtons) {
	case kButtonCenter:
		newScreen = AUTONOMOUS_MENU;
		break;

	default:
		newScreen = CONFIRMATION_SCREEN;
		break;
	}
	while (nLCDButtons != kButtonNone) {
		wait1Msec(20);
	}
	return newScreen;
}

task lcdTask() {
	int screenNum = BATTERY_SCREEN;

	clearLCDLine(0);
	clearLCDLine(1);
	bLCDBacklight = true;

	while (true) {
		switch (screenNum) {
		case ALLIANCE_COLOR_SCREEN:
			screenNum = allianceColorScreen();
			break;

		case STARTING_TILE_SCREEN:
			screenNum = startingTileScreen();
			break;

		case BATTERY_SCREEN:
			screenNum = batteryScreen();
			break;

		case AUTONOMOUS_MENU:
			screenNum = autonomousMenu();
			break;

		case CONFIRMATION_SCREEN:
			screenNum = confirmationScreen();
			break;

		default:
			break;
		}
		wait1Msec(20);
	}
}

/*===========================================================
pre_auton
===========================================================*/
void pre_auton()
{
	bStopTasksBetweenModes = true;
	startTask(lcdTask);
}
/*===========================================================
Autonomous
===========================================================*/
task autonomous()
{
	if (autonomousRoutine == 0) {
	displayLCDString(0, 0, "Big Boi");
		bool red = allianceColor == RED;
		motor[Motor_FW1]=127;
		motor[Motor_FW2]=127;
		wait1Msec(2000);
		motor[Motor_FW1]=127;
		motor[Motor_FW2]=127;
		motor[Indexer]=-127;
		motor[Intake]=-127;
		wait1Msec(1000);
		if(red == false){
			rotate(true,90);
		}
		else{
			rotate(false,90);
		}
		wait1Msec(500);
		motor[Indexer]=0;
		wait1Msec(1000);
		move(90,0,0,0);
		wait1Msec(1500);
		move(0,0,0,0);
		wait1Msec(500);
		move(-95,0,0,0);
		wait1Msec(1550);
		move(0,0,0,0);
		wait1Msec(500);
		if(red == false){
			rotate(false,90);
		}
		else{
			rotate(true,90);
		}
		wait1Msec(500);
		move(60,0,0,0);
		wait1Msec(1000);
		move(0,0,0,0);
		motor[Indexer] = -127;
		wait1Msec(500);
		move(60,0,0,0);
		wait1Msec(500);
		move(-120,0,0,0);
		wait1Msec(1500);
		move(0,0,0,0);
		wait1Msec(15000);
		} else if (autonomousRoutine == 1) {
			displayLCDString(0, 0, "smol Boi");
		motor[Motor_FW1]=127;
		motor[Motor_FW1]=127;
		wait1Msec(2000);
		move(60,0,0,0);
		wait1Msec(500);
		move(0,0,0,0);
		wait1Msec(500);
		motor[Motor_FW1]=127;
		motor[Motor_FW1]=127;
		motor[Intake]=-127;
		motor[Indexer]=-127;
		wait1Msec(1000);
		motor[Indexer]=0;
		wait1Msec(500);
		//comment here
		move(60,0,0,0);
		wait1Msec(1000);
		move(0,0,0,0);
		motor[Indexer]=127;
		wait1Msec(500);
		move(60,0,0,0);
		//comment here
		move(60,0,0,0);
		wait1Msec(1500);
		move(0,0,0,0);
		wait1Msec(500);
		move(-120,0,0,0);
		wait1Msec(500);
		move(0,0,0,0);
		wait1Msec(15000);
	}
else	if (autonomousRoutine == 2) {
			displayLCDString(0, 0, "back Boi");
		motor[Intake]=127;
		move(100,0,0,0);
		wait1Msec(700);
		move(0,0,0,0);
		wait1Msec(500);
		move(-100,0,0,0);
		wait1Msec(500);
		move(0,0,0,0);
		wait1Msec(15000);
	}
	if (autonomousRoutine == 3) {
			displayLCDString(0, 0, "skills Boi");
		motor[Motor_FW1]=127;
		motor[Motor_FW2]=127;
		wait1Msec(2000);
		motor[Motor_FW1]=127;
		motor[Motor_FW2]=127;
		motor[Indexer]=-127;
		motor[Intake]=-127;
		wait1Msec(1000);
		rotate(true,90);
		wait1Msec(500);
		motor[Indexer]=0;
		wait1Msec(1000);
		move(90,0,0,0);
		wait1Msec(1500);
		move(0,0,0,0);
		wait1Msec(500);
		move(-95,0,0,0);
		wait1Msec(1550);
		move(0,0,0,0);
		wait1Msec(500);
		rotate(false,90);
		wait1Msec(500);
		move(60,0,0,0);
		wait1Msec(1000);
		move(0,0,0,0);
		motor[Indexer] = -127;
		wait1Msec(500);
		move(60,0,0,0);
		wait1Msec(500);
		move(-120,0,0,0);
		wait1Msec(1500);
		move(0,0,0,0);
		//reversing the Intake
		motor[Intake] = 127;
		//move to backwards starting square
		move(-120,0,0,0);
		wait1Msec(1500);
		move(0,0,0,0);
		wait1Msec(500);
		//going to knock over 2 caps
		rotate(false,90);
		move(90,0,0,0);
		wait1Msec(1500);
		move(0,0,0,0);
		wait1Msec(500);
		//turning around and aiming for the other 2 caps
		rotate(false,90);
		move(60,0,0,0);
		wait1Msec(1500);
		move(0,0,0,0);
		wait1Msec(500);
		rotate(false,90);
		wait1Msec(500);
		//hitting the other 2 caps
		move(90,0,0,0);
		wait1Msec(1500);
		move(0,0,0,0);
		wait1Msec(45000);}
//else	if (autonomousRoutine == 4) {
//	displayLCDString(0, 0, "back tile flags Boi");
//		wait1Msec(5000);
//		move(60,0,0,0);
//		wait1Msec(1500);
//		move(0,0,0,0);
//		motor[Motor_FW1]=127;
//		motor[Motor_FW2]=127;
//		motor[Indexer]=-127;
//		motor[Intake]=-127;
//		wait1Msec(1500);
//		motor[Motor_FW1]=60;
//		motor[Motor_FW2]=60;
//		motor[Indexer]=-127;
//		motor[Intake]=-127;
//		rotate(true,90);
//		wait1Msec(500);
//		move(0,0,0,0);
//		wait1Msec(15000);}
	else	if(autonomousRoutine ==4){
			displayLCDString(0, 0, "Test Boi");
				displayLCDString(1, 0, " d");
			SensorValue[LeftDriveE]=0;
			SensorValue[RightDriveE]=0;
	motor[DriveR1] = -127;
		motor[DriveR2] = -127;
		motor[DriveL1] = 127;
		motor[DriveL2] = 127;
		waitUntil(SensorValue[Gyro]=90);
		motor[DriveR1] = 0;
		motor[DriveR2] = 0;
		motor[DriveL1] = 0;
		motor[DriveL2] = 0;
	wait1Msec(100000);
	}
}
/*===========================================================
User Control
===========================================================*/
task usercontrol()
{

	datalogClear();
  startTask(lcdTask);
	 startTask( FwControlTask );
	while (true)
	{
		speedLeft = vexRT[Ch3];
		speedRight = vexRT[Ch2];
		if (abs(speedLeft) > threshold) {
			leftDriveMotors(speedLeft);
		}
		else{
			leftDriveMotors(0);
		}
		if (abs(speedRight) > threshold) {
			rightDriveMotors(speedRight);
		}
		else{
			rightDriveMotors(0);
		}
		//datalogAddValue(0,);
		if(vexRT[Btn5D] == 1)
		{
			motor[Intake] = -127;
		}
		else if(vexRT[Btn5U] == 1)
		{
			motor[Intake] = 127;
		}
		else
		{
			motor[Intake] = 0;
		}
		if(vexRT[Btn6D] == 1)
		{
			motor[Indexer] = -127;
		}
		else if(vexRT[Btn6U] == 1)
		{
			motor[Indexer] = 127;
		}
		else
		{
			motor[Indexer] = 0;
		}
		if(vexRT[Btn7L] == 1){
			motor[Descorer] = -127;
			}else if(vexRT[Btn7U] == 1){
			motor[Descorer] = 127;
			}else{
			motor[Descorer] = 0;
		}
        if( vexRT[ Btn8L ] == 1 )
            FwVelocitySet( 600, 0.55 );
        if( vexRT[ Btn8U ] == 1 )
            FwVelocitySet( 500, 0.38 );
        if( vexRT[ Btn8R ] == 1 )
            FwVelocitySet( 700, 0.75 );
        if( vexRT[ Btn8D ] == 1 )
            FwVelocitySet( 00, 0 );

        // Display useful things on the LCD
        //sprintf( str, "%4d %4d  %5.2f", target_velocity,  motor_velocity, nImmediateBatteryLevel/1000.0 );
        //displayLCDString(0, 0, str );
        //sprintf( str, "%4.2f %4.2f ", drive, drive_at_zero );
        //displayLCDString(1, 0, str );

        // Don't hog the cpu :)
        //wait1Msec(10);
        }
}

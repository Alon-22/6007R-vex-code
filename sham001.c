#pragma config(Sensor, dgtl11, FlyEnc,         sensorQuadEncoder)
#pragma config(Motor,  port2,           DriveL1,       tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           DriveL2,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           Descorer,      tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           Fly1,          tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           Fly2,          tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           Intake,        tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port8,           DriveR2,       tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           DriveR1,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port10,          Indexer,       tmotorVex393TurboSpeed_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX2)

#pragma competitionControl(Competition)


#include "Vex_Competition_Includes.c"


// Custom Rolling Robots Functions /////

float rpm=0;
float speed=0;
float error=0;
float FlyTarget=2400;
float ticks2;
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
const unsigned short trueSpeed[128] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 21 , 21, 21, 22, 22, 22, 23, 24, 24,
	25, 25, 25, 25, 26, 27, 27, 28, 28, 28,
	28, 29, 30, 30, 30, 31, 31, 32, 32, 32,
	33, 33, 34, 34, 35, 35, 35, 36, 36, 37,
	37, 37, 37, 38, 38, 39, 39, 39, 40, 40,
	41, 41, 42, 42, 43, 44, 44, 45, 45, 46,
	46, 47, 47, 48, 48, 49, 50, 50, 51, 52,
	52, 53, 54, 55, 56, 57, 57, 58, 59, 60,
	61, 62, 63, 64, 65, 66, 67, 67, 68, 70,
	71, 72, 72, 73, 74, 76, 77, 78, 79, 79,
	80, 81, 83, 84, 84, 86, 86, 87, 87, 88,
	88, 89, 89, 90, 90,127,127,127
};

//LINEAR DRIVE VARIABLES
int threshold = 10;
int speedLeft;
int speedRight;
string rightDrive= "rightDrive";
string leftDrive = "leftDrive";
//SETSPEED FUNCTION
void setSpeed(char* motorName,int speed){

	if(speed > 127) speed = 127;
	if(speed < -127) speed = -127;

	int absSpeed = trueSpeed[abs(speed)]*(speed/abs(speed+0.0001));

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
	/*	while(abs(SensorValue[RightDriveE]) <= rotate){} //&& (abs(SensorValue[LeftDriveE]) <= rotate ){}
	if(speed > 0 && dime == 1)
	{
	motor[DriveR1] = 30;
	motor[DriveR2] = 30;
	motor[DriveL1] = 30;
	motor[DriveL2] = 30;
	wait1Msec(200);
	motor[DriveR1] = 0;
	motor[DriveR2] = 0;
	motor[DriveL1] = 0;
	motor[DriveL2] = 0;
	}
	else if(speed < 0 && dime == 1)
	{
	motor[DriveR1] = 30;
	motor[DriveR2] = 30;
	motor[DriveL1] = 30;
	motor[DriveL2] = 30;
	wait1Msec(200);
	motor[DriveR1] = 0;
	motor[DriveR2] = 0;
	motor[DriveL1] = 0;
	motor[DriveL2] = 0;
	}
	else if(dime ==0)
	{
	motor[DriveR1] = 0;
	motor[DriveR2] = 0;
	motor[DriveL1] = 0;
	motor[DriveL2] = 0;
	}
	}
	*/
	//task flywheel(){
	//motor[Fly1]=127;
	//motor[Fly2]=127;
	//}
}
void rotate(bool right, float degrees){
	if(right == true){
		motor[DriveR1] = 127;
		motor[DriveR2] = 127;
		motor[DriveL1] = -127;
		motor[DriveL2] = -127;
		wait1Msec(5*degrees);
		motor[DriveR1] = 0;
		motor[DriveR2] = 0;
		motor[DriveL1] = 0;
		motor[DriveL2] = 0;

		}if(right == true){
		motor[DriveR1] = -127;
		motor[DriveR2] = -127;
		motor[DriveL1] = 127;
		motor[DriveL2] = 127;
		wait1Msec(5*degrees);
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

	//while(abs(SensorValue[RightDriveE]) <= rotation){}
	//motor[DriveR1] = -20;
	//motor[DriveR2] = -20;
	//motor[DriveL1] = 20;
	//motor[DriveL2] = 20;
	//wait1Msec(100);
	//motor[DriveR1] = 0;
	//motor[DriveR2] = 0;
	//motor[DriveL1] = 0;
	//motor[DriveL2] = 0;
}

//void rotateR(int speed, int rotation)
//{

//	motor[DriveR1] = -speed;
//	motor[DriveR2] = -speed;
//	motor[DriveL1] = speed;
//	motor[DriveL2] = speed;

//	while(abs(SensorValue[RightDriveE]) <= rotation){}
//	motor[DriveR1] = -20;
//	motor[DriveR2] = -20;
//	motor[DriveL1] = 20;
//	motor[DriveL2] = 20;
//	wait1Msec(100);
//	motor[DriveR1] = 0;
//	motor[DriveR2] = 0;
//	motor[DriveL1] = 0;
//	motor[DriveL2] = 0;
//}

//void FlyWheel(float target)
//{
//	float speed0=120;
//	float kp=0.2;

//	error=target-rpm;
//	speed=speed0+kp*error;
//	motor[Fly1]=speed;
//	motor[Fly2]=speed;







void pre_auton()
{

	bStopTasksBetweenModes = true;


}



task autonomous()
{
	//big auton
	bool red = true;
	//motor[descorer] = -127;
	wait1Msec(500);
	//motor[descorer]=0;
	motor[Fly1]=127;
	motor[Fly2]=127;
	wait1Msec(200);
	motor[Fly1]=127;
	motor[Fly2]=127;
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
	//wait1Msec(15000);

	/*
	//mini-auton
	//motor[descorer] = -127;
	//wait1Msec(500);
	//motor[descorer]=0;
	motor[Fly1]=127;
	motor[Fly2]=127;
	wait1Msec(2000);
	move(60,0,0,0);
	wait1Msec(500);
	move(0,0,0,0);
	wait1Msec(500);
	motor[Fly1]=127;
	motor[Fly2]=127;
	motor[Intake]=-127;
	motor[Indexer]=-127;
	wait1Msec(1000);
	motor[Indexer]=0;
	wait1Msec(500);
	\\comment here
	move(60,0,0,0);
	wait1Msec(1000);
	move(0,0,0,0);
	motor[Indexer]=127;
	wait1Msec(500);
	move(60,0,0,0);
	\\comment here
	move(60,0,0,0);
	wait1Msec(1500);
	move(0,0,0,0);
	wait1Msec(500);
	move(-120,0,0,0);
	wait1Msec(500);
	move(0,0,0,0);
	wait1Msec(15000);*/
	//========================================
	//For skills
	//reversing the Intake
	motor[intake] = 127;
	//move to backwards starting square
	move(-120,0,0,0);
	wait1Msec(1500);
	move(0,0,0,0);
	wait1Msec(500);
	//going to knock over 2 caps
	rotate(false,90);
	move(300,0,0,0);
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
	move(300,0,0,0);
	wait1Msec(1500);
	move(0,0,0,0);
	wait1Msec(45000);


	//===================================

	/*
	//back auton
	//motor[descorer] = -127;
	motor[Fly1]=127;
	motor[Fly2]=127;
	wait1Msec(4000);
	motor[Fly1]=127;
	motor[Fly2]=127;
	motor[Indexer]=127;
	motor[Intake]=127;
	wait1Msec(2000);
	motor[Fly1]=0;
	motor[Fly2]=0;
	wait1Msec(2000);
	move(200,0,0,0);
	wait1Msec(1500);
	move(0,0,0,0);
	wait1Msec(500);
	move(-200,0,0,0);
	wait1Msec(1500);
	move(0,0,0,0);
	wait1Msec(15000);*/






}



task usercontrol()
{
	// User control code here, inside the loop



	datalogClear();
	bool on = false;
	SensorValue(FlyEnc) = 0;
	while (true)
	{
		speedLeft = vexRT[Ch3];
		speedRight = vexRT[Ch2];

		if(speedLeft > threshold || speedLeft < threshold){
			leftDriveMotors(speedLeft);
		}
		else{
			leftDriveMotors(0);
		}
		if(speedRight > threshold || speedRight < threshold){
			rightDriveMotors(speedRight);
		}
		else{
			rightDriveMotors(0);
		}

		motor[Fly1] = 127;
		motor[Fly2] = 127;

		//if(vexRT[Btn7U]==1) {

		//	motor[Fly1]= 127
		//	motor[fly2]=127;
		//	while(vexRT[Btn7U])
		//	{
		//		wait1Msec(5);
		//	}
		//}

		//else if(vexRT[Btn7D]==1) {

		//	motor[Fly1]=0
		//motor[fly2]=0;

		//if (vexRT[Btn8R])
		//{
		//	on=!on;
		//	while(vexRT[Btn8R])
		//	{
		//		wait1Msec(5);
		//	}
		//}


		//if(on)
		//{

		//motor[Fly1] = 127;
		//motor[fly2] = 127;
		//}
		//else
		//{

		//	motor[Fly1]=0;
		//	motor[Fly2]=0;
		//}
		datalogAddValue(0,rpm);
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

	}
}

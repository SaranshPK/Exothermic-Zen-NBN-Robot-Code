#pragma config(Sensor, in1,    lightRPM,       sensorLineFollower)
#pragma config(Sensor, dgtl1,  rpmReady,       sensorDigitalOut)
#pragma config(Sensor, dgtl2,  rpmNotReady,    sensorDigitalOut)
#pragma config(Sensor, dgtl10, rpmSensor,      sensorQuadEncoder)
#pragma config(Sensor, dgtl12, shotsFired,     sensorDigitalIn)
#pragma config(Motor,  port1,           RightCon,      tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           RightDriveFront, tmotorVex393_MC29, openLoop, reversed, driveRight)
#pragma config(Motor,  port3,           RightDriveBack, tmotorVex393_MC29, openLoop, reversed, driveRight)
#pragma config(Motor,  port4,           fly1,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           fly2,          tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           fly3,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           fly4,          tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           LeftDriveBack, tmotorVex393_MC29, openLoop, driveLeft)
#pragma config(Motor,  port9,           LeftDriveFront, tmotorVex393_MC29, openLoop, driveLeft)
#pragma config(Motor,  port10,          LeftCon,       tmotorVex393_HBridge, openLoop)
#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!
int driverTarget = 2600;
#include <Movement.h>

/////////////////////////////////////////////////////////////////////////////////////////
//
//                          Pre-Autonomous Functions
//
// You may want to perform some actions before the competition starts. Do them in the
// following function.
//
/////////////////////////////////////////////////////////////////////////////////////////
void setUp()
{
	//resetSlewArray();
	SensorValue[rpmReady] = 1;
	SensorValue[rpmNotReady] = 1;
	SensorValue[rpmSensor] = 0;
	startTask(rpmIndicator);
}

void pre_auton()
{
	setUp();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Autonomous Task
//
// This task is used to control your robot during the autonomous phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task autonomous()
{
	//8.3 battery working
	setUp();
	startTask(FlywheelController);
	startTask(recoverFromShots);
	SetTarget(2700,63);
	wait1Msec(5000);
	Conveyor(127);
	wait1Msec(500);
	Conveyor(0);
	wait1Msec(2000);
	Conveyor(127);
	wait1Msec(500);
	Conveyor(0);
	wait1Msec(2000);
	Conveyor(127);
	wait1Msec(500);
	Conveyor(0);
	wait1Msec(2000);
	Conveyor(127);
	wait1Msec(500);
	Conveyor(0);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 User Control Task
//
// This task is used to control your robot during the user control phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task usercontrol()
{
	bool clicked;
	setUp();
	startTask(driverControl);
	startTask(conveyorControl);
	startTask(targetAdjustment);
	while(true)
	{
		while(vexRT[Btn8D] == 0)
		{
			wait1Msec(10);
		}
		while(vexRT[Btn8D] == 1)
		{
			wait1Msec(10);
		}

		clicked = !clicked;
		if(clicked)
		{
			startTask(FlywheelController);
			startTask(recoverFromShots);
			SetTarget(driverTarget, 65);
		}
		else
		{
			SetTarget(0, 0);
		}
	}
}

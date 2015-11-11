#pragma config(Sensor, dgtl1,  rpmSensor,       sensorQuadEncoder)
#pragma config(Motor,  port1,  RightCon,        tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,  RightDriveFront, tmotorVex393_MC29, 		openLoop, reversed, driveRight)
#pragma config(Motor,  port3,  RightDriveBack, 	tmotorVex393_MC29, 		openLoop, reversed, driveRight)
#pragma config(Motor,  port4,  RightFlyDown,  	tmotorVex393_MC29, 		openLoop)
#pragma config(Motor,  port5,  RightFlyUp,    	tmotorVex393_MC29, 		openLoop, reversed)
#pragma config(Motor,  port6,  LeftFlyUp,     	tmotorVex393_MC29, 		openLoop, reversed)
#pragma config(Motor,  port7,  LeftFlyDown,   	tmotorVex393_MC29, 		openLoop)
#pragma config(Motor,  port8,  LeftDriveBack, 	tmotorVex393_MC29, 		openLoop, driveLeft)
#pragma config(Motor,  port9,  LeftDriveFront, 	tmotorVex393_MC29, 		openLoop, driveLeft)
#pragma config(Motor,  port10, LeftCon,       	tmotorVex393_HBridge, openLoop)

#include <Setup.h>
#include <Movement.h>

task main()
{
	resetSlewArray();
	// motor
	Conveyor(127);
	flywheelPowerInput(127, true);
	//startTask(SlewManager);
	startTask(RPM);

	while(true){}
}

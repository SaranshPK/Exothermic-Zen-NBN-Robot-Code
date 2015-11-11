float currentRPM = 0;
int deadzone = 5;
int TargetValue = 0;

void flywheelPowerInput(int power)
{
	motor[LeftFlyUp] = motor[LeftFlyDown] = motor[RightFlyUp] = motor[RightFlyDown] = slew(0,power);
}

void flywheelTargetInput(int target){
}

void Conveyor(int power){
		motor[LeftCon] = motor[RightCon] = 127;
}
task RPM(){
	int y1 = 0;
	int y2 = 0;
	int waitTime = 200;
	int ratio = 5;
	while(true){
		y2 = SensorValue[rpmSensor];
		currentRPM = abs((((y2-y1)*ratio)/360.0)/(waitTime/60000.0));
		y1 = y2;
		wait1Msec(waitTime);
	}
}

task Flywheel()
{
	int error;
	int mp;
	int dropValue = 0;
	while(true)
	{
		error = TargetValue - currentRPM;
		mp = previous mp +error
		if(sgn(error) != sgn(currentRPM))
		{
		}
	}
}

float currentRPM = 0;
int deadzone = 5;
int TargetValue = 2000;
float mp = 0;

void FlywheelPower(int power)
{
	motor[fly1] = motor[fly2] = motor[fly3] = motor[fly4] = power;
}

void Conveyor(int power){
		motor[LeftCon] = motor[RightCon] = power;
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

task FlywheelController()
{ /*
	int error = 0;
	int mp = 0;
	int dropValue = 0;
	while(true)
	{
		error = TargetValue - currentRPM;
		mp = slew(127);
		if(sgn(error) != sgn(currentRPM))
		{
			resetSlewArray(0,dropValue);
		}

		mp = dropValue;
		while()
	}*/
	float error = 0;
	float prevmp = 0;
	float Kp = 0.0005;
	while(true)
	{
		error = TargetValue - currentRPM;
		mp = prevmp + (error*Kp);
		if(mp>127)
		{
			mp=127;
		}
		prevmp = mp;
		FlywheelPower(mp);
		wait1Msec(10);
	}
}

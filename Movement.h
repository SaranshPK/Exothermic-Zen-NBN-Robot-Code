float currentRPM = 0;
int deadzone = 5;
int TargetValue = 2650;
float mp = 0;

void FlywheelPower(int power)
{
	motor[fly1] = motor[fly2] = motor[fly3] = motor[fly4] = power;
}

void Conveyor(int power){
		motor[LeftCon] = motor[RightCon] = power;
}

task RPM(){
	SensorValue[rpmSensor] = 0;
	int y1 = 0;
	int y2 = 0;
	int waitTime = 100;
	int ratio = 5;
	while(true){
		if(abs(TargetValue-currentRPM)<100)
		{
			SensorValue[rpmReady] = 1;
		}
		else
		{
			SensorValue[rpmReady] = 0;
		}
		y2 = SensorValue[rpmSensor];
		currentRPM = abs((((y2-y1)*ratio)/360.0)/(waitTime/60000.0));
		y1 = y2;
		wait1Msec(waitTime);
	}
}

task FlywheelController()
{
	while(true)
	{
		float error = 0;
		float prevRPM = 0;
		float dropValue = 0;
		for(int tbhIteration = 0; tbhIteration<1; tbhIteration++)
		{
			while(true)
			{
				error = TargetValue - currentRPM;
				mp = slew(0,127);
				FlywheelPower(mp);
				if(error <= 0)
				{
					resetSlewArray(0,dropValue);
					FlywheelPower(dropValue);
					mp = dropValue;
					break;
				}
				prevRPM = currentRPM;
				wait1Msec(10);
			}
			while(currentRPM>=prevRPM)
			{
				deadzone = 10;
				prevRPM = currentRPM;
				wait1Msec(10);
			}
			while(currentRPM<=prevRPM)
			{
				prevRPM = currentRPM;
				mp = mp + 0.7;
				deadzone = mp;
				dropValue = mp;
				FlywheelPower(mp);
				wait1Msec(10);
			}
			error = TargetValue - currentRPM;
		}
		float prevmp = dropValue;
		float Kp = 0.005;
		resetSlewArray(0,0);
		while(true)
		{
			error = TargetValue - currentRPM;
			if(error>400)
			{
				break;
			}
			mp = prevmp + (error*Kp);
			if(mp>127)
			{
				mp=127;
			}
			if(mp<50)
			{
				mp = 50;
			}
			prevmp = mp;
			FlywheelPower(mp);
			wait1Msec(10);
		}
	}
}

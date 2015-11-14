#define FW_LOOP_SPEED              25

// Maximum power we want to send to the flywheel motors
#define FW_MAX_POWER              127

// encoder counts per revolution depending on motor
#define MOTOR_TPR_QUAD          360.0

long            counter;
float           ticks_per_rev;
long            e_current;
long            e_last;

float           v_current;
long            v_time;

long            target;
long            current;
long            last;
float           error;
float           last_error;
float           gain;
float           drive;
float           drive_at_zero;
long            first_cross;
float           drive_approx;

long            motor_drive;

void FlywheelPower(int power)
{
	motor[fly1] = motor[fly2] = motor[fly3] = motor[fly4] = power;
}

void SetTarget(int velocity, float predicted_drive )
{
	target = velocity;
	error = target - current;
	last_error = error;
	drive_approx  = predicted_drive;
	first_cross   = 1;
	drive_at_zero = 0;
}

void CalculateRPM()
{
	int delta_ms;
	int delta_enc;
	int ratio = 5;

	// Get current encoder value
	e_current = SensorValue[rpmSensor];

	delta_ms   = nSysTime - v_time;
	v_time = nSysTime;

	delta_enc = (e_current - e_last);
	e_last = e_current;

	v_current = abs((((delta_enc)*ratio)/360.0)/(delta_ms/60000.0));
}

void FwControlUpdateVelocityTbh()
{
	error = target - current;

	drive =  drive + (error * gain);

	if( drive > 127 )
		drive = 127;
	if( drive < 0 )
		drive = 0;

	if( sgn(error) != sgn(last_error) ) {
		if( first_cross ) {
			drive = drive_approx;
			first_cross = 0;
		}
		else
			drive = 0.5 * ( drive + drive_at_zero );
			drive_at_zero = drive;
		}
	last_error = error;
}

task FlywheelController()
{
	SensorValue[rpmSensor] = 0;
 	gain = 0.0005;

	while(true)
	{
		counter++;
		CalculateRPM();
		current = v_current;
		FwControlUpdateVelocityTbh() ;
		motor_drive  = drive;

		FlywheelPower(motor_drive);

		// Run at somewhere between 20 and 50mS
		wait1Msec(FW_LOOP_SPEED);
	}
}

task recoverFromShots()
{
	while(true)
	{
		if(SensorValue[shotsFired] == 0)
		{
			while(true)
			{
				if(SensorValue[shotsFired] == 1)
				{
					wait1Msec(250);
					SetTarget(target, drive_approx);
					break;
				}
				wait1Msec(10);
			}
		}
		wait1Msec(10);
	}
}

void Conveyor(int power)
{
	motor[LeftCon] = motor[RightCon] = power;
}

task driverControl()
{
	while(true)
	{
		if(abs(vexRT[Ch2]) > 5)
		{
			motor[RightDriveBack] = motor[RightDriveFront] = vexRT[Ch2];
		}
		else
		{
			motor[RightDriveBack] = motor[RightDriveFront] = 0;
		}
		if(abs(vexRT[Ch3]) > 5)
		{
			motor[LeftDriveBack] = motor[LeftDriveFront] = vexRT[Ch3];
		}
		else
		{
			motor[LeftDriveBack] = motor[LeftDriveFront] = 0;
		}
		wait1Msec(10);
	}
}

task rpmIndicator()
{
	while(true)
	{
		if(error<300)
		{
			SensorValue[rpmReady] = 0;
			SensorValue[rpmNotReady] = 1;
		}
		else
		{
			SensorValue[rpmReady] = 1;
			SensorValue[rpmNotReady] = 0;
		}
		wait1Msec(10);
	}
}

task conveyorControl()
{
	while(true)
	{
		if(vexRT[Btn6U] == 1||vexRT[Btn5U] == 1)
		{
			Conveyor(127);
			while(vexRT[Btn6U] == 1||vexRT[Btn5U] == 1)
			{
				wait1Msec(10);
			}
			Conveyor(0);
		}
		if(vexRT[Btn6D] == 1||vexRT[Btn5D] == 1)
		{
			Conveyor(-127);
			while(vexRT[Btn6D] == 1||vexRT[Btn5D] == 1)
			{
				wait1Msec(10);
			}
			Conveyor(0);
		}
	}
}

task targetAdjustment()
{
	while(true)
	{
		if(vexRT[Btn7D] == 1)
		{
			while(vexRT[Btn7D] == 1)
			{
				wait1Msec(10);
			}
			driverTarget -= 25;
			SetTarget(driverTarget, 65);
		}
		if(vexRT[Btn7U] == 1)
		{
			while(vexRT[Btn7D] == 1)
			{
				wait1Msec(10);
			}
			driverTarget += 25;
			SetTarget(driverTarget, 65);
		}
		wait1Msec(10);
	}
}

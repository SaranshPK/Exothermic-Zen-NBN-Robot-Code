#pragma config(Sensor, dgtl1,  rpmSensor,      sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  shotsFired,     sensorDigitalIn)
#pragma config(Motor,  port4,           Motor_FW1,     tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           Motor_FW2,     tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           Motor_FW3,     tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           Motor_FW4,     tmotorVex393TurboSpeed_MC29, openLoop, reversed)

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

bool meep = false;
// Structure to gather all the flywheel ralated data
typedef struct _fw_controller {

	long            counter;                ///< loop counter used for debug

	// encoder tick per revolution
	float           ticks_per_rev;          ///< encoder ticks per revolution

	// Encoder
	long            e_current;              ///< current encoder count
	long            e_last;                 ///< current encoder count

	// velocity measurement
	float           v_current;              ///< current velocity in rpm
	long            v_time;                 ///< Time of last velocity calculation

	// TBH control algorithm variables
	long            target;                 ///< target velocity
	long            current;                ///< current velocity
	long            last;                   ///< last velocity
	float           error;                  ///< error between actual and target velocities
	float           last_error;             ///< error last time update called
	float           gain;                   ///< gain
	float           drive;                  ///< final drive out of TBH (0.0 to 1.0)
	float           drive_at_zero;          ///< drive at last zero crossing
	long            first_cross;            ///< flag indicating first zero crossing
	float           drive_approx;           ///< estimated open loop drive

	// final motor drive
	long            motor_drive;            ///< final motor control value
} fw_controller;

// Make the controller global for easy debugging
static  fw_controller   flywheel;

/*-----------------------------------------------------------------------------*/
/** @brief      Set the flywheen motors                                        */
/** @param[in]  value motor control value                                      */
/*-----------------------------------------------------------------------------*/
void
FwMotorSet( int value )
{
	motor[ Motor_FW1 ] = value;
	motor[ Motor_FW2 ] = value;
	motor[ Motor_FW3 ] = value;
	motor[ Motor_FW4 ] = value;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Get the flywheen motor encoder count                           */
/*-----------------------------------------------------------------------------*/
long
FwMotorEncoderGet()
{
	return( SensorValue[rpmSensor] );
}

/*-----------------------------------------------------------------------------*/
/** @brief      Set the controller position                                    */
/** @param[in]  fw pointer to flywheel controller structure                    */
/** @param[in]  desired velocity                                               */
/** @param[in]  predicted_drive estimated open loop motor drive                */
/*-----------------------------------------------------------------------------*/
void
FwVelocitySet( fw_controller *fw, int velocity, float predicted_drive )
{
	// set target velocity (motor rpm)
	fw->target        = velocity;

	// Set error so zero crossing is correctly detected
	fw->error         = fw->target - fw->current;
	fw->last_error    = fw->error;

	// Set predicted open loop drive value
	fw->drive_approx  = predicted_drive;
	// Set flag to detect first zero crossing
	fw->first_cross   = 1;
	// clear tbh variable
	fw->drive_at_zero = 0;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Calculate the current flywheel motor velocity                  */
/** @param[in]  fw pointer to flywheel controller structure                    */
/*-----------------------------------------------------------------------------*/
void
FwCalculateSpeed( fw_controller *fw )
{
	int     delta_ms;
	int     delta_enc;

	// Get current encoder value
	fw->e_current = FwMotorEncoderGet();

	// This is just used so we don't need to know how often we are called
	// how many mS since we were last here
	delta_ms   = nSysTime - fw->v_time;
	fw->v_time = nSysTime;

	// Change in encoder count
	delta_enc = (fw->e_current - fw->e_last);

	// save last position
	fw->e_last = fw->e_current;

	// Calculate velocity in rpm
	fw->v_current = (1000.0 / delta_ms) * delta_enc * 60.0 / fw->ticks_per_rev;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Update the velocity tbh controller variables                   */
/** @param[in]  fw pointer to flywheel controller structure                    */
/*-----------------------------------------------------------------------------*/
void
FwControlUpdateVelocityTbh( fw_controller *fw )
{
	// calculate error in velocity
	// target is desired velocity
	// current is measured velocity
	fw->error = fw->target - fw->current;

	// Use Kp as gain
	fw->drive =  fw->drive + (fw->error * fw->gain);

	// Clip - we are only going forwards
	if( fw->drive > 1 )
		fw->drive = 1;
	if( fw->drive < 0 )
		fw->drive = 0;

	// Check for zero crossing
	if( sgn(fw->error) != sgn(fw->last_error) ) {
		// First zero crossing after a new set velocity command
		if( fw->first_cross ) {
			// Set drive to the open loop approximation
			fw->drive = fw->drive_approx;
			fw->first_cross = 0;
		}
		else
			fw->drive = 0.5 * ( fw->drive + fw->drive_at_zero );

		// Save this drive value in the "tbh" variable
		fw->drive_at_zero = fw->drive;
	}

	// Save last error
	fw->last_error = fw->error;
}

/*-----------------------------------------------------------------------------*/
/** @brief     Task to control the velocity of the flywheel                    */
/*-----------------------------------------------------------------------------*/
task
FwControlTask()
{
	SensorValue[rpmSensor] = 0;
	fw_controller *fw = &flywheel;

	// Set the gain
	fw->gain = 0.00025;

	// We are using Speed geared motors
	// Set the encoder ticks per revolution
	fw->ticks_per_rev = MOTOR_TPR_QUAD;

	while(1)
	{
		// debug counter
		fw->counter++;

		// Calculate velocity
		FwCalculateSpeed( fw );

		// Set current speed for the tbh calculation code
		fw->current = fw->v_current;

		// Do the velocity TBH calculations
		FwControlUpdateVelocityTbh( fw ) ;

		// Scale drive into the range the motors need
		fw->motor_drive  = (fw->drive * FW_MAX_POWER) + 0.5;

		// Final Limit of motor values - don't really need this
		if( fw->motor_drive >  127 ) fw->motor_drive =  127;
		if( fw->motor_drive < -127 ) fw->motor_drive = -127;

		// and finally set the motor control value
		FwMotorSet( fw->motor_drive );

		// Run at somewhere between 20 and 50mS
		wait1Msec( FW_LOOP_SPEED );
	}
}

// Main user task
task main()
{
	motor[port1] =
	127;
	motor[port10] =
	-127;
	// Start the flywheel control task
  startTask( FwControlTask );
	FwVelocitySet( &flywheel, 525, 0.55 );
	// Main user control loop
	while(true)
	{
		if(SensorValue[shotsFired] == 0)
		{
			while(true)
			{
				if(SensorValue[shotsFired] == 1)
				{
					meep = true;
					wait1Msec(250);
					FwVelocitySet( &flywheel, 525, 0.55 );
					break;
				}
				wait1Msec(10);
			}
		}
		//meep = false;
		// Don't hog the cpu :)
		wait1Msec(10);
	}
}

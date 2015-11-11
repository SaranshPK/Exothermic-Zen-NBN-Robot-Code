float currentRPM = 0;
int deadzone = 5;

void flywheelInput(int power){
	if(abs(power) > deadzone){
		FlyDL.req = FlyUL.req = FlyUR.req = FlyDR.req = power;
	}
}

void Conveyor(int power){
	if(abs(power) > deadzone){
		Con1.req = Con2.req = power;
	}
}

task RPM(){
	int y1 = 0;
	int y2 = 0;
	int waitTime = 100;
	while(true){
		y2 = SensorValue[rpmSensor];
		currentRPM = (((y2-y1)/360.0)/(waitTime/60000.0));
		y1 = y2;
		wait1Msec(waitTime);
	}
}

task Flywheel(){

}

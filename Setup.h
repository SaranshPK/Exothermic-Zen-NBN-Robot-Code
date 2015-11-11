// MOTOR STRUCT SETUP ---------------------------
typedef struct{
	int req;
	char* Sensor;
} drive, flywheel, intake; // Main Subsystems

drive DriveFR;
drive DriveFL;
drive DriveBR;
drive DriveBL;
flywheel FlyDL;
flywheel FlyUL;
flywheel FlyDR;
flywheel FlyUR;
intake Con1;
intake Con2;

// SLEWING CODE ---------------------------------

int Slew[10][51];

void refreshReq(){
	Slew[0][1] = Con1.req;
	Slew[1][1] = DriveFR.req;
	Slew[2][1] = DriveBR.req;
	Slew[3][1] = FlyUR.req;
	Slew[4][1] = FlyDR.req;
	Slew[5][1] = FlyUL.req;
	Slew[6][1] = FlyDL.req;
	Slew[7][1] = DriveFL.req;
	Slew[8][1] = DriveBL.req;
	Slew[9][1] = Con2.req;
}

void slewUpdate(){
	for(int x = 0; x<9; x++){
		for(int y = 49; y>0; y--){
			Slew[x][y+1] = Slew[x][y];
		}
	}
	refreshReq();
}

void slewAverage(){
	for(int x = 0; x<9; x++){
		int sum = 0;
		for(int y = 1; y<50; y++){
			sum = sum + Slew[x][y];
		}
		Slew[x][0] = (sum/50);
	}
}

void slewSend(){
	for(int i = 0; i < 10; i++){
		motor[i] = Slew[i][0];
	}
}

task SlewManager{
	float duration = 1.0; //seconds
	while(true){
		slewUpdate();
		slewAverage();
		slewSend();
		wait1Msec(duration/50);
	}
}

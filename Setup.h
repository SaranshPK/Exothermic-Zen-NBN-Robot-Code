const int MAX_SLEWS = 10;
const int SLEW_LENGTH = 50;
int slewArray[MAX_SLEWS][SLEW_LENGTH];

void resetSlewArray()
{
	for(int x = 0;x<MAX_SLEWS;x++)
	{
		for(int y = 0;y<SLEW_LENGTH;y++)
		{
			slewArray[x][y] = 0;
		}
	}
}

void resetSlewArray(int slewnumber, int value)
{
	for(int x = 0;x<SLEW_LENGTH;x++)
	{
		slewArray[slewnumber][x] = value;
	}
}

int slew(int slewnumber, int value)
{
	for(int x = SLEW_LENGTH-2;x>=0;x--)
	{
		slewArray[slewnumber][x+1] = slewArray[slewnumber][x];
	}
	slewArray[slewnumber][0] = value;
	int sum = 0;
	for(int x = 0;x<SLEW_LENGTH;x++)
	{
		sum += slewArray[slewnumber][x];
	}
	return sum/SLEW_LENGTH;
}

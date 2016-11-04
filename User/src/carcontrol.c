#include "carcontrol.h"
#include "car.h"
#include "systick.h"
#include "time.h"

extern Car car;

uint8_t speed_inc;
uint8_t speed_dec;

//GO --> Left--> Go -- Left --> Go --> Left --> Go -->Stop

/*
void Run_AlongRect(void)
{
    Car_Go();
	delay1s(5);

	Car_Left();
    delay1s(2);

	Parameter_Init(40,40,0,0);
	Car_Go();
	delay1s(5);

	Car_Left();
	delay1s(2);

	Parameter_Init(40,40,0,0);
	Car_Go();
	delay1s(5);

	Car_Left();
	delay1s(2);

	Parameter_Init(40,40,0,0);
	Car_Go();
	delay1s(5);

	Car_Stop();	
} */

void Run_GivenDistance(float distance)
{
    if(Car_GetRunDistance()>(distance/4) && speed_inc==0 )
	{
	   Car_SpeedInc(&car,0.3);
	   speed_inc = 1;
	}

	if(Car_GetRunDistance()>(distance/2) && speed_dec==0 )
	{
	   Car_SpeedDec(&car,0.4);
	   speed_dec = 1;
	}


	if(Car_GetRunDistance()>distance)
	{
	   Car_Stop();
	}
	
}  


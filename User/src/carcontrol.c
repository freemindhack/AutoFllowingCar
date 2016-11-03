#include "carcontrol.h"
#include "car.h"
#include "systick.h"

//GO --> Left--> Go -- Left --> Go --> Left --> Go -->Stop
void Run_AlongRect(void)
{
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

	Car_Left();
	delay1s(2);

	Parameter_Init(40,40,0,0);
	Car_Go();
	delay1s(5);

	Car_Stop();	
}


void Run_GivenDistance(float distance)
{
    float now_distance = 0;

    while(1)
	{
	   now_distance = Car_GetRunDistance();

	   if(now_distance - distance > -E && now_distance - distance < E )
	   {
	      break;
	   }

	   delay1s(1);
	}

	Car_Stop();
}


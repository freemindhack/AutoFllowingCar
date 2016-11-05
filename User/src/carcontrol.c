#include "carcontrol.h"
#include "car.h"
#include "systick.h"
#include "time.h"
#include "usart.h"

extern Car car;

uint8_t speed_inc;
uint8_t speed_dec;

float k = 0.2;

SysTime time;

void Run_Init(void)
{
   UserTime_Reset(&time);
}

void Run_FllowTarget(float distance)
{
    if(Car_GetRunDistance()<4)
	{
	    if(T_1ms(time,100))
		{ 
			Car_SpeedInc(&car,0.04);
			UserTime_Reset(&time);
		}	
	}
	else if( Car_GetRunDistance()>= 4 && Car_GetRunDistance()<5)
	{
	   	if(T_1ms(time,100))
		{ 
		    Car_SpeedDec(&car,k*0.05); 
			UserTime_Reset(&time);
		}
	}
	else
	{
	    Car_Stop();
	}


}

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
	   Car_SpeedDec(&car,0.2);
	   speed_dec = 1;
	}  

	
	if(Car_GetRunDistance()>distance)
	{
	   Car_Stop();
											   
	   //printf("circle : %0.2f\r\n",car.wheel[0].circles);
	} 
	
}  


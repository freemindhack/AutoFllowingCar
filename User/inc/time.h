#ifndef __TIME_H_
#define __TIME_H_


#include "stm32f10x.h"


typedef struct
{
    uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t msec;
	uint8_t usec;
}SysTime;


typedef enum
{
   FALSE = 0,
   TRUE = !FALSE,
}BOOL;


#endif
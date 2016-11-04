#ifndef __TIME_H_
#define __TIME_H_


#include "stm32f10x.h"


typedef struct
{
    uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint16_t msec;
	uint16_t usec;
}SysTime;


typedef enum
{
   FALSE = 0,
   TRUE = !FALSE,
}BOOL;

void Init_TIMER(void);
BOOL T_1s(SysTime now_time,uint16_t xs);
BOOL T_1ms(SysTime now_time,uint16_t xms);
void UserTime_Reset(SysTime* time);
uint32_t Sys_GetSeconds(void);


#endif

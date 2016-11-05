#ifndef __PID_H_
#define __PID_H_

#include "stm32f10x.h"


typedef struct
{
	float kp; //比例系数
	float ki; //积分系数
	float kd; //微分系数

    float set_speed; //设置值
	float real_speed;

	float err; //当前误差
	float err_next;	//Error[-1]
	float err_last;	//Error[-2]

}PID;


void PID_ParameterInit(PID* pid,float set_speed);

float PID_SpeedAjust(PID* pid,float real_speed);


#endif

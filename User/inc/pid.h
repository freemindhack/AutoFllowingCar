#ifndef __PID_H_
#define __PID_H_

#include "stm32f10x.h"


typedef struct
{
	float kp; //����ϵ��
	float ki; //����ϵ��
	float kd; //΢��ϵ��

    float set_speed; //����ֵ
	float real_speed;

	float err; //��ǰ���
	float err_next;	//Error[-1]
	float err_last;	//Error[-2]

}PID;


void PID_ParameterInit(PID* pid,float set_speed);

float PID_SpeedAjust(PID* pid,float real_speed);


#endif

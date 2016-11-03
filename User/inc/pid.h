#ifndef __PID_H_
#define __PID_H_

#include "stm32f10x.h"


typedef struct
{
	float kp; //比例系数
	float ki; //积分系数
	float kd; //微分系数

	float errNow; //当前误差
	float dCtrOut; //控制增量输出
	float ctrOut; //控制量输出

	float errOld1;	//Error[-1]
	float errOld2;	//Error[-2]

}PID;




#endif

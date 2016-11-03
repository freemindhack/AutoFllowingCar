#include "pid.h"

#define KP  15
#define KI  5
#define KD  5

void PID_ParameterInit(PID* pid)
{
   pid->kp      = KP;             //写入比例系数为15
   pid->ki      = KI;              //写入积分系数为5
   pid->kd      = KD;              //写入微分系数为5

   pid->errNow = 0;
   pid->dCtrOut = 0;
   pid->errOld1 = 0;
   pid->errOld2 = 0;

}

//增量式PID
void PID_SpeedAjust(PID* pid,u32 set_speed,u32 real_speed)
{
    float dErrP, dErrI, dErrD;

   	if( pid->kp < 0 ) pid->kp = -pid->kp;
	if( pid->ki < 0 ) pid->ki = -pid->ki;
	if( pid->kd < 0 ) pid->kd = -pid->kd;

	pid->errNow = set_speed - real_speed;
	dErrP = pid->errNow	- pid->errOld1;
	dErrI = pid->errNow;

	
	dErrD = pid->errNow - 2 * pid->errOld1 + pid->errOld2;

    pid->errOld2 = pid->errOld1; //二阶误差微分
    pid->errOld1 = pid->errNow;  //一阶误差微分

   /*增量式PID计算*/
   pid->dCtrOut = pid->kp * dErrP + pid->ki * dErrI + pid->kd * dErrD;
 
   if(pid->kp == 0 && pid->ki == 0 && pid->kd == 0)   pid->ctrOut = 0;

   else pid->ctrOut += pid->dCtrOut;
}


//方式2
void PIDCalc(PID* pid,u32 set_speed,u32 real_speed)
{
	float dError,Error;
	Error = set_speed - real_speed; // 偏差
	pid->errNow += Error; // 积分
	dError = pid->errOld1 - pid->errOld2; // 当前微分
	pid->errOld2 = pid->errOld1;
	pid->errOld1 = Error;

    if(pid->errNow>900)  pid->errNow=900;
    else if(pid->errNow<-900)	  pid->errNow=-900;

	pid->ctrOut +=(pid->kp * Error // 比例项
	+ pid->ki * pid->errNow // 积分项
	+ pid->kd * dError // 微分项
	);
}
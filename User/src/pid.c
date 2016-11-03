#include "pid.h"

#define KP  15
#define KI  5
#define KD  5

void PID_ParameterInit(PID* pid)
{
   pid->kp      = KP;             //д�����ϵ��Ϊ15
   pid->ki      = KI;              //д�����ϵ��Ϊ5
   pid->kd      = KD;              //д��΢��ϵ��Ϊ5

   pid->errNow = 0;
   pid->dCtrOut = 0;
   pid->errOld1 = 0;
   pid->errOld2 = 0;

}

//����ʽPID
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

    pid->errOld2 = pid->errOld1; //�������΢��
    pid->errOld1 = pid->errNow;  //һ�����΢��

   /*����ʽPID����*/
   pid->dCtrOut = pid->kp * dErrP + pid->ki * dErrI + pid->kd * dErrD;
 
   if(pid->kp == 0 && pid->ki == 0 && pid->kd == 0)   pid->ctrOut = 0;

   else pid->ctrOut += pid->dCtrOut;
}


//��ʽ2
void PIDCalc(PID* pid,u32 set_speed,u32 real_speed)
{
	float dError,Error;
	Error = set_speed - real_speed; // ƫ��
	pid->errNow += Error; // ����
	dError = pid->errOld1 - pid->errOld2; // ��ǰ΢��
	pid->errOld2 = pid->errOld1;
	pid->errOld1 = Error;

    if(pid->errNow>900)  pid->errNow=900;
    else if(pid->errNow<-900)	  pid->errNow=-900;

	pid->ctrOut +=(pid->kp * Error // ������
	+ pid->ki * pid->errNow // ������
	+ pid->kd * dError // ΢����
	);
}
#include "pid.h"

#define KP  0.2
#define KI  0.015
#define KD  0.2

void PID_ParameterInit(PID* pid,float set_speed)
{
   pid->kp      = KP;             //д�����ϵ��Ϊ15
   pid->ki      = KI;              //д�����ϵ��Ϊ5
   pid->kd      = KD;              //д��΢��ϵ��Ϊ5

   pid->err = 0;
   pid->err_last = 0;
   pid->err_next = 0;

   pid->set_speed = set_speed;
   pid->real_speed = 0;

}

//����ʽPID
float PID_SpeedAjust(PID* pid,u32 real_speed)
{
    float inc = 0;

    pid->err = pid->set_speed - real_speed;

	inc = pid->kp*(pid->err - pid->err_next) + pid->ki*pid->err + pid->kd*(pid->err - 2*pid->err_next + pid->err_last);

	pid->real_speed += inc;

	pid->err_last = pid->err_next;
	pid->err_next = pid->err;
	 
	return (pid->set_speed + pid->real_speed);
}


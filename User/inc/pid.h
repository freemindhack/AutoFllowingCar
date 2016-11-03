#ifndef __PID_H_
#define __PID_H_

#include "stm32f10x.h"


typedef struct
{
	float kp; //����ϵ��
	float ki; //����ϵ��
	float kd; //΢��ϵ��

	float errNow; //��ǰ���
	float dCtrOut; //�����������
	float ctrOut; //���������

	float errOld1;	//Error[-1]
	float errOld2;	//Error[-2]

}PID;




#endif

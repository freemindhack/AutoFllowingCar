#include "time.h"

static SysTime sys_time;//ϵͳʱ��


SysTime get_sys_time(void)
{
    return sys_time;
}

static void SysTime_Init(void)
{
   sys_time.hour = 0;
   sys_time.min = 0;
   sys_time.sec = 0;
   sys_time.msec = 0;
   sys_time.usec = 0;  
}

//��ȡ��ǰϵͳ����
uint32_t Sys_GetSeconds(void)
{
   	return (sys_time.hour*3600 + sys_time.min*60 + sys_time.sec);
}

void UserTime_Reset(SysTime* time)
{
   time->hour = sys_time.hour;
   time->min = sys_time.min;
   time->sec = sys_time.sec;
   time->msec = sys_time.msec;
}

BOOL T_1ms(SysTime now_time,uint16_t xms)
{
    return ((now_time.hour*3600*1000+now_time.min*60*1000 + now_time.sec*1000 + now_time.msec + xms) <= (sys_time.hour*3600*1000+sys_time.min*60*1000 + sys_time.sec*1000 + sys_time.msec) ? TRUE : FALSE);
}

BOOL T_1s(SysTime now_time,uint16_t xs)
{
   	return ((now_time.hour*3600+now_time.min*60 + now_time.sec + xs) <= (sys_time.hour*3600+sys_time.min*60 + sys_time.sec) ? TRUE : FALSE);
}


void Init_TIMER(void)  //���ﶨʱʱ����1ms	 
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = 10; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������N*10ΪNms
	TIM_TimeBaseStructure.TIM_Prescaler =(7200-1); //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM3
		TIM_IT_Update  |  //TIM �ж�Դ
		TIM_IT_Trigger,   //TIM �����ж�Դ 
		ENABLE  //ʹ��
		);
	/*�����ж����ȼ�*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	SysTime_Init();
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����							 
}

void TIM3_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  //���TIMx���жϱ�־λ

			sys_time.msec++;

			if(sys_time.msec >= 1000)
			{
			    sys_time.msec = 0;
				sys_time.sec ++;

				if(sys_time.sec >= 60)
				{
				   sys_time.sec = 0;
				   sys_time.min ++;
				   
				   if(sys_time.min >= 60)
				   {
				       sys_time.min = 0;
				       sys_time.hour ++;
					   if(sys_time.hour >= 24)
					   {
					      sys_time.hour = 0;
					   }
				   } 
				}
			}
		}


}




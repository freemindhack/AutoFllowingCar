#include "time.h"

static SysTime sys_time;//系统时钟


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

//获取当前系统秒数
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


void Init_TIMER(void)  //这里定时时间是1ms	 
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = 10; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到N*10为Nms
	TIM_TimeBaseStructure.TIM_Prescaler =(7200-1); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM3
		TIM_IT_Update  |  //TIM 中断源
		TIM_IT_Trigger,   //TIM 触发中断源 
		ENABLE  //使能
		);
	/*配置中断优先级*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	SysTime_Init();
	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设							 
}

void TIM3_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  //清除TIMx的中断标志位

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




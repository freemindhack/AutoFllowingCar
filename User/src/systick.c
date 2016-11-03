#include "SysTick.h"
static u8  fac_us=0;//us延时倍乘数
static u16 fac_ms=0;//ms延时倍乘数

/******************************************
*	功能：SysTick初始化
*	参数：SYSCLK系统时钟频率（单位：M）
******************************************/
void SysTick_Init(void)
{
	SysTick->CTRL&=0xfffffffb;//bit2清空,选择外部时钟  HCLK/8
	fac_us=9;		    
	fac_ms=(u16)fac_us*1000;
}

/********************************************
*	功能：毫秒级延时
*	参数：nms待延时的毫秒数
*
*       SysTick->LOAD为24位寄存器,所以,最大延时为:
*	nms<=0xffffff*8*1000/SYSCLK
*	对72M条件下,nms<=1864
********************************************/
void delay1ms(u16 nms)
{
		 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL=0x01 ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	 
	 	    
}

/********************************************
*	功能：微秒级延时
*	参数：nus待延时的微秒数
********************************************/   		    								   
void delay1us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	 
}

void delay1s(u16 ns)
{
   while(ns>0)
   {
     delay1ms(1000);
      ns--;
   }
}

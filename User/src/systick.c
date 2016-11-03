#include "SysTick.h"
static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������

/******************************************
*	���ܣ�SysTick��ʼ��
*	������SYSCLKϵͳʱ��Ƶ�ʣ���λ��M��
******************************************/
void SysTick_Init(void)
{
	SysTick->CTRL&=0xfffffffb;//bit2���,ѡ���ⲿʱ��  HCLK/8
	fac_us=9;		    
	fac_ms=(u16)fac_us*1000;
}

/********************************************
*	���ܣ����뼶��ʱ
*	������nms����ʱ�ĺ�����
*
*       SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
*	nms<=0xffffff*8*1000/SYSCLK
*	��72M������,nms<=1864
********************************************/
void delay1ms(u16 nms)
{
		 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL=0x01 ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	 
	 	    
}

/********************************************
*	���ܣ�΢�뼶��ʱ
*	������nus����ʱ��΢����
********************************************/   		    								   
void delay1us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	 
}

void delay1s(u16 ns)
{
   while(ns>0)
   {
     delay1ms(1000);
      ns--;
   }
}

#ifndef __USART_H
#define	__USART_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdbool.h>

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define RX_BUF_MAX_LEN     1024                                   

extern struct  STRUCT_USARTx_Fram                                  //串口数据帧的处理结构体
{
	char  Data_RX_BUF[ RX_BUF_MAX_LEN ];
	
  union {
    __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
  }; 
	
} strPc_Fram_Record;


extern unsigned char carready;
extern u8 readyflag;
void USART1_Config(void);
int fputc(int ch, FILE *f);
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...);
void USART2_Config(void) ;
void USART1_NVIC_Configuration(void);
void USART2_NVIC_Configuration(void);
void Sys_Soft_Reset(void);


#endif /* __USART1_H */

#include "BSP.h"


/*
 * 函数名：BSP_Init
 * 描述  ：时钟初始化、硬件初始化
 * 输入  ：无
 * 输出  ：无
 */
void BSP_Init(void)
{
    /*时钟初始化*/
	RCC_Configuration();	
	SysTick_Init();
	Car_Init();	

}

/*
 * 函数名：SysTick_init
 * 描述  ：配置SysTick定时器
 * 输入  ：无
 * 输出  ：无
 */

 void RCC_Configuration(void)
{
	/* 定义枚举类型变量 HSEStartUpStatus */
	ErrorStatus HSEStartUpStatus;

  	/* 复位系统时钟设置 */
  	RCC_DeInit();
  	/* 开启 HSE */
  	RCC_HSEConfig(RCC_HSE_ON);
  	/* 等待 HSE 起振并稳定 */
  	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	/* 判断 HSE 起是否振成功，是则进入if()内部 */
  	if(HSEStartUpStatus == SUCCESS)
  	{
    	/* 选择 HCLK（AHB）时钟源为SYSCLK 1分频 */
    	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    	/* 选择 PCLK2 时钟源为 HCLK（AHB）1分频 */
    	RCC_PCLK2Config(RCC_HCLK_Div1); 
    	/* 选择 PCLK1 时钟源为 HCLK（AHB）2分频 */
    	RCC_PCLK1Config(RCC_HCLK_Div2);
    	/* 设置 FLASH 延时周期数为2 */
    	FLASH_SetLatency(FLASH_Latency_2);
    	/* 使能 FLASH 预取缓存 */
    	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    	/* 选择锁相环（PLL）时钟源为HSE 1分频，倍频数为9，则PLL输出频率为 8MHz * 9 = 72MHz */
    	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    	/* 使能 PLL */ 
    	RCC_PLLCmd(ENABLE);
    	/* 等待 PLL 输出稳定 */
    	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    	/* 选择 SYSCLK 时钟源为 PLL */
    	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    	/* 等待 PLL 成为 SYSCLK 时钟源 */
    	while(RCC_GetSYSCLKSource() != 0x08);
  	}
	/* 打开 APB1 总线上的 PWR，BKP 时钟 */
}

#include "BSP.h"


/*
 * ��������BSP_Init
 * ����  ��ʱ�ӳ�ʼ����Ӳ����ʼ��
 * ����  ����
 * ���  ����
 */
void BSP_Init(void)
{
    /*ʱ�ӳ�ʼ��*/
	RCC_Configuration();	
	SysTick_Init();
	Car_Init();	

}

/*
 * ��������SysTick_init
 * ����  ������SysTick��ʱ��
 * ����  ����
 * ���  ����
 */

 void RCC_Configuration(void)
{
	/* ����ö�����ͱ��� HSEStartUpStatus */
	ErrorStatus HSEStartUpStatus;

  	/* ��λϵͳʱ������ */
  	RCC_DeInit();
  	/* ���� HSE */
  	RCC_HSEConfig(RCC_HSE_ON);
  	/* �ȴ� HSE �����ȶ� */
  	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	/* �ж� HSE ���Ƿ���ɹ����������if()�ڲ� */
  	if(HSEStartUpStatus == SUCCESS)
  	{
    	/* ѡ�� HCLK��AHB��ʱ��ԴΪSYSCLK 1��Ƶ */
    	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    	/* ѡ�� PCLK2 ʱ��ԴΪ HCLK��AHB��1��Ƶ */
    	RCC_PCLK2Config(RCC_HCLK_Div1); 
    	/* ѡ�� PCLK1 ʱ��ԴΪ HCLK��AHB��2��Ƶ */
    	RCC_PCLK1Config(RCC_HCLK_Div2);
    	/* ���� FLASH ��ʱ������Ϊ2 */
    	FLASH_SetLatency(FLASH_Latency_2);
    	/* ʹ�� FLASH Ԥȡ���� */
    	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    	/* ѡ�����໷��PLL��ʱ��ԴΪHSE 1��Ƶ����Ƶ��Ϊ9����PLL���Ƶ��Ϊ 8MHz * 9 = 72MHz */
    	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    	/* ʹ�� PLL */ 
    	RCC_PLLCmd(ENABLE);
    	/* �ȴ� PLL ����ȶ� */
    	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    	/* ѡ�� SYSCLK ʱ��ԴΪ PLL */
    	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    	/* �ȴ� PLL ��Ϊ SYSCLK ʱ��Դ */
    	while(RCC_GetSYSCLKSource() != 0x08);
  	}
	/* �� APB1 �����ϵ� PWR��BKP ʱ�� */
}

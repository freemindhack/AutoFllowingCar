#include "BSP.H"

int main()
{
	BSP_Init();//��ʼ��
	
	while(1)
	{
	   Car_RunCtl();
	   Run_GivenDistance(10);
	}
		
	return 0;

}


#include "BSP.H"

int main()
{
	BSP_Init();//��ʼ��

	Run_Init();

	while(1)
	{
	   Car_RunCtl();
	   //Run_GivenDistance(5);
	   Run_FllowTarget(5);
	}

}


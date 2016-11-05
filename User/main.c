#include "BSP.H"

int main()
{
	BSP_Init();//≥ı ºªØ

	Run_Init();

	while(1)
	{
	   Car_RunCtl();
	   //Run_GivenDistance(5);
	   Run_FllowTarget(5);
	}

}


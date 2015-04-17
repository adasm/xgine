#include "Headers.h"

/*********************************************************
**********************************************************
**********************************************************
**********************************************************


ERRORS:

1) World.cpp -> line  111

2) Dynamics  -> line  299


**********************************************************
**********************************************************
**********************************************************
**********************************************************/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_appName = "Farm Sim 1.0";
	core.init(hInstance);
	{
		while(gEngine.device->loop() )
		{
			core.proceedLoop();
			if(gEngine.input->isKeyDown(DIK_ESCAPE))break;
		}
	}
	core.close();
	return 0;
}
#include "Headers.h"

void Core::init(HINSTANCE hInstance)
{
	game = new Game;
	dynamics = new Dynamics;

	gEngine.init();
	gEngine.kernel->init();
	gEngine.loadPluginCfg("plugins.txt");
	gEngine.shaderMgr->init();
	//gEngine.kernel->fs->addSrc("data\\pak1.xpak");
//	gEngine.kernel->fs->addSrc("data\\pak2.xpak");
	//gEngine.kernel->con->initWnd(hInstance);
	RenderWindowInfo rendInfo;
	gEngine.device->createRendWnd(hInstance, &rendInfo);
	gEngine.device->init(&rendInfo);
	gEngine.input->init();
	gEngine.renderer->init();

	gEngine.desktop->m_Visible = true;


	REGCVARF32(g_grassEntityRenderDistance);
	REGCVARF32(g_grassSectorRenderDistance);
	REGCVARF32(g_grassSectorLowRenderDistance);
	REGCVARF32(g_grassTileSize);
	REGCVARF32(g_grassTextureCellWidth);
	REGCVARU32(g_grassSectorSize);
	REGCVARU32(g_grassEntitiesNum);
	REGCVARU32(g_grassSectorExistTime);

	
	REGCVARF32(g_treeRenderHiDetailsDistance);
	REGCVARF32(g_treeRenderMedDetailsDistance);
	REGCVARF32(g_treeRenderLowDetailsDistance);

	game->init();
}

void Core::close()
{
	game->close();
	gEngine.renderer->close();
	gEngine.input->close();
	gEngine.resMgr->close();
	gEngine.shaderMgr->close();
	gEngine.device->close();
	gEngine.kernel->close();
	gEngine.close();
}

void Core::proceedLoop()
{
	game->proceed();
}
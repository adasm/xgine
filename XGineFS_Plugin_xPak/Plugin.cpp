/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */

#include "Plugin.h"
#pragma comment(lib, "../Debug/XGine.lib")

Engine	*plugEngine;

BOOL APIENTRY _XGinePluginDllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//Plugin
extern "C" XGINE_PLUGIN_API int getVersion() 
{
	return 1;
}

extern "C" XGINE_PLUGIN_API void registerPlugin(Engine *K)
{
	plugEngine = K;
	plugEngine->kernel->fs->registerArchiveMgr(new FSArchMgrPACK());
	plugEngine->kernel->log->prn(LT_SUCCESS, "XGineFileSystem Plugin", "Added FSArchMgrPACK to file system.");
	plugEngine->kernel->fs->registerArchiveMgr(new FSArchMgrXPAK());
	plugEngine->kernel->log->prn(LT_SUCCESS, "XGineFileSystem Plugin", "Added FSArchMgrXPAK (with RT Update Support) to file system.");
	/*plugEngine->kernel->fs->registerArchiveMgr(new FSArchMgrCACHE());
	plugEngine->kernel->log->prn(LT_SUCCESS, "XGineFileSystem Plugin", "Added FSArchMgrCACHE to file system.");*/
}
#include "ResMgr.h"

std::queue<IRes*> resourcesToLoad;
u32 threadCreated = 0;

DWORD ResMgr::ResourceLoadingThread(void *ptr)
{
	if(gEngine.kernel && gEngine.kernel->log)
		gEngine.kernel->log->prnEx(LT_INFO, "ResMgr", "Loading-Thread started.");
	IRes* res;
	while(threadCreated)
	{
		if(!resourcesToLoad.empty())
		{
			res = resourcesToLoad.front();
			resourcesToLoad.pop();
		}
		else
		{
			res = 0;
			Sleep(0);
		}
		if(!res)continue;
		string fileName = res->getFolder()+res->getResFilename();
		Buffer *buffer = new Buffer();
		if(g_allowResourcesCache)
		{
			if(!gEngine.kernel->fs->loadCached(fileName, *buffer, res->getCache()))
			{
				gEngine.kernel->mem->freeBuff(*buffer);
				buffer = 0;
				continue;
			}
		}
		else if(!gEngine.kernel->fs->load(fileName, *buffer))
		{
			gEngine.kernel->mem->freeBuff(*buffer);
			buffer = 0;
			continue;
		}
		res->buffer = buffer;
		/*if(gEngine.kernel && gEngine.kernel->log)
			gEngine.kernel->log->prnEx(LT_INFO, "ResourceLoadingThread", "Loaded buffer '%s'.", fileName.c_str()); */
	}
	return 0;
}

ResMgr::~ResMgr()
{
}

void ResMgr::close()
{
	closeLoadingThread();
	for(tContainerIterator it = m_Container.begin(); it != m_Container.end(); ++it)
	{
		gEngine.kernel->log->prnEx(LT_DEBUG, "ResMgr", "Releasing unreleased resource: '%s'",it->second->strFilename.c_str());
		delete(it->second);
	}
	if(m_Container.size() == 0)
		gEngine.kernel->log->prn(LT_INFO, "ResMgr", "No unreleased resources");
	m_Container.clear();
}

void ResMgr::release(IRes *resource)
{
	if(resource != 0)
		release(resource->strFilename);
}

void ResMgr::release(string name)
{
	tContainerIterator it = m_Container.find(name);
	if(it != m_Container.end())
	{
		if(--(it->second->iRef) == 0)
		{
			gEngine.kernel->log->prnEx(LT_INFO, "ResMgr", "Releasing asset: %s", it->second->strFilename.c_str());
			delete (it->second);
			m_Container.erase(it);
		}
	}
}

bool ResMgr::reload()
{
	for(tContainerIterator it = m_Container.begin(); it != m_Container.end(); it++){
		if(!it->second->load()){
			gEngine.kernel->log->prnEx(LT_ERROR, "ResMgr", "Unable to reload asset: '%s'", it->second->strFilename.c_str()); 
			return false; 
		}
		else{
			gEngine.kernel->log->prnEx(LT_INFO, "ResMgr", "Reloaded asset: '%s'", it->second->strFilename.c_str()); 
		}
	}
	return true;
}
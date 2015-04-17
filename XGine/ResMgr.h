#pragma once
#include "XGine.h"

extern XGINE_API std::queue<IRes*> resourcesToLoad;
extern XGINE_API u32 threadCreated;

struct XGINE_API ResMgr
{
	typedef map<std::string, IRes*> tContainer;
	typedef tContainer::iterator	tContainerIterator;
	tContainer	m_Container;

	ResMgr()
	{
	}

	~ResMgr();

	static DWORD WINAPI ResourceLoadingThread(void *ptr);

	void close();
	void createLoadingThread()
	{
		if(!threadCreated)
		{
			threadCreated = 1;
			CreateThread(NULL,NULL,ResourceLoadingThread,0,NULL,NULL);
		}
	}
	void closeLoadingThread()
	{
		threadCreated = 0;
	}

	template<class T> T* load(string name, u32 background = 1)
	{
		tContainerIterator it = m_Container.find(name);
		if(it != m_Container.end()){
			it->second->iRef++;
			return dynamic_cast<T*>(it->second);
		}
		else
		{
			u32 tstart = timeGetTime();

			T* tr = new T();
			tr->iRef = 1;
			tr->strFilename = name;
			tr->buffer = 0;
			
			if(g_allowResourcesCache)
			{
				gEngine.kernel->fs->addSrc(tr->getCache(), "XPAK");
			}

			u32 genMem = (name.substr(0, 3) == "%gm");

			if(background && tr->canBeLoadBackground() && tr->mustLoadBuffer() && !genMem)
			{
				if(gEngine.kernel->fs->exists(tr->getFolder()+name))
				{
					resourcesToLoad.push((IRes*)(tr));
					m_Container.insert( make_pair(name, tr) );
					return tr;
				}
				else
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "ResMgr", "Unable to load asset '%s'. Missing file.", name.c_str());
					delete(tr);
					return 0;
				}
			}
			else
			{
				if(tr->mustLoadBuffer() && !genMem)
				{
					string g_path = tr->getFolder();
					tr->buffer = new Buffer();
					if(g_allowResourcesCache)
					{
						if(!gEngine.kernel->fs->loadCached(tr->getFolder()+tr->getResFilename(), *tr->buffer, tr->getCache()))return 0;
					}
					else
					{
						if(!gEngine.kernel->fs->load(tr->getFolder()+tr->getResFilename(), *tr->buffer))return 0;
					}
				}

				if(tr->load()){				
					gEngine.kernel->log->prnEx(LT_SUCCESS, "ResMgr", "Loaded asset '%s' in %ums", name.c_str(), (u32)timeGetTime()-tstart);
					m_Container.insert( make_pair(name, tr) );
					return tr;
				}
				else{
					gEngine.kernel->log->prnEx(LT_ERROR, "ResMgr", "Unable to load asset '%s'", name.c_str());
					delete(tr);
					return 0;
				}
			}
		}
	}

	template<class T> T* add(string name)
	{
		tContainerIterator it = m_Container.find(name);
		if(it != m_Container.end()){
			it->second->iRef++;
			return dynamic_cast<T*>(it->second);
		}
		else
		{
			T* tr = new T();
			tr->iRef = 0;
			tr->strFilename = name;	
			gEngine.kernel->log->prnEx("Added asset '%s'", name.c_str());
			m_Container.insert( make_pair(name, tr) );
			return tr;
		}
	}

	template<class T> T* clone(IRes *res)
	{
		res->iRef++;
		return res;
	}
	
	void	 release(string name);
	void	 release(IRes *resource);
	bool	 reload();
};
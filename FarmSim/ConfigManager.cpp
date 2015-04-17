#include "ConfigManager.h"

template<class T> T* ConfigManager::load(string fname)
{
	m_configsIt = m_configs.find(name);
	if(m_configsIt != m_configs.end())
		return dynamic_cast<T*>(m_configsIt->second);
	else
	{
		T* config = new T();
		config->load(fname);
		gEngine.kernel->log->prnEx("Added config to cfgManager '%s'", fname.c_str());
		m_configs.insert(make_pair(fname, config));
		return config;
	}
}
/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */

#include "Engine.h"

u32 Engine::init()
{
	kernel		= new Kernel();
	device		= new Device();
	input		= new Input();
	renderer	= new Renderer();
	resMgr		= new ResMgr();
	shaderMgr	= new ShaderMgr();
	desktop		= new Desktop();
	audio		= new alAudio();
	return true;
}

void Engine::close()
{
	delete(audio);		audio = 0;
	delete(shaderMgr);	shaderMgr = 0;
	delete(resMgr);		resMgr = 0;
	delete(renderer);	renderer = 0;
	delete(input);		input = 0;
	delete(device);		device = 0;
	delete(kernel);		kernel = 0;
	delete(desktop);	desktop = 0;
	m_loadedPlugins.clear();
}

void Engine::loadPluginCfg(const std::string &fileName)
{
	Buffer buff;
	if(!gEngine.kernel->fs->load(fileName, buff))
		return;
	
	Tokenizer tokenizer(string(buff.data, buff.size));
	gEngine.kernel->mem->freeBuff(buff);

	tokenizer.clear(";");

	string token;
	while(tokenizer.nextToken(&token))
	{
		if(gEngine.kernel->fs->exists(token.c_str()))
			addPlugin(token);
	}
}

void Engine::addPlugin(const std::string &fileName)
{
	if(m_loadedPlugins.find(fileName) == m_loadedPlugins.end())
	{
        m_loadedPlugins.insert(PluginMap::value_type(fileName,Plugin(fileName))).first->second.registerPlugin(*this);
		gEngine.kernel->log->prnEx(LT_SUCCESS, "Engine", "Loaded plugin '%s'.", fileName.c_str());
	}
}
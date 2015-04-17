/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */

#pragma once
#include "XGine.h"

struct XGINE_API Engine
{
public:
	u32		init();
	void	close();
	void	loadPluginCfg(const std::string &fileName);
	void	addPlugin(const std::string &fileName);

	Kernel*		kernel;
	Device*		device;
	IInput*		input;
	Renderer*	renderer;
	ResMgr*		resMgr;
	ShaderMgr*	shaderMgr;
	Desktop*	desktop;

	alAudio*	audio;

private:
	typedef std::map<std::string, Plugin>	PluginMap;
    PluginMap								m_loadedPlugins;
};
/*  By Adam Micha³owski / J 0 |< e R / (c) 2009  */

#pragma once
#include "../XGine/XGine.h"

///////////////////////////////////////////////////
#ifdef XGINEFS_PLUGIN_XPAK_EXPORTS
	#define XGINE_PLUGIN_API __declspec(dllexport)
#else
	#define XGINE_PLUGIN_API __declspec(dllimport)
#endif


extern XGINE_PLUGIN_API Engine	*plugEngine;
#include "FileSystem.h"
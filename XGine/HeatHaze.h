/* By Adam Micha�owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API HeatHaze
{
	HeatHaze();
	~HeatHaze();

	u32		init();
	void	close();

	void	process();

protected:
	Shader	*shader;
};
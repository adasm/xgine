/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API DepthOfField
{
	DepthOfField();
	~DepthOfField();

	u32		init();
	void	close();

	void	process();

protected:
	Shader					*shader;
	RenderTexture			*prevDepth, *currentDepth;
};
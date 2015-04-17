/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct IMaterial
{
	virtual			~IMaterial()			{ }
	
	virtual	u32		apply(Mat* world, Light* light)		= 0;
	virtual	u32		apply(Mat* world, Light* light, RenderTexture* shadowMap)		= 0;

	virtual	Shader* getShader()				= 0;
	virtual	u32		numPasses()				= 0;
	virtual void	beginPass(u32 i)		= 0;
	virtual void	endPass()				= 0;
};
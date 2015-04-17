/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API VolumetricLightScattering
{
	VolumetricLightScattering() : initialized(0), shader(0), texture(0) { }
	~VolumetricLightScattering() { close(); }

	u32		init();
	void	close();

	void	prepare();
	void	draw();

	void setSunPos(Vec3 pos) { sunPos = pos; }

	RenderTexture*	getTexture() { return texture; }

protected:
	u32					initialized;
	Shader				*shader;
	RenderTexture		*texture, *tempEffect, *blured, *finalEffect;
	GuassianBlur		*guassianBlur;
	Vec3				sunPos;
};
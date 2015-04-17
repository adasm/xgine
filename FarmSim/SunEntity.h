#pragma once
#include "Headers.h"

struct SunEntity : IEntity
{
	SunEntity(LensFlare* lens);
	~SunEntity();

	void	update(ICamera* cam);
	void	renderLens();
	u32		renderDeferred();
	u32		renderForward(Light* light = 0);
	u32		renderRaw();
	void	setSunLight(Light *light) { g_sunLight = light; }

	inline u32			rendEnabled()	{ return 1; }
	inline u32			selfRendering() { return 1; }
	inline Mesh*		getMesh()		{ return 0; }
	inline Material*	getMaterial()	{ return 0; }
	inline Shader*		getShader()		{ return shader; }
	inline Texture*		getTexture()	{ return NULL; }
	inline u32			hasShadow()		{ return 0; }
		   REND_TYPE	getRendType();

	inline bool			isToRender() { return m_render; }

protected:
	bool			m_render;
	LensFlare*		m_lensFlare;
	Light*			g_sunLight;
};
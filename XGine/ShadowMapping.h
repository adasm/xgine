/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API ShadowMapping
{
	ShadowMapping();

	u32		init();
	void	close();
	
	u32		renderDepth(RenderTexture* rt, RenderList* rl, Light* light);

	u32		prepareShadowMap(RenderList* in, Light* light, RenderTexture* screenMap);

protected:
	u32						m_initialized;
	Shader*					m_castingShader;
	Texture*				m_noiseMap;
};
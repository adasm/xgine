/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API ShadowVolume
{
	ShadowVolume();

	u32		init();
	void	close();
	
	u32		prepareStencil(RenderList* in, Light* light);

protected:
	u32						m_initialized;
	Shader*					m_shader;
};
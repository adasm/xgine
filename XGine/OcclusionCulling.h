/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API OcclusionCulling
{
	OcclusionCulling();

	u32		init();
	void	close();

	u32		check(RenderList *in);
	
	inline LPDIRECT3DQUERY9			getOcclusionQuery()		{ return d3dQuery; }
	inline LPD3DXRENDERTOSURFACE	getOcclusionRender()	{ return occlusionRender; }
	inline LPDIRECT3DSURFACE9		getOcclusionSurface()	{ return occlusionSurface; }
	inline LPDIRECT3DTEXTURE9		getOcclusionTexture()	{ return occlusionTexture; }
	inline Shader*					getOcclusionShader()	{ return m_occlusionShader; }

protected:
	u32						m_initialized;
	Shader					*m_occlusionShader;
	LPDIRECT3DQUERY9		d3dQuery;
	LPD3DXRENDERTOSURFACE	occlusionRender;
	LPDIRECT3DSURFACE9		occlusionSurface;
	LPDIRECT3DTEXTURE9		occlusionTexture;
};
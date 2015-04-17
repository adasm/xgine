#pragma once
#include "XGine.h"

struct XGINE_API DepthMap
{
	DepthMap();
	~DepthMap();

	struct CreationParams
	{
		u32			size; 
		D3DFORMAT	colorFormat;
		D3DFORMAT	depthFormat;
	};

	u32		init(const CreationParams &cp);
	void	close();

	void	enableRendering();
	void	disableRendering();

	inline const CreationParams		&getParams()		{ return m_Params; }
	inline Mat*						getTexAdjMat()		{ return &m_texAdjMat; }
	inline LPDIRECT3DTEXTURE9		getColorTexture()	{ return m_pTexture; }

protected:
	CreationParams		m_Params;
	LPDIRECT3DTEXTURE9	m_pTexture;
	LPDIRECT3DSURFACE9	m_pSurface;
	LPDIRECT3DSURFACE9	m_pDSSurface;
	D3DVIEWPORT9		m_Viewport;
	LPDIRECT3DSURFACE9	m_pOldDSSurface;
	LPDIRECT3DSURFACE9	m_pOldRenderTarget;
	D3DVIEWPORT9		m_OldViewport;
	Mat					m_texAdjMat;
};

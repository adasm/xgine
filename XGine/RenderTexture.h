#pragma once
#include "XGine.h"

struct XGINE_API RenderTexture
{
	RenderTexture();
	~RenderTexture();

	struct CreationParams
	{
		u32			width;
		u32			height;  
		D3DFORMAT	colorFormat;
		D3DFORMAT	depthFormat;
	};

	u32		init(const CreationParams &cp);
	void	close();

	void	enableRendering(u32 depthStencilEnabled = 1);
	void	disableRendering();

	inline const CreationParams		&getParams()		{ return m_Params; }
	inline D3DVIEWPORT9				&getViewport()		{ return m_Viewport; }
	inline Mat*						getTexAdjMat()		{ return &m_texAdjMat; }
	inline LPDIRECT3DTEXTURE9		getColorTexture()	{ return m_pTexture; }
	inline LPDIRECT3DSURFACE9		getColorSurface()	{ return m_pSurface; }

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

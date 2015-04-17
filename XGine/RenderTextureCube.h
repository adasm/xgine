#pragma once
#include "XGine.h"

struct XGINE_API RenderTextureCube
{
	RenderTextureCube();
	~RenderTextureCube();

	struct CreationParams
	{
		u32			size;
		D3DFORMAT	colorFormat;
		D3DFORMAT	depthFormat;
	};

	u32		init(const CreationParams &cp);
	void	close();

	void	enableRendering();
	void	setupCubeFace(u32 i);
	void	disableRendering();

	inline const CreationParams		&getParams()		{ return m_Params; }
	inline Mat*						getTexAdjMat()		{ return &m_texAdjMat; }
	inline LPDIRECT3DCUBETEXTURE9	getCubeTexture()	{ return m_pCubeTexture; }

	static void calculateCubeMapViewMat(u32 face, const D3DXVECTOR3* centerPos, D3DXMATRIX* outMatrix);
protected:
	CreationParams			m_Params;
	LPDIRECT3DCUBETEXTURE9 	m_pCubeTexture;
	LPDIRECT3DSURFACE9		m_pDSSurface;
	D3DVIEWPORT9			m_Viewport;
	LPDIRECT3DSURFACE9		m_pOldDSSurface;
	LPDIRECT3DSURFACE9		m_pOldRenderTarget;
	D3DVIEWPORT9			m_OldViewport;
	Mat						m_texAdjMat;
	Mat						m_cubeFaceProjection;
};

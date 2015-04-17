#include "RenderTexture.h"

DepthMap::DepthMap()
{
	m_pTexture			=NULL;
	m_pSurface			=NULL;
	m_pDSSurface		=NULL;
	m_pOldDSSurface		=NULL;
	m_pOldRenderTarget	=NULL;
}

DepthMap::~DepthMap()
{
	close();
}

u32 DepthMap::init(const CreationParams &cp)
{
	if(FAILED(gEngine.device->getDev()->CreateTexture(cp.size,cp.size,1,D3DUSAGE_RENDERTARGET,cp.colorFormat,D3DPOOL_DEFAULT,&m_pTexture,NULL)))
	{
		gEngine.kernel->log->prn("Creating DepthMap failed. Could not create texture.");
		return 0;
	}
	if(FAILED(m_pTexture->GetSurfaceLevel(0,&m_pSurface)))
	{
		gEngine.kernel->log->prn("Creating DepthMap failed. GetSurfaceLevel failed.");
		return 0;
	}
	if(FAILED(gEngine.device->getDev()->CreateDepthStencilSurface(cp.size,cp.size,cp.depthFormat,D3DMULTISAMPLE_NONE,0,TRUE,&m_pDSSurface,NULL)))
	{
		gEngine.kernel->log->prn("Creating DepthMap failed. CreateDepthStencilSurface failed.");
		return 0;
	}
	m_Viewport.X = 0;
	m_Viewport.Y = 0;
	m_Viewport.Width  = cp.size;
	m_Viewport.Height = cp.size;
	m_Viewport.MinZ = 0.0f;
	m_Viewport.MaxZ = 1.0f;
	float ZBias		= -0.001f;
	float fTexOffs	= 0.5f + (0.5f / (float)cp.size);
	m_texAdjMat		= Mat(0.5f,		0.0f,	0.0f,	0.0f,
						  0.0f,    -0.5f,	0.0f,	0.0f,
						  0.0f,		0.0f,	1.0f,	0.0f,
						  fTexOffs, fTexOffs,  ZBias, 1.0f );
	m_Params=cp;
	return 1;
}

void DepthMap::close()
{
	DXRELEASE(m_pDSSurface);
	DXRELEASE(m_pSurface);
	DXRELEASE(m_pTexture);
	DXRELEASE(m_pOldDSSurface);
	DXRELEASE(m_pOldRenderTarget);
}

void DepthMap::enableRendering()
{
	// Store original values
	//gEngine.device->getDev()->GetViewport(&m_OldViewport);
	gEngine.device->getDev()->GetRenderTarget(0,&m_pOldRenderTarget);
	gEngine.device->getDev()->GetDepthStencilSurface(&m_pOldDSSurface);
	// Set new values
	//gEngine.device->getDev()->SetViewport(&m_Viewport);
	gEngine.device->getDev()->SetRenderTarget(0,m_pSurface);
	gEngine.device->getDev()->SetDepthStencilSurface(m_pDSSurface);
}

void DepthMap::disableRendering()
{
	// Restore old target and depth stencil
	gEngine.device->getDev()->SetRenderTarget(0,m_pOldRenderTarget);
	gEngine.device->getDev()->SetDepthStencilSurface(m_pOldDSSurface);
	DXRELEASE(m_pOldRenderTarget);
	DXRELEASE(m_pOldDSSurface);
	// Restore old viewport
	//gEngine.device->getDev()->SetViewport(&m_OldViewport);
}
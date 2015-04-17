#include "RenderTexture.h"

RenderTexture::RenderTexture()
{
	m_pTexture			=NULL;
	m_pSurface			=NULL;
	m_pDSSurface		=NULL;
	m_pOldDSSurface		=NULL;
	m_pOldRenderTarget	=NULL;
}

RenderTexture::~RenderTexture()
{
	close();
}

u32 RenderTexture::init(const CreationParams &cp)
{
	if(FAILED(gEngine.device->getDev()->CreateTexture(cp.width,cp.height,1,D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP,cp.colorFormat,D3DPOOL_DEFAULT,&m_pTexture,NULL))) 
	{
		gEngine.kernel->log->prn(LT_FATAL, "RenderTexture", "Creating render texture failed. Could not create texture.");
		return 0;
	}
	if(FAILED(m_pTexture->GetSurfaceLevel(0,&m_pSurface)))
	{
		gEngine.kernel->log->prn(LT_FATAL, "RenderTexture", "Creating render texture failed. GetSurfaceLevel failed.");
		return 0;
	}
	if(FAILED(gEngine.device->getDev()->CreateDepthStencilSurface(cp.width,cp.height,cp.depthFormat,(D3DMULTISAMPLE_TYPE)0,0,TRUE,&m_pDSSurface,NULL)))
	{
		gEngine.kernel->log->prn(LT_FATAL, "RenderTexture", "Creating render texture failed. CreateDepthStencilSurface failed.");
		return 0;
	}
	m_Viewport.X = 0;
	m_Viewport.Y = 0;
	m_Viewport.Width  = cp.width;
	m_Viewport.Height = cp.height;
	m_Viewport.MinZ = 0.0f;
	m_Viewport.MaxZ = 1.0f;
	float ZBias		= 0;
	float fTexOffsX	= 0.5f + (0.5f / (float)cp.width);
	float fTexOffsY	= 0.5f + (0.5f / (float)cp.height);
	m_texAdjMat		= Mat(0.5f,		0.0f,	0.0f,	0.0f,
						  0.0f,    -0.5f,	0.0f,	0.0f,
						  0.0f,		0.0f,	1.0f,	0.0f,
						  fTexOffsX, fTexOffsY,  ZBias, 1.0f );
	m_Params=cp;

	gEngine.device->getDev()->GetRenderTarget(0,&m_pOldRenderTarget);
	gEngine.device->getDev()->GetDepthStencilSurface(&m_pOldDSSurface);
	gEngine.device->getDev()->GetViewport(&m_OldViewport);
	return 1;
}

void RenderTexture::close()
{
	DXRELEASE(m_pDSSurface);
	DXRELEASE(m_pSurface);
	DXRELEASE(m_pTexture);
	DXRELEASE(m_pOldRenderTarget);
	DXRELEASE(m_pOldDSSurface);
}

void RenderTexture::enableRendering(u32 depthStencilEnabled)
{
	gEngine.device->getDev()->SetViewport(&m_Viewport);
	gEngine.device->getDev()->SetRenderTarget(0,m_pSurface);
	if(depthStencilEnabled)
		gEngine.device->getDev()->SetDepthStencilSurface(m_pDSSurface);
}

void RenderTexture::disableRendering()
{
	gEngine.device->getDev()->SetRenderTarget(0,m_pOldRenderTarget);
	gEngine.device->getDev()->SetDepthStencilSurface(m_pOldDSSurface);
	gEngine.device->getDev()->SetViewport(&m_OldViewport);
}
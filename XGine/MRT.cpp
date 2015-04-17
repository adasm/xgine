#include "MRT.h"

MRT::MRT()
	: m_initialized(0), m_numMRTs(0), m_rtDepth(0), m_pOldRenderTarget(0), m_pOldDSSurface(0)
{
	for(u32 i = 0; i < 4; ++i)
	{
		m_rt[i] = 0;
		m_rtSurface[i] = 0;
	}
}

MRT::~MRT()
{
	close();
}

u32 MRT::init(u32 numMRTs, D3DFORMAT *mrtFormat)
{
	if((m_initialized && (numMRTs != m_numMRTs || mrtFormat != m_mrtFormat)) || !m_initialized)
	{
		close();
		m_numMRTs	= numMRTs;
		 
		HRESULT hRes;
		for(u32 i = 0; i < m_numMRTs; ++i)
		{
			m_mrtFormat[i] = mrtFormat[i];
			hRes = gEngine.device->getDev()->CreateTexture(gEngine.device->getWidth(),gEngine.device->getHeight(),1,D3DUSAGE_RENDERTARGET,m_mrtFormat[i],D3DPOOL_DEFAULT,&m_rt[i],0);
			if(FAILED(hRes))
			{
				gEngine.kernel->log->prn(LT_ERROR, "MRT", "MRT initialization failed. Could not create render texture.");
				close();
				return 0;
			}
			m_rt[i]->GetSurfaceLevel( 0, &m_rtSurface[i] );
		}
		hRes = gEngine.device->getDev()->CreateDepthStencilSurface(gEngine.device->getWidth(),gEngine.device->getHeight(),D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,FALSE,&m_rtDepth,0);
		if(FAILED(hRes))
		{
			gEngine.kernel->log->prn(LT_ERROR, "MRT", "MRT initialization failed. Could not create depth texture.");
			close();
			return 0;
		}
		
		gEngine.device->getDev()->GetRenderTarget(0,&m_pOldRenderTarget);
		gEngine.device->getDev()->GetDepthStencilSurface(&m_pOldDSSurface);

		if(!m_pOldRenderTarget || !m_pOldDSSurface)
		{
			gEngine.kernel->log->prn(LT_ERROR, "MRT", "MRT initialization failed. Could not get screen render target and stencil depth surface.");
			close();
			return 0;
		}

		m_initialized = 1;
		return 1;
	}
	return 1;
}

void MRT::close()
{
	DXRELEASE(m_rtDepth);
	for(u32 i = 0; i < m_numMRTs; ++i)
	{
		DXRELEASE(m_rtSurface[i]);
		DXRELEASE(m_rt[i]);
	}

	DXRELEASE(m_pOldRenderTarget);
	DXRELEASE(m_pOldDSSurface);

	m_initialized = 0;
}

void MRT::bindDepthStencil()
{
	gEngine.device->getDev()->SetDepthStencilSurface(m_rtDepth);
}

void MRT::begin(u32 depthStencilEnabled)
{
	if(depthStencilEnabled)
		gEngine.device->getDev()->SetDepthStencilSurface(m_rtDepth);
	for(u32 i = 0; i < m_numMRTs; ++i)
		gEngine.device->getDev()->SetRenderTarget(i, m_rtSurface[i]);
	for(u32 i = m_numMRTs; i < 4; ++i)
		gEngine.device->getDev()->SetRenderTarget(i, 0);
}

void MRT::end()
{
	gEngine.device->getDev()->SetRenderTarget(0, m_pOldRenderTarget);
	for(u32 i = 1; i < 4; ++i)
		gEngine.device->getDev()->SetRenderTarget(i, 0);
	gEngine.device->getDev()->SetDepthStencilSurface(m_pOldDSSurface);
}

void MRT::bindTextures()
{
	for(u32 i = 0; i < m_numMRTs; ++i)
		gEngine.device->getDev()->SetTexture(i, m_rt[i]);
}
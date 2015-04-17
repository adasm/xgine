#include "ShadowVolume.h"

ShadowVolume::ShadowVolume()
{
	m_initialized = 0;
	m_shader = 0;
}

u32 ShadowVolume::init()
{
	if(m_initialized)return 1;
	{
		m_shader = gEngine.shaderMgr->load("ShadowVolume.fx");
		if( !m_shader )
		{
			gEngine.kernel->log->prn("Could not initialize ShadowVolume. Loading shader failed.");
			return 0;
		}
	}
	m_initialized = 1;
	return 1;
}

void ShadowVolume::close()
{
	if(m_initialized)
	{
		m_initialized = 0;
	}
}

u32 ShadowVolume::prepareStencil(RenderList* in, Light* light)
{
	gEngine.device->getDev()->SetRenderState(D3DRS_ZENABLE,			 TRUE );
	gEngine.device->getDev()->SetRenderState(D3DRS_ZWRITEENABLE,	 FALSE);
	gEngine.device->getDev()->SetRenderState(D3DRS_ZFUNC,			 D3DCMP_LESSEQUAL);
	gEngine.device->getDev()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	gEngine.device->getDev()->SetRenderState(D3DRS_SHADEMODE,		 D3DSHADE_FLAT);
	gEngine.device->getDev()->SetRenderState(D3DRS_LIGHTING,		 FALSE);
	gEngine.device->getDev()->SetRenderState(D3DRS_STENCILENABLE,	 TRUE);

	gEngine.renderer->get().pass = REND_SV;
	if(light->getType() == LIGHT_DIR) gEngine.renderer->setShader(m_shader->setTech("ShadowVolumeDir"));
	else gEngine.renderer->setShader(m_shader->setTech("ShadowVolumePos"));
	gEngine.renderer->m_defaultShader = m_shader;
	//gEngine.renderer->renderListSV(&in->culledSolid, light);
	gEngine.renderer->get().pass = REND_SOLID;

	return 1;
}
#include "OcclusionCulling.h"

static f32 r_ocTexWidthScale = 0.25;
static f32 r_ocTexHeightScale = 0.25;

OcclusionCulling::OcclusionCulling() 
	: m_initialized(0), m_occlusionShader(0), d3dQuery(0), occlusionRender(0), occlusionSurface(0), occlusionTexture(0) 
{ 
}

u32 OcclusionCulling::init()
{
	if(m_initialized)return 1;
	REGCVARF32(r_ocTexWidthScale);
	REGCVARF32(r_ocTexHeightScale);
	gEngine.device->getDev()->CreateQuery( D3DQUERYTYPE_OCCLUSION, &d3dQuery );
    D3DDISPLAYMODE mode;
    gEngine.device->getDev()->GetDisplayMode( 0, &mode );
	u32 width = gEngine.device->getWidth() * r_ocTexWidthScale;
	u32	height = gEngine.device->getHeight() * r_ocTexHeightScale;
    if(FAILED(D3DXCreateTexture(gEngine.device->getDev(), width, height, 1, D3DUSAGE_RENDERTARGET, mode.Format, D3DPOOL_DEFAULT, &occlusionTexture)))
    {
		gEngine.kernel->log->prnEx(LT_ERROR, "OcclusionCulling", "Could not initialize OcclusionCulling. Creation of occlusionTexture failed.");
        return 0;
    }
    D3DSURFACE_DESC desc;
    occlusionTexture->GetSurfaceLevel(0, &occlusionSurface);
    occlusionSurface->GetDesc(&desc);
    if(FAILED(D3DXCreateRenderToSurface(gEngine.device->getDev(), desc.Width, desc.Height, desc.Format,TRUE, D3DFMT_D16, &occlusionRender)))
    {
		gEngine.kernel->log->prn(LT_ERROR, "OcclusionCulling", "Could not initialize OcclusionCulling. Creation of occlusionRender failed.");
        return 0;
    }
	m_occlusionShader = gEngine.shaderMgr->load("ZFill.fx");
	if(m_occlusionShader == 0)
	{
		gEngine.kernel->log->prn(LT_ERROR, "OcclusionCulling", "Could not initialize OcclusionCulling. Loading occlusion shader failed.");
        return 0;
    }
	m_initialized = 1;
    return 1;
}

void OcclusionCulling::close()
{
	if(!m_initialized)return;
	DXRELEASE(occlusionRender);
	DXRELEASE(occlusionSurface);
	DXRELEASE(occlusionTexture);
	DXRELEASE(d3dQuery);
	m_initialized = 0;
}

u32 OcclusionCulling::check(RenderList *in)
{
	if(!m_initialized)return 0;

	if(SUCCEEDED(occlusionRender->BeginScene(occlusionSurface, NULL)))
    {
		gEngine.device->getDev()->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200, 200, 200), 1.0f, 0);
       
		gEngine.renderer->get().pass = REND_ZFILL;

		gEngine.renderer->state.setZEnable(TRUE);
		gEngine.renderer->state.setZWriteEnable(TRUE);
		gEngine.renderer->state.setZFunc(D3DCMP_LESSEQUAL);
		gEngine.renderer->state.setColorWriteEnable(0x0000000f);
		gEngine.renderer->state.setAlphaBlendEnable(FALSE);
		gEngine.renderer->state.setStencilEnable(FALSE);
		gEngine.renderer->state.setShadeMode(D3DSHADE_GOURAUD);
		gEngine.renderer->state.setLighting(FALSE);

		gEngine.renderer->renderListRaw(&in->culledSurfaces);
	
		DWORD pixelsVisible;

		u32 count = 0;

		for(u32 i = 0; i < in->culledSurfaces.dataSize; i++)
		{	
			Surface *surfacePrev = 0;
			Surface *surfaceNext = in->culledSurfaces.data[i];

			while(surfaceNext)
			{
				d3dQuery->Issue(D3DISSUE_BEGIN);
				gEngine.renderer->setShader(surfaceNext->shader->setTech("RenderRaw"));
				gEngine.renderer->setMaterial(surfaceNext->material);
				gEngine.renderer->setWorld(&surfaceNext->world);
				if(surfaceNext->material->m_type == MT_LEAF)
				{
					surfaceNext->shader->setVec3("g_boundingBoxSphereCenter", &surfaceNext->sphereCenter);
					surfaceNext->shader->setF32("g_boundingBoxSphereRadius", surfaceNext->sphereRadius);
				}
				gEngine.renderer->commitChanges();
				surfaceNext->mesh->render();
				d3dQuery->Issue(D3DISSUE_END);

				pixelsVisible = 0;
				while(d3dQuery->GetData((void *) &pixelsVisible, sizeof(DWORD), D3DGETDATA_FLUSH) == S_FALSE);

				if(pixelsVisible == 0 && surfacePrev)
				{
					surfacePrev->next = surfaceNext->next;
					count++;
				}

				surfacePrev = surfaceNext;
				surfaceNext = surfaceNext->next;
			}
		}


		gEngine.renderer->setShader(0);
		occlusionRender->EndScene(0);

		gEngine.renderer->addTxt("OC: culled: %u", count);
		return 1;
    }
    return 0;
}
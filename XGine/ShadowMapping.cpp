#include "ShadowMapping.h"

ShadowMapping::ShadowMapping()
	:	m_initialized(0), m_castingShader(0)
{
}

u32 ShadowMapping::init()
{
	if(m_initialized)return 1;
	{
		m_castingShader = gEngine.shaderMgr->load("ShadowMapping.fx");
		m_noiseMap = gEngine.resMgr->load<Texture>("noiseNormals.png");
		if( !m_castingShader )
		{
			gEngine.kernel->log->prn(LT_ERROR, "ShadowMapping", "Could not initialize ShadowMapping. Loading casting shader failed.");
			return 0;
		}
	}
	m_initialized = 1;
	return 1;
}

void ShadowMapping::close()
{
	if(m_initialized)
	{
		gEngine.resMgr->release(m_noiseMap);
		m_initialized = 0;
	}
}


u32 ShadowMapping::renderDepth(RenderTexture* rt, RenderList* rl, Light* light)
{
	if(!light || !rt || !rl)
		return 0;

	if(light->getType() == LIGHT_SPOT)
	{
		//Check for entities inside light-shape
		rl->culledSurfaces.clear();
		PerspectiveCamera camera;
		camera.setFOV(light->getPhi());
		camera.setPosition(*light->getPos());
		camera.lookAt(*light->getPos() + *light->getDir());
		camera.update();
		FrustumCulling::check(&camera, rl->surfaceVector, &rl->culledSurfaces);
		rl->surfaceOctree->cull(&camera, &rl->culledSurfaces);
		rt->enableRendering();
		gEngine.renderer->get().light = light;
		REND_PASS oldRendPass = gEngine.renderer->get().pass;
		gEngine.renderer->get().pass = REND_SM;
		gEngine.renderer->clear(1, 1, 0, 0xffffffff);
		if(r_enabledVarianceShadowMapping && !r_enabledExponentialShadowMapping)
			gEngine.renderer->setShader(m_castingShader->setTech("RenderVSM"));
		else
			gEngine.renderer->setShader(m_castingShader->setTech("RenderSM"));
		gEngine.renderer->m_defaultShader = m_castingShader;
		gEngine.renderer->state.setStencilEnable(FALSE);
		gEngine.renderer->state.setZEnable(TRUE);
		gEngine.renderer->state.setZWriteEnable(TRUE);
		gEngine.renderer->state.setZFunc(D3DCMP_LESS);
		gEngine.renderer->state.setColorWriteEnable(0x0000000f);
		gEngine.renderer->state.setAlphaBlendEnable(FALSE);
		m_castingShader->commitChanges();
			u32 rendered = gEngine.renderer->renderListRaw(&rl->culledSurfaces);
		gEngine.renderer->setShader(0);
		rt->disableRendering();
		gEngine.renderer->get().pass = oldRendPass;
		gEngine.renderer->get().light = 0;
 
		gEngine.renderer->addTxt("renderDepth : Spot : rendered %u entities", rendered);

		if(gEngine.input->isKeyDown(DIK_T))
		{
			gEngine.renderer->setShader(m_castingShader->setTech("quad2"));
			gEngine.renderer->state.setAlphaBlendEnable(FALSE);
			gEngine.renderer->state.setZEnable(FALSE);
			gEngine.renderer->state.setZWriteEnable(FALSE);
			m_castingShader->setTex("ColorMap", rt->getColorTexture());
			m_castingShader->commitChanges();
			gEngine.renderer->r2d->drawScreenQuad(0xffffffff, 1);
			gEngine.renderer->setShader(0);
			
		}
		return 1;
	}
	else if(light->getType() == LIGHT_DIR)
	{
		gEngine.renderer->state.setStencilEnable(FALSE);
		gEngine.renderer->state.setZEnable(TRUE);
		gEngine.renderer->state.setZWriteEnable(TRUE);
		gEngine.renderer->state.setZFunc(D3DCMP_LESSEQUAL);
		gEngine.renderer->state.setColorWriteEnable(0x0000000f);
		gEngine.renderer->state.setAlphaBlendEnable(FALSE);
		
		rl->culledSurfaces.clear();
		PerspectiveCamera camera;
		camera.setFOV(light->getPhi());
		camera.setPosition(*light->getPos());
		camera.lookAt(*light->getPos() + *light->getDir());
		camera.update();
		FrustumCulling::check(&camera, rl->surfaceVector, &rl->culledSurfaces);
		rl->surfaceOctree->cull(&camera, &rl->culledSurfaces);

		rt->enableRendering();
		gEngine.renderer->get().light = light;
		REND_PASS oldRendPass = gEngine.renderer->get().pass;
		gEngine.renderer->get().pass = REND_SM;
		gEngine.renderer->clear(1, 1, 0, 0xffffffff);
		if(r_enabledVarianceShadowMapping && !r_enabledExponentialShadowMapping)
			gEngine.renderer->setShader(m_castingShader->setTech("RenderVSM"));
		else
			gEngine.renderer->setShader(m_castingShader->setTech("RenderSM"));
		gEngine.renderer->m_defaultShader = m_castingShader;

			//rl->renderRawRange(*light->getPos()+*light->getDir()*g_shadowMapDirViewDist/2, g_shadowMapDirViewVolume + 5000);
			
		u32 rendered = gEngine.renderer->renderListRaw(&rl->culledSurfaces);
			
		gEngine.renderer->setShader(0);
		rt->disableRendering();
		gEngine.renderer->get().pass = oldRendPass;
		gEngine.renderer->get().light = 0;

		gEngine.renderer->addTxt("renderDepth : Dir : rendered %u entities", rendered);
 
		if(gEngine.input->isKeyDown(DIK_T))
		{
			gEngine.renderer->setShader(m_castingShader->setTech("quad2"));
			gEngine.renderer->state.setAlphaBlendEnable(FALSE);
			m_castingShader->setTex("ColorMap", rt->getColorTexture());
			m_castingShader->commitChanges();
			gEngine.renderer->r2d->drawScreenQuad(0xffffffff, 1);
			gEngine.renderer->setShader(0);
			
		}
		return 1;
	}
	
	return 0;
}

u32 ShadowMapping::prepareShadowMap(RenderList* in, Light* light, RenderTexture *screenMap)
{
	/*PROFILER("prepareShadowMap");
	
	light->renderDepth(in);

	////UNLIT
	gEngine.renderer->get().light = 0;
	screenMap->enableRendering();
	gEngine.renderer->clear(1,1,0,0xffffffff);
	gEngine.renderer->get().pass = REND_UNLIT;
	gEngine.renderer->m_defaultShader = m_castingShader;
	gEngine.device->getDev()->SetRenderState(D3DRS_ZENABLE,				TRUE );
	gEngine.device->getDev()->SetRenderState(D3DRS_ZWRITEENABLE,		TRUE);
	gEngine.device->getDev()->SetRenderState(D3DRS_ZFUNC,				D3DCMP_LESSEQUAL);
	gEngine.device->getDev()->SetRenderState(D3DRS_COLORWRITEENABLE,	0x0000000f);
	
	if(r_enabledShadowMappingPCF)
	{
		if(r_enabledShadowMappingPCFHigh)
		{
			if(r_enabledShadowMappingGeoRendering)
				gEngine.renderer->setShader(m_castingShader->setTech("techUnlitPCFHighGeo"));
			else
				gEngine.renderer->setShader(m_castingShader->setTech("techUnlitPCFHigh"));
			// 7x7 PCF
			f32 fTexelSize = (f32)(1.0f/(f32)light->getDepthMap()->getParams().width);
			Vec4 vTexCoords[49];
			u32 i = 0;
			for(f32 x = -3; x <= 3; x++)
				for(f32 y = -3; y <= 3; y++)
					vTexCoords[i++] = Vec4(fTexelSize * (f32)x, fTexelSize * (f32)y, 0.0f, 0.0f);
			m_castingShader->setVal("vTexCoords7x7", &vTexCoords, 49 * sizeof(Vec4));
		}
		else if(r_enabledShadowMappingPCFMed)
		{
			if(r_enabledShadowMappingGeoRendering)
				gEngine.renderer->setShader(m_castingShader->setTech("techUnlitPCFMedGeo"));
			else
				gEngine.renderer->setShader(m_castingShader->setTech("techUnlitPCFMed"));
			// 5x5 PCF
			f32 fTexelSize = (f32)(1.0f/(f32)light->getDepthMap()->getParams().width);
			Vec4 vTexCoords[25];
			u32 i = 0;
			for(f32 x = -2; x <= 2; x++)
				for(f32 y = -2; y <= 2; y++)
					vTexCoords[i++] = Vec4(fTexelSize * (f32)x, fTexelSize * (f32)y, 0.0f, 0.0f);
			m_castingShader->setVal("vTexCoords5x5", &vTexCoords, 25 * sizeof(Vec4));
		}
		else
		{
			if(r_enabledShadowMappingGeoRendering)
				gEngine.renderer->setShader(m_castingShader->setTech("techUnlitPCFLowGeo"));
			else
				gEngine.renderer->setShader(m_castingShader->setTech("techUnlitPCFLow"));
			// 3x3 PCF
			f32 fTexelSize = (f32)(1.0f/(f32)light->getDepthMap()->getParams().width);
			Vec4 vTexCoords[9];
			u32 i = 0;
			for(f32 x = -1; x <= 1; x++)
				for(f32 y = -1; y <= 1; y++)
					vTexCoords[i++] = Vec4(fTexelSize * (f32)x, fTexelSize * (f32)y, 0.0f, 0.0f);
			m_castingShader->setVal("vTexCoords3x3", &vTexCoords, 9 * sizeof(Vec4));
		}
	}
	else
	{
		if(r_enabledShadowMappingGeoRendering)
			gEngine.renderer->setShader(m_castingShader->setTech("techUnlitGeo"));
		else
			gEngine.renderer->setShader(m_castingShader->setTech("techUnlit"));
	}
	
	Mat texViewProj = (*(light->getViewProjMat())) * (*(light->getDepthMap()->getTexAdjMat()));
	m_castingShader->setMat("g_matTexture", &texViewProj);
	m_castingShader->setTex("ShadowMap", light->getDepthMap()->getColorTexture());
	m_castingShader->setTex("DEPTHMAP", gEngine.renderer->getMRT()->getRT(2));

	 Mat g_matInvViewProj;
	 D3DXMatrixInverse(&g_matInvViewProj, 0, gEngine.renderer->get().camera->getViewProjectionMat());
	m_castingShader->setMat("g_matInvViewProj", &g_matInvViewProj);
	m_castingShader->setF32("zBias", r_shadowMappingZBias);
	commitChanges();

	if(r_enabledShadowMappingGeoRendering)
		gEngine.renderer->renderListRaw(&in->culledSolid);
	else
		gEngine.renderer->r2d->drawScreenQuad(0xffffffff, 1);

	gEngine.renderer->setShader(0);
	screenMap->disableRendering();

	//DEBUG
	if(gEngine.input->isKeyDown(DIK_P))
	{
		gEngine.renderer->setShader(m_castingShader->setTech("quad"));
		m_castingShader->setTex("ColorMap", screenMap->getColorTexture());
		commitChanges();
		gEngine.renderer->r2d->drawScreenQuad(0xffffffff, 1);
		gEngine.renderer->setShader(0);
	}
	*/
	return 0;
}
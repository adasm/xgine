#include "HDRToneMap.h"

struct VertexTransformedTextured
{
	D3DXVECTOR4 p;
	f32		u;
	f32		v;
};
const DWORD VertexTransformedTextured_FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

HDRToneMap::HDRToneMap()
{
	initialized = 0; failure = 1;
	blur = 0;
	shader = 0;
	downSampled = blured = bluredTemp = luminance = avgLuminance = prevLuminance = currentLuminance = 0;
	toneMapTextures.clear();
	toneMapQuads.clear();
	m_vbHalf = 0;
}

HDRToneMap::~HDRToneMap()
{
}

u32 HDRToneMap::init()
{
	if(initialized && !failure)return 1;

	close();
	failure = 1;

	blur = new GuassianBlur();
	
	if(!blur->init(7))
	{ initialized = 1; failure = 1; gEngine.kernel->log->prnEx(LT_ERROR, "HDRToneMap", "Couldn't initialize HDRToneMap!"); return 0; }

	shader = gEngine.shaderMgr->load("HDR.fx");

	if(!shader)
	{ initialized = 1; failure = 1; gEngine.kernel->log->prnEx(LT_ERROR, "HDRToneMap", "Couldn't initialize HDRToneMap!"); return 0; }

	downSampled = new RenderTexture();
	RenderTexture::CreationParams cp;
	cp.width = gEngine.device->getWidth()/2;
	cp.height = gEngine.device->getHeight()/2;
	cp.colorFormat = D3DFMT_A16B16G16R16F;
	cp.depthFormat = D3DFMT_D24S8;
	downSampled = new RenderTexture();
	if(!downSampled->init(cp))
	{
		delete(downSampled);
		downSampled = 0;
		{ initialized = 1; failure = 1; gEngine.kernel->log->prnEx(LT_ERROR, "HDRToneMap", "Couldn't initialize HDRToneMap - Unable to create downSampled!"); return 0; }
	}
	

	blured = new RenderTexture();
	if(!blured->init(cp))
	{
		delete(blured);
		blured = 0;
		{ initialized = 1; failure = 1; gEngine.kernel->log->prnEx(LT_ERROR, "HDRToneMap", "Couldn't initialize HDRToneMap - Unable to create blured!"); return 0; }
	}
	
	bluredTemp = new RenderTexture();
	if(!bluredTemp->init(cp))
	{
		delete(bluredTemp);
		bluredTemp = 0;
		{ initialized = 1; failure = 1; gEngine.kernel->log->prnEx(LT_ERROR, "HDRToneMap", "Couldn't initialize HDRToneMap - Unable to create bluredTemp!"); return 0; }
	}

	cp.width = gEngine.device->getWidth();
	cp.height = gEngine.device->getHeight();
	luminance = new RenderTexture();
	if(!luminance->init(cp))
	{
		delete(luminance);
		luminance = 0;
		{ initialized = 1; failure = 1; gEngine.kernel->log->prnEx(LT_ERROR, "HDRToneMap", "Couldn't initialize HDRToneMap - Unable to create luminance!"); return 0; }
	}

	f32 width = gEngine.device->getWidth();
	f32 height = gEngine.device->getHeight();

	VertexTransformedTextured* data;

	if(FAILED(gEngine.device->getDev()->CreateVertexBuffer(4 * sizeof(VertexTransformedTextured), D3DUSAGE_WRITEONLY, VertexTransformedTextured_FVF,
		D3DPOOL_MANAGED, &m_vbHalf, 0)))
		{ initialized = 1; failure = 1; gEngine.kernel->log->prnEx(LT_ERROR, "HDRToneMap", "Couldn't initialize HDRToneMap - Unable to create vertex buffer!"); return 0; }

	if(FAILED(m_vbHalf->Lock(0, 0, (void**)&data, 0)))
		{ initialized = 1; failure = 1; gEngine.kernel->log->prnEx(LT_ERROR, "HDRToneMap", "Couldn't initialize HDRToneMap - Unable to lock vertex buffer!"); return 0; }
	data[0].p = D3DXVECTOR4(-0.5f, height/2-0.5f, 0.0f, 1.0f);
	data[0].u = 0.0f;
	data[0].v = 1.0f;
	data[1].p = D3DXVECTOR4(width/2-0.5f, height/2-0.5f, 0.0f, 1.0f);
	data[1].u = 1.0f;
	data[1].v = 1.0f;
	data[2].p = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
	data[2].u = 0.0f;
	data[2].v = 0.0f;
	data[3].p = D3DXVECTOR4(width/2-0.5f, -0.5f, 0.0f, 1.0f);
	data[3].u = 1.0f;
	data[3].v = 0.0f;
	m_vbHalf->Unlock();


	unsigned int numMipLevelsH = static_cast<int>((logf(static_cast<float>(height)) / logf(2.0f))) + 1;
	unsigned int numMipLevelsW = static_cast<int>((logf(static_cast<float>(width)) / logf(2.0f))) + 1;
	unsigned int numMipLevels = numMipLevelsH > numMipLevelsW ? numMipLevelsH : numMipLevelsW;

	toneMapTextures.resize(numMipLevels - 1);
	toneMapQuads.resize(numMipLevels - 1);

	//we create rendertargets to the 1x1 level, but we don't create one for the top level
	for (unsigned int i = 0; i < numMipLevels - 1; ++i)
	{
		unsigned int mipHeight = (u32)height >> (i + 1);
		mipHeight = (mipHeight == 0) ? 1 : mipHeight;
		unsigned int mipWidth = (u32)width >> (i + 1);
		mipWidth = (mipWidth == 0) ? 1 : mipWidth;

		toneMapTextures[i] = new RenderTexture();
		cp.width = mipWidth;
		cp.height = mipHeight;
		toneMapTextures[i]->init(cp);

		if(FAILED(gEngine.device->getDev()->CreateVertexBuffer(4 * sizeof(VertexTransformedTextured), D3DUSAGE_WRITEONLY, VertexTransformedTextured_FVF, D3DPOOL_MANAGED, &toneMapQuads[i], 0)))
			{ initialized = 1; failure = 1; gEngine.kernel->log->prnEx(LT_ERROR, "HDRToneMap", "Couldn't initialize HDRToneMap - Unable to create vertex buffer!"); return 0; }
		
		if(FAILED(toneMapQuads[i]->Lock(0, 0, (void**)&data, 0)))
			{ initialized = 1; failure = 1; gEngine.kernel->log->prnEx(LT_ERROR, "HDRToneMap", "Couldn't initialize HDRToneMap - Unable to lock vertex buffer!"); return 0; }

		data[0].p = D3DXVECTOR4(-0.5f, mipHeight-0.5f, 0.0f, 1.0f);
		data[0].u = 0.0f;
		data[0].v = 1.0f;
		data[1].p = D3DXVECTOR4(mipWidth-0.5f, mipHeight-0.5f, 0.0f, 1.0f);
		data[1].u = 1.0f;
		data[1].v = 1.0f;
		data[2].p = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
		data[2].u = 0.0f;
		data[2].v = 0.0f;
		data[3].p = D3DXVECTOR4(mipWidth-0.5f, -0.5f, 0.0f, 1.0f);
		data[3].u = 1.0f;
		data[3].v = 0.0f;
		toneMapQuads[i]->Unlock();
	}

	cp.width = 1;
	cp.height = 1;
	prevLuminance = new RenderTexture();
	if(!prevLuminance->init(cp))
	{
		delete(luminance);
		luminance = 0;
		{ initialized = 1; failure = 1; gEngine.kernel->log->prnEx(LT_ERROR, "HDRToneMap", "Couldn't initialize HDRToneMap - Unable to create prevLuminance!"); return 0; }
	}

	currentLuminance = new RenderTexture();
	if(!currentLuminance->init(cp))
	{
		delete(currentLuminance);
		currentLuminance = 0;
		{ initialized = 1; failure = 1; gEngine.kernel->log->prnEx(LT_ERROR, "HDRToneMap", "Couldn't initialize HDRToneMap - Unable to create currentLuminance!"); return 0; }
	}

	initialized = 1; failure = 0;
	return 1;
}

void HDRToneMap::close()
{
	if(!initialized)return;
	initialized = failure = 0;
	if(blur) { delete(blur); blur = 0; }
	if(downSampled) { downSampled->close(); delete(downSampled); downSampled = 0; }
	if(blured) { blured->close(); delete(blured); blured = 0; }
	if(bluredTemp) { bluredTemp->close(); delete(bluredTemp); bluredTemp = 0; }
	if(luminance) { luminance->close(); delete(luminance); luminance = 0; }
	if(avgLuminance) { avgLuminance->close(); delete(avgLuminance); avgLuminance = 0; }
	if(prevLuminance) { prevLuminance->close(); delete(prevLuminance); prevLuminance = 0; }
	if(currentLuminance) { currentLuminance->close(); delete(currentLuminance); currentLuminance = 0; }
	for(u32 i = 0; i < toneMapTextures.size(); i++)
		if(toneMapTextures[i]) { toneMapTextures[i]->close(); delete(toneMapTextures[i]); toneMapTextures[i] = 0; }
	toneMapTextures.clear();
	for(u32 i = 0; i < toneMapQuads.size(); i++)
		if(toneMapQuads[i]) { toneMapQuads[i]->Release(); toneMapQuads[i] = 0; }
	toneMapQuads.clear();
	if(m_vbHalf) { m_vbHalf->Release(); m_vbHalf = 0; }
}

void HDRToneMap::process(RenderTexture *lightTranportTexture)
{
	if(failure)return;

	gEngine.renderer->state.setAlphaBlendEnable(FALSE);


	//ScreenSpace light // r_vlsEnabled
	Vec3 sunPosScreen;
	Vec2 vecSunToCenter;
	Mat world;
	Vec2 screenCenter;
	D3DVIEWPORT9 viewport;
	gEngine.device->getDev()->GetViewport(&viewport);
	D3DXMatrixTranslation(&world, 0, 0, 0);
	D3DXVec3Project(&sunPosScreen, &sunPos, &viewport, gEngine.renderer->get().camera->getProjectionMat(), gEngine.renderer->get().camera->getViewMat(), &world);
    Vec2 half_texel = Vec2(1.0f/(f32)gEngine.device->getWidth(), 1.0f/(f32)gEngine.device->getHeight());
	Vec2 sunPosSS = Vec2(sunPosScreen);
		 sunPosSS.x *= half_texel.x;
		 sunPosSS.y *= half_texel.y;
		 sunPosSS += 0.5*half_texel;

	if(r_hdrToneMap)
	{
		luminance->enableRendering();
		gEngine.renderer->clear(1,0,0);
		gEngine.renderer->setShader(shader->setTech("ConvertToLuminance"));
		shader->setTex("LightTransport", lightTranportTexture->getColorTexture());
		gEngine.renderer->commitChanges();
		gEngine.renderer->r2d->drawScreenQuadRaw();
		//luminance->disableRendering();

		for(u32 i = 0; i < toneMapTextures.size(); i++)
		{
			toneMapTextures[i]->enableRendering();
			gEngine.renderer->clear(1,0,0);
			gEngine.renderer->setShader(shader->setTech("DownSampleLum"));
			shader->setTex("Luminance", (i==0)?(luminance->getColorTexture()):(toneMapTextures[i-1]->getColorTexture()));
			gEngine.renderer->commitChanges();
			gEngine.device->getDev()->SetFVF(VertexTransformedTextured_FVF);
			gEngine.device->getDev()->SetStreamSource(0, toneMapQuads[i], 0, sizeof(VertexTransformedTextured));
			gEngine.device->getDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			//toneMapTextures[i]->disableRendering();
		}

		toneMapTextures[toneMapTextures.size()-1]->disableRendering();
	}

	if(r_hdrEyeAdaptation)
	{
		static RenderTexture* temp;
		temp = prevLuminance;
		prevLuminance = currentLuminance;
		currentLuminance = temp;

		currentLuminance->enableRendering();
		gEngine.renderer->clear(1,0,0);
		gEngine.renderer->setShader(shader->setTech("Adaptation"));
		shader->setTex("PrevLum", prevLuminance->getColorTexture());
		shader->setTex("CurrentLum", toneMapTextures[toneMapTextures.size()-1]->getColorTexture());
		shader->setF32("g_time", gEngine.kernel->tmr->get() * 0.001f * r_hdrEyeAdaptationTimeScale);
		gEngine.renderer->commitChanges();
		gEngine.device->getDev()->SetFVF(VertexTransformedTextured_FVF);
		gEngine.device->getDev()->SetStreamSource(0, toneMapQuads[toneMapTextures.size()-1], 0, sizeof(VertexTransformedTextured));
		gEngine.device->getDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		currentLuminance->disableRendering();
	}

	if(r_hdrToneMap && r_hdrBrightPass && r_hdrBloom)
	{
		downSampled->enableRendering();
		gEngine.renderer->clear(1,0,0);
		if(r_enabledVLS && r_enabledHDRVLS) 
		{
			gEngine.renderer->setShader(shader->setTech("BrightPassVLS"));
			shader->setTex("Tex0", gEngine.renderer->vls->getTexture()->getColorTexture());
		}
		else if(r_enabledHDRVLS)
		{
			gEngine.renderer->setShader(shader->setTech("BrightPassHDRVLS"));
		}
		else
		{
			gEngine.renderer->setShader(shader->setTech("BrightPass"));
		}
		shader->setTex("LightTransport", lightTranportTexture->getColorTexture());
		shader->setTex("AvgLuminance", (r_hdrEyeAdaptation)?(currentLuminance->getColorTexture()):(toneMapTextures[toneMapTextures.size()-1]->getColorTexture()));
		shader->setTex("Luminance", luminance->getColorTexture());
		shader->setF32("g_tonemapScale", r_hdrToneMapScale);
		shader->setVec2("g_sunPosSS", &sunPosSS);
		gEngine.renderer->commitChanges();
		gEngine.device->getDev()->SetFVF(VertexTransformedTextured_FVF);
		gEngine.device->getDev()->SetStreamSource(0, m_vbHalf, 0, sizeof(VertexTransformedTextured));
		gEngine.device->getDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		//downSampled->disableRendering();

		blur->process(downSampled, bluredTemp, blured);
	}
	else if(r_hdrBloom)
	{
		blur->process(lightTranportTexture, bluredTemp, blured);
	}

	if(r_hdrColorGradeEnabled)
	{
		shader->setVec3("g_colorGrade", &r_hdrColorGrade);
	}
	else
	{
		shader->setVec3("g_colorGrade", &Vec3(1,1,1));
	}

	if(r_hdrToneMap && r_hdrBloom)
	{
		gEngine.renderer->setShader(shader->setTech("ToneMapBloom"));
		shader->setTex("LightTransport", lightTranportTexture->getColorTexture());
		shader->setTex("AvgLuminance", (r_hdrEyeAdaptation)?(currentLuminance->getColorTexture()):(toneMapTextures[toneMapTextures.size()-1]->getColorTexture()));
		shader->setTex("Luminance", luminance->getColorTexture());
		shader->setTex("BlurTexture", blured->getColorTexture());
		shader->setF32("g_tonemapScale", r_hdrToneMapScale);
		shader->setF32("g_blurAmount", r_hdrBloomAmount);		
		gEngine.renderer->commitChanges();
		gEngine.renderer->r2d->drawScreenQuadRaw();
	}
	else if(r_hdrToneMap)
	{
		gEngine.renderer->setShader(shader->setTech("ToneMap"));
		shader->setTex("LightTransport", lightTranportTexture->getColorTexture());
		shader->setTex("AvgLuminance", (r_hdrEyeAdaptation)?(currentLuminance->getColorTexture()):(toneMapTextures[toneMapTextures.size()-1]->getColorTexture()));
		shader->setTex("Luminance", luminance->getColorTexture());
		shader->setF32("g_tonemapScale", r_hdrToneMapScale);
		gEngine.renderer->commitChanges();
		gEngine.renderer->r2d->drawScreenQuadRaw();
	}
	else if(r_hdrBloom)
	{
		gEngine.renderer->setShader(shader->setTech("HDRBloom"));
		shader->setTex("LightTransport", lightTranportTexture->getColorTexture());
		shader->setTex("BlurTexture", blured->getColorTexture());
		shader->setF32("g_blurAmount", r_hdrBloomAmount);
		gEngine.renderer->commitChanges();
		gEngine.renderer->r2d->drawScreenQuadRaw();
	}
	else
	{
		gEngine.renderer->setShader(shader->setTech("HDRRender"));
		shader->setTex("LightTransport", lightTranportTexture->getColorTexture());
		gEngine.renderer->commitChanges();
		gEngine.renderer->r2d->drawScreenQuadRaw();
	}

	gEngine.renderer->addTxt("Performed HDR");
}
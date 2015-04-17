/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#include "Renderer.h"

f32 g_timeFadeMax = 2.0f;
f32 g_timeFadeHold = 2.5f;
f32 g_timeFadeIn = 1.0f;
f32 g_fadeYPos = 0.0f;
f32 g_fadeYRel = 0.0f;

u32 g_loadingFadeEnabled = 0;
f32 g_loadingFadeSpeed = 0.0005;
f32 g_loadingFade = 1.0f;

const u32 maxInstanced = 20000;

const u32 G_LOADING_SPLASH_ENABLED = 1;

Renderer::Renderer() 
	:	m_initialized(0), pQuery(0), oc(0), sm(0), m_fadestrNum(0), r2d(0), m_dev(0), m_hdrRendering(0), m_accumulationRT_HDR(0), m_hdrToneMap(0)
{
}

u32 Renderer::init()
{			
	if(m_initialized)return 1;

	m_dev = gEngine.device->getDev();

	state.reset();

	r2d = new Renderer2D();
	if(!r2d->init())return 0;

	memset(&m_rend, 0, sizeof(RendSet));
	for(u32 i = 0; i < 8; i++)m_textures[i] = 0;

	RenderTexture::CreationParams cp;
	cp.width = gEngine.device->getWidth();
	cp.height = gEngine.device->getHeight();
	cp.colorFormat = D3DFMT_A8R8G8B8;
	cp.depthFormat = D3DFMT_D24S8;
	screenMap = new RenderTexture();
	if(!screenMap->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Renderer", "Unable to create screenMap!");
		return 0;
	}

	//if(G_LOADING_SPLASH_ENABLED)
	{
		g_loadingFade = 1.0f;
		begin();
			screenMap->enableRendering();
			clear(1,1,1,0xff112233);
				r2d->begin();
					commitChanges();
					r2d->drawQuad(0, 0, gEngine.device->getWidth(), gEngine.device->getHeight(), D3DXCOLOR(0.5, 0.3, 0.1, g_loadingFade), D3DXCOLOR(0.5, 0.3, 0.1, g_loadingFade), D3DXCOLOR(0.1, 0.1, 0.1, g_loadingFade), D3DXCOLOR(0.1, 0.1, 0.1, g_loadingFade));
					RECT rect = { 0, 10, gEngine.device->getWidth(), 30 };
					r2d->drawTextSmall(rect, D3DXCOLOR(0.7, 0.85, 1, g_loadingFade), DT_CENTER, XGINE_VER);
					r2d->drawTextBigCenter(0,gEngine.device->getHeight()/2.0 - 50, D3DXCOLOR(1.0, 0.6, 0.2, g_loadingFade), g_appName.c_str());
					r2d->drawTextCenter(0,gEngine.device->getHeight()/2.0, D3DXCOLOR(1.0, 0.7, 0.4, g_loadingFade), "L  O  A  D  I  N  G");
					r2d->drawTextCenter(0,gEngine.device->getHeight()/2.0+50, D3DXCOLOR(0.2, 0.5, 0.8, g_loadingFade), "Press F2 to start loading-thread");
					r2d->drawTextCenter(0,gEngine.device->getHeight()/2.0+70, D3DXCOLOR(0.1, 0.4, 0.7, g_loadingFade), "Press ~ to open in-game console view and be able to input commands");
					r2d->drawTextCenter(0,gEngine.device->getHeight()/2.0+90, D3DXCOLOR(0.0, 0.3, 0.6, g_loadingFade), "Press H+M to show statistics and settings");
					r2d->drawTextEx("Arial '25'", 0,0, D3DXCOLOR(0.4, 0.6, 0.9, g_loadingFade), "%.2f sec", gEngine.kernel->tmr->getAppTime()/1000.0f);
				r2d->end();
			screenMap->disableRendering();
			r2d->begin();
			r2d->drawQuad(screenMap->getColorTexture(), 0, 0, gEngine.device->getWidth(), gEngine.device->getHeight(), D3DXCOLOR(1, 1, 1, g_loadingFade), D3DXCOLOR(1, 1, 1, g_loadingFade), D3DXCOLOR(1, 1, 1, g_loadingFade), D3DXCOLOR(1, 1, 1, g_loadingFade)); 
			r2d->end();
		end(0, 0);
		g_loadingFadeEnabled = 1;
	}

	m_dev->CreateQuery(D3DQUERYTYPE_EVENT, &pQuery);
	pQuery->Issue(D3DISSUE_END);
	m_dev->GetRenderTarget(0, &m_screenRT);

	/*if(FAILED(D3DXCreateTexture(m_dev, gEngine.device->getWidth(), gEngine.device->getHeight(), 1,	D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_backBuffer)))
	{
		MessageBox( 0, TEXT("Unable to create m_backBuffer!"), ERROR, MB_OK | MB_ICONERROR );
		return 0;
	}*/

	oc = new OcclusionCulling();
	oc->init();
	sm = new ShadowMapping();
	sm->init();
	vls = new VolumetricLightScattering();
	vls->init();

	m_mrt = new MRT();
	D3DFORMAT fmt[] = { D3DFMT_A8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_R32F };
	//D3DFORMAT fmt[] = { D3DFMT_A32B32G32R32F, D3DFMT_A32B32G32R32F, D3DFMT_A32B32G32R32F };
	m_mrt->init(3, fmt);

	
	cp.width = gEngine.device->getWidth();
	cp.height = gEngine.device->getHeight();
	cp.colorFormat = D3DFMT_A8R8G8B8;
	cp.depthFormat = D3DFMT_D24S8;
	m_depthNormals = new RenderTexture();
	if(!m_depthNormals->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Renderer", "Unable to create m_depthNormals!");
		return 0;
	}

	m_lightingLPP = new RenderTexture();
	if(!m_lightingLPP->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Renderer", "Unable to create m_lightingLPP!");
		return 0;
	}


	if(gEngine.device->isHDREnabled())
	{
		RenderTexture::CreationParams cp;
		cp.width = gEngine.device->getWidth();
		cp.height = gEngine.device->getHeight();
		cp.colorFormat = D3DFMT_A16B16G16R16F;
		cp.depthFormat = D3DFMT_D24S8;
		m_accumulationRT_HDR = new RenderTexture();
		if(!m_accumulationRT_HDR->init(cp))
		{
			gEngine.kernel->log->prnEx(LT_FATAL, "Renderer", "Unable to create m_accumulationRT_HDR!");
			return 0;
		}

		m_hdrToneMap = new HDRToneMap();
		if(!m_hdrToneMap->init())
		{
			delete(m_hdrToneMap);
			m_hdrToneMap = 0;
		}
		m_hdrRendering = 0;
	}
	else
	{
		m_accumulationRT_HDR = 0;
		m_hdrToneMap = 0;
		m_hdrRendering = 0;
	}

	m_depthOfField = new DepthOfField();
	m_depthOfField->init();

	m_heatHaze = new HeatHaze();
	m_heatHaze->init();

	m_zfillShader			= gEngine.shaderMgr->load("ZFill.fx");
	m_guiShader				= gEngine.shaderMgr->load("GUIShader.fx");
	m_deferredShaderFill	= gEngine.shaderMgr->load("DeferredFill.fx");
	m_deferredShaderLight	= gEngine.shaderMgr->load("DeferredLight.fx");
	m_deferredLightVolume	= gEngine.shaderMgr->load("ZFill.fx");
	m_forwardShader			= gEngine.shaderMgr->load("ForwardLight.fx");
	//m_waterShader			= gEngine.shaderMgr->load("waterD.fx");

	m_fadestrNum = 0;
	g_fadeYPos = 200.0f;
	g_fadeYRel = 0.0f;
	memset(&m_rend, 0, sizeof(RendSet));
	m_rend.type = REND_DEFERRED;

	cp.width = gEngine.device->getWidth();
	cp.height = gEngine.device->getHeight();
	cp.colorFormat = D3DFMT_A8R8G8B8;
	cp.depthFormat = D3DFMT_D24S8;
	screenMap = new RenderTexture();
	if(!screenMap->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Renderer", "Unable to create screenMap!");
		return 0;
	}
	m_backBuffer = new RenderTexture();
	if(!m_backBuffer->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Renderer", "Unable to create m_backBuffer!");
		return 0;
	}

	m_backBufferBlured = new RenderTexture();
	if(!m_backBufferBlured->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Renderer", "Unable to create m_backBufferBlured!");
		return 0;
	}

	m_backBufferBluredTemp = new RenderTexture();
	if(!m_backBufferBluredTemp->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Renderer", "Unable to create m_backBufferBluredTemp!");
		return 0;
	}

	cp.width = gEngine.device->getWidth()/2;
	cp.height = gEngine.device->getHeight()/2;
	cp.colorFormat = D3DFMT_A8R8G8B8;
	cp.depthFormat = D3DFMT_D24S8;
	for(u32 i = 0; i < 2; i++ )
	{
		blurMap[i] = new RenderTexture();
		if(!blurMap[i]->init(cp))
		{
			gEngine.kernel->log->prnEx(LT_FATAL, "Renderer", "Unable to create blurMap!");
			return 0;
		}
	}
	
	cp.width = gEngine.device->getWidth();
	cp.height = gEngine.device->getHeight();
	cp.colorFormat = D3DFMT_A8R8G8B8;
	cp.depthFormat = D3DFMT_D24S8;
	ssaoMap = new RenderTexture();
	if(!ssaoMap->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Renderer", "Unable to create ssaoMap!");
		return 0;
	}

	m_reflection = new RenderTexture();
	m_reflectionBlur = new RenderTexture();
	cp.width = gEngine.device->getWidth()/4;
	cp.height = gEngine.device->getHeight()/4;
	cp.colorFormat = D3DFMT_A8R8G8B8;
	cp.depthFormat = D3DFMT_D24S8;
	m_reflection->init(cp);
	m_reflectionBlur->init(cp);

	guassianBlur = new GuassianBlur();

	if(r_guassianBlurKernelSize < 1 || r_guassianBlurKernelSize > 15 || (r_guassianBlurKernelSize%2 == 0))
		r_guassianBlurKernelSize = 7;

	guassianBlur->init(r_guassianBlurKernelSize);
	
	rnm = gEngine.resMgr->load<Texture>("noiseNormals.png");

	//Vertex Declarations
	const D3DVERTEXELEMENT9 vd_default[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
		{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};
	gEngine.device->getDev()->CreateVertexDeclaration(vd_default, &m_vd[VD_DEFAULT]);

	const D3DVERTEXELEMENT9 vd_default_instanced[] =
		{
			{ 0, 0,  D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 0, 20, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
			{ 0, 32, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
			{ 0, 44, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
			{ 1, 0,  D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
			D3DDECL_END()
		};
	gEngine.device->getDev()->CreateVertexDeclaration(vd_default_instanced, &m_vd[VD_DEFAULT_INSTANCED]);

	const D3DVERTEXELEMENT9 vd_default_instanced_packed[] =
		{
			{ 0, 0,  D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 0, 20, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
			{ 0, 32, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
			{ 0, 44, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
			{ 1, 0,  D3DDECLTYPE_FLOAT16_4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
			D3DDECL_END()
		};
	gEngine.device->getDev()->CreateVertexDeclaration(vd_default_instanced_packed, &m_vd[VD_DEFAULT_INSTANCED_PACKED]);

	const D3DVERTEXELEMENT9 vd_particle[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 8,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
		{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
		{ 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,	   0 },
		{ 0, 64, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    1 },
		D3DDECL_END()
	};
	gEngine.device->getDev()->CreateVertexDeclaration(vd_particle, &m_vd[VD_PARTICLE]);

	const D3DVERTEXELEMENT9 vd_skydome[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		D3DDECL_END()
	};
	gEngine.device->getDev()->CreateVertexDeclaration(vd_skydome, &m_vd[VD_SKYDOME]);

	
	const D3DVERTEXELEMENT9 vd_terrain[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
		{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};
	gEngine.device->getDev()->CreateVertexDeclaration(vd_terrain, &m_vd[VD_TERRAIN]);

	const D3DVERTEXELEMENT9 vd_vegetation[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	gEngine.device->getDev()->CreateVertexDeclaration(vd_vegetation, &m_vd[VD_VEGETATION]);

	m_initialized = 1;
	return 1;
}

void Renderer::close()
{
	gEngine.resMgr->release(rnm); rnm = 0;

	if(m_heatHaze)
	{
		m_heatHaze->close();
		delete(m_heatHaze);
	}

	if(m_depthOfField)
	{
		m_depthOfField->close();
		delete(m_depthOfField);
	}

	if(m_hdrToneMap)delete(m_hdrToneMap);
	if(m_accumulationRT_HDR)
	{
		m_accumulationRT_HDR->close();
		delete(m_accumulationRT_HDR);
	}

	screenMap->close();
	delete(screenMap);
	blurMap[0]->close();
	delete(blurMap[0]);
	blurMap[1]->close();
	delete(blurMap[1]);
	m_reflectionBlur->close();
	delete(m_reflectionBlur);
	m_reflection->close();
	delete(m_reflection);
	delete(guassianBlur);

	m_mrt->close();
	delete(m_mrt); m_mrt = 0;
	r2d->close();
	delete(r2d); r2d = 0;
	DXRELEASE(pQuery);
	vls->close();
	delete(vls); vls = 0;
	sm->close();
	delete(sm); sm = 0;
	oc->close();
	delete(oc); oc = 0;
}

u32 Renderer::enableHDR()
{ 
	static u32 frameClear = 0;
	if(!m_hdrRendering && gEngine.device->isHDREnabled() && r_hdrRendering && m_accumulationRT_HDR && m_hdrToneMap)
	{
		m_accumulationRT_HDR->enableRendering(0);
		if(frameClear != gEngine.kernel->tmr->getFrameID())
		{
			clear(1,0,0,0x00000000);
			frameClear = gEngine.kernel->tmr->getFrameID();
		}
	}
	else return 0;
	
	m_hdrRendering = 1; 
	return m_hdrRendering;

}
void Renderer::disableHDR()
{ 
	if(m_hdrRendering)
		m_accumulationRT_HDR->disableRendering(); 
	m_hdrRendering = 0;  
}

void Renderer::beginSceneRender()
{
	clear(1,1,1,0x00000000);
	enableHDR();	
}

void Renderer::clear(i32 clearScreen, i32 clearDepth, i32 clearStencil, u32 color)
{
	m_dev->Clear( 0, NULL,(clearScreen ? D3DCLEAR_TARGET : 0) | (clearDepth ? D3DCLEAR_ZBUFFER : 0) | (clearStencil ? D3DCLEAR_STENCIL : 0),	color, 1.0f, 0L );
}

bool Renderer::begin()
{
	state.setFillMode((r_wireframe)?D3DFILL_WIREFRAME:D3DFILL_SOLID);
	m_rend.shader = 0;
	_renderedPrimitivesCount = 0;
	_renderedVerticesCount = 0;
	_renderedDrawIndexedPrimitive = 0;
	_rendererShaderChanges = 0;
	_rendererRSChanges = 0;
	m_hdrRendering = 0;

	return m_dev->BeginScene() == S_OK;
}

void Renderer::end(u32 rendStats, u32 rendGui)
{
	static HRESULT d3dPresentResult = 0;
	//PROFILER(__FUNCTION__);
	//if(1)
	{
		if(rendStats)
		{
				addTxt("Rendered vertices count: %u", _renderedVerticesCount);
				addTxt("Rendered primitives count: %u", _renderedPrimitivesCount);
				addTxt("Rendered draw indexed primitives count: %u", _renderedDrawIndexedPrimitive);
				addTxt("Rendered shader changes: %u", _rendererShaderChanges);
				addTxt("Render states changes: %u", _rendererRSChanges);				

				if(d3dPresentResult == D3DERR_WASSTILLDRAWING)
					addTxt("last frame: D3DERR_WASSTILLDRAWING");

			/*	mapMemUsageIt it;
				for(it = gEngine.kernel->memUsage.begin(); it != gEngine.kernel->memUsage.end(); it++)
					addTxt("MemUsage: %s   : %.3fMB", it->first.c_str(), it->second / 1024.0f / 1024.0f);*/

				/*addTxt("PROFILER:");
				tContainerIterator pit;
				pit = gEngine.kernel->prf->counters.begin();
				while(pit != gEngine.kernel->prf->counters.end())
				{
					gEngine.renderer->addTxt((gEngine.kernel->prf->result(pit->first)).c_str());
					pit++;
				}*/
			stats();
		}
		
		if(rendGui)
		{
			if(gEngine.desktop && gEngine.desktop->m_Visible)
			{
				gEngine.renderer->r2d->begin();
				commitChanges();
				gEngine.desktop->postMessage(WM_PAINT, 0,0,0);
				gEngine.renderer->r2d->end();
			}
		}
		
		m_outstr = "";
		m_outstr2 = "";
	}

	{
		//PROFILER("m_dev->EndScene");
		//gEngine.renderer->setShader(0);
		m_dev->EndScene();
	}

	if(pQuery)
	{
		//PROFILER("pQuery -> End");
		while(S_FALSE == pQuery->GetData( NULL, 0, D3DGETDATA_FLUSH ));
	}
	
	{
		//PROFILER("m_dev->Present");
		d3dPresentResult = m_dev->Present(NULL,NULL,NULL,NULL);
	}

	{
		//PROFILER("computeFps")
		if(pQuery)computeFps();
	}

	if(gEngine.desktop->isFocused() && gEngine.input->isKeyPressed(DIK_E)){ gEngine.renderer->takeScreenshot(); }
}

void Renderer::stats()
{
	static float tpc, tp = 0.0f, fadeIn = 0.001f,fadeOut = 0.00025f, fadeHold = 5.0f;
	r2d->begin();

	if(gEngine.input->isKeyDown(DIK_H) && gEngine.input->isKeyDown(DIK_0))
			r_hidestats = 0;
	else if(gEngine.input->isKeyDown(DIK_H) && gEngine.input->isKeyDown(DIK_1))
		r_hidestats = 1;
	else if(gEngine.input->isKeyDown(DIK_H) && gEngine.input->isKeyDown(DIK_2))
		r_hidestats = 2;
	else if(gEngine.input->isKeyDown(DIK_H) && gEngine.input->isKeyDown(DIK_3))
		r_hidestats = 3;

	if(r_hidestats)
	{
		if(r_hidestats == 2 || r_hidestats == 3 || (gEngine.input->isKeyDown(DIK_H))){ tp += gEngine.kernel->tmr->get()*fadeIn; tp = (tp > fadeHold)?fadeHold:tp; }
		else { tp -= gEngine.kernel->tmr->get()*fadeOut; tp = (tp < 0.0f)?0.0f:tp; }
	}
	else
	{
		tp += gEngine.kernel->tmr->get()*fadeIn;
		tp = (tp > fadeHold)?fadeHold:tp;
	}
	tpc = (tp > 1.0f)?1.0f:tp;

	if(tpc < 0.0f)tpc = 0.0f;
	else if(tpc > 1.0f)tpc = 1.0f;

	D3DXCOLOR c1 = D3DXCOLOR(0,0,0,tpc);
	D3DXCOLOR c2 = D3DXCOLOR(0,0,0,0);
	D3DXCOLOR c3 = D3DXCOLOR(tpc,tpc,tpc,tpc);
	D3DXCOLOR c4;

	if(tpc > 0.0f)
	{
		if(m_fps >= 60)c4 = D3DXCOLOR(0,1,0,tpc);
		else if(m_fps >= 30)c4 = D3DXCOLOR(1,1,0,tpc);
		else c4 = D3DXCOLOR(1,0,0,tpc);

		r2d->drawQuad(0,0,125,32, c1, c2, c1, c2);
		//drawQuad(0,gEngine.device->getHeight()-16,gEngine.device->getWidth(),gEngine.device->getHeight(), c1, c1, c1, c1);
		r2d->drawText(0,1, c4, "FPS: %-8u", m_fps);
		r2d->drawText(0,16, c3, " FT: %-7.3fms",  m_frameTime);
		//this->drawText(0,gEngine.device->getHeight()-15, c3, "Cam pos: x=%.2f y=%.2f z=%.2f, dir: x=%.2f y=%.2f z=%.2f", m_camera->GetPosition()->x, m_camera->GetPosition()->y, m_camera->GetPosition()->z,m_camera->GetViewDirection()->x, m_camera->GetViewDirection()->y, m_camera->GetViewDirection()->z);		
	
		//INFO H+N
		if(r_hidestats && r_hidestats != 2)
		{
			RECT rect;
			rect.left = 0;
			rect.top = 1;
			rect.right = gEngine.device->getWidth();
			rect.bottom = gEngine.desktop->getHeight();
			D3DXCOLOR infoColor = D3DXCOLOR(1,1,1,tpc*0.75f);
			r2d->drawTextSmall( rect, infoColor, DT_RIGHT, "Press H+M to show statistics and settings");	
		}
	}

	if(r_hidestats == 1 && tpc < 1.0f)
	{
		r2d->drawTextCenter(0,1,D3DXCOLOR(0.5,0.7,1.0,1.0-tpc), "Press H to show statistics");
	}

	
	if(!r_hidestats)
	{
		u32 lineCount = 4;
		for(u32 i = 0; i < m_outstr.size(); i++)if(m_outstr[i] == '\n')lineCount++;
		for(u32 i = 0; i < m_outstr2.size(); i++)if(m_outstr2[i] == '\n')lineCount++;
		r2d->drawQuad(0,50,550,(lineCount+2)*10, c1, c2, c1, c2);
		r2d->drawTextSmall(0,50,0xFFFFFFFF,(m_outstr+"\n"+m_outstr2).c_str());

		//FADE STR
		/*for(u32 i = 0; i < m_fadestrNum; i++)
		{
			m_fadetime[i] -= gEngine.kernel->tmr->get() * 0.001f;
			if(m_fadetime[i] < 0.1f)
			{
				for(u32 x = i; x < m_fadestrNum+1; x++)
				{
					m_fadestr[x] = m_fadestr[x+1];
					m_fadetime[x] = m_fadetime[x+1];
				}
				g_fadeYRel += 11.0f;
				m_fadestrNum--;
				i--;
			}
		}

		if(g_fadeYRel>0)g_fadeYRel-=gEngine.kernel->tmr->get() * 0.001f * g_fadeYRel;
		if(g_fadeYRel<0)g_fadeYRel=0;

		for(u32 i = 0; i < m_fadestrNum; i++)
		{
			f32 a;
			f32 max_hold = g_timeFadeMax + g_timeFadeHold;
			if(m_fadetime[i] > max_hold) a = (f32)1.0f - (f32)((f32)m_fadetime[i]-(f32)max_hold)/(f32)g_timeFadeIn;
			else a = ((f32)m_fadetime[i]/g_timeFadeMax);
			r2d->drawTextSmall(gEngine.device->getWidth()/3,20+g_fadeYRel+i*11,D3DXCOLOR(1,0,0,a),m_fadestr[i].c_str());
		}*/
	}
	
	/*if(r_hidestats)
	{
		r2d->drawQuad(gEngine.device->getWidth()-280,-.5,gEngine.device->getWidth(),12, 0xdd000000, 0xdd000000, 0xdd000000, 0xdd000000);
		r2d->drawTextSmall(gEngine.device->getWidth()-275,0,D3DXCOLOR(1,0,0,1),XGINE_VER);
	}*/
	//r2d->end();
}

void Renderer::computeFps()
{
	static u32 fct = 0;
	static f32 cumm = 0;
	static f32 cummX = 300;
	static f32 frameTimeMin, frameTimeMax;
	cumm += gEngine.kernel->tmr->get();
	cummX += gEngine.kernel->tmr->get();
	fct++;
	
	if(cumm > 1000.0f)
	{
		m_frameTime = (f32)(cumm / fct);
		m_fps = (u32)(fct * 1000.0f / cumm);
		cumm = 0;
		fct = 1;
	}

	if(cummX > 500.0f)
	{
		m_frameTimeMin = frameTimeMin;
		m_frameTimeMax = frameTimeMax;
		m_fpsMin = 1000.0f/frameTimeMax;
		m_fpsMax = 1000.0f/frameTimeMin;
		frameTimeMin = 9999999;
		frameTimeMax = -9999999;
		cummX = 0;
	}

	m_frameTime = gEngine.kernel->tmr->get();

	if(m_frameTime > frameTimeMax)frameTimeMax = m_frameTime;
	if(m_frameTime < frameTimeMin)frameTimeMin = m_frameTime;
}

void Renderer::drawBoundingBox(BoundingBox* boundingBox,Vec4 color)
{
	static struct A { D3DXVECTOR3 pos; } vertsbb[28];
	static Vec3 pbb[28];
	static u32 i;

	setShader(m_zfillShader->setTech("BB"));
	Mat idd;
	D3DXMatrixIdentity(&idd);
	setWorld(&idd);

	buildBoundingBoxLines(&boundingBox->CurrentMin, &boundingBox->CurrentMax, pbb);
	for(i = 0; i < 28; i++)
	{
		vertsbb[i].pos = pbb[i];
	}

	m_zfillShader->setVec4("color", &color);
	commitChanges();
	m_dev->SetFVF(D3DFVF_XYZ);
	m_dev->DrawPrimitiveUP(D3DPT_LINELIST, 14, vertsbb, sizeof(A));
}

void Renderer::setCamera(ICamera* cam)
{
	m_rend.camera = cam;
}

void Renderer::setLight(Light* light)
{
	m_rend.light = light;
}

void Renderer::setupLight(Light* light)
{
	m_rend.light = light;

	if(m_rend.shader && m_rend.light)
	{
		switch(m_rend.light->getType())
		{
			case LIGHT_POINT:
				{
					/*m_rend.shader->setTech("PointLighting");*/
					m_rend.shader->setVec4("g_lightPos",		m_rend.light->getPosV4());
					m_rend.shader->setF32("g_lightInvRange",	m_rend.light->getInvRange());
					m_rend.shader->setVec4("g_lightColor",		m_rend.light->getColor());
				}break;
			case LIGHT_SPOT:
				{
					m_rend.shader->setVec4("g_lightColor",		m_rend.light->getColor());
					m_rend.shader->setVec4("g_lightPos",		m_rend.light->getPosV4());
					m_rend.shader->setVec3("g_lightDir",		m_rend.light->getDir());
					m_rend.shader->setF32("g_lightCosPhiHalf",	m_rend.light->getCosPhiHalf());
					m_rend.shader->setF32("g_lightInvRange",	m_rend.light->getInvRange());
				}break;
			case LIGHT_DIR:
				{
					m_rend.shader->setVec3("g_lightDir",		m_rend.light->getDir());
					m_rend.shader->setVec4("g_lightColor",		m_rend.light->getColor());

					if(m_rend.type == REND_FORWARD || m_rend.pass == REND_REFLECT)
					{
						m_rend.shader->setVec4("g_fogParams",	&r_fogParams);
						m_rend.shader->setVec3("g_fogColor",	&r_fogColor);
						m_rend.shader->setF32("g_ambientColor", m_ambientColor);
					}
				}break;
		}
	}
}

void Renderer::setShader(Shader* shader)
{
	if(m_rend.shader != shader)
	{
		if(m_rend.shader)
			m_rend.shader->end();

		m_rend.shader = shader; 

		if(m_rend.shader)
		{
			m_rend.shader->begin();
			_rendererShaderChanges++;
			//_addTxt("Shader changed to %s", m_rend.shader->getFileName().c_str());

		}
	}
}

void Renderer::setWorld(Mat* world)
{	
	m_rend.matWorld = *world;
	if(m_rend.shader && m_rend.camera)
	{
		if(m_rend.pass == REND_SM && m_rend.light)
		{
			Mat lightViewProj = m_rend.matWorld * (*m_rend.light->getViewProjMat());
			m_rend.shader->setMat("g_matWorldViewProj", &lightViewProj);
			Mat lightView = m_rend.matWorld * (*m_rend.light->getViewMat());
			m_rend.shader->setMat("g_matViewProj", m_rend.light->getViewProjMat());
			//m_rend.shader->setVec4("g_lightPos",	m_rend.light->getPosV4());
		}
		else if(m_rend.pass == REND_REFLECT)
		{
			Plane reflectPlane;
			Vec3 point = Vec3( 0 , m_reflectionHeight, 0 );
			Vec3 normal( 0, 1, 0 );
			D3DXPlaneFromPointNormal(&reflectPlane,&point,&normal);
			D3DXPlaneNormalize(&reflectPlane,&reflectPlane);

			Plane clipPlane;
			Vec3 pointClip = Vec3( 0 , m_reflectionHeight, 0 );
			Vec3 normalClip( 0, -1, 0 );
			D3DXPlaneFromPointNormal(&clipPlane,&pointClip,&normalClip);
			D3DXPlaneNormalize(&clipPlane,&clipPlane);
			D3DXMATRIXA16 matrix = (*m_rend.camera->getViewProjectionMat());
			D3DXMatrixInverse(&matrix, NULL, &matrix);
			D3DXMatrixTranspose(&matrix, &matrix);
			D3DXPLANE viewSpacePlane;
			D3DXPlaneTransform(&viewSpacePlane, &clipPlane, &matrix);
			m_dev->SetClipPlane( 0, viewSpacePlane);
			state.setClipPlaneEnable(TRUE);
			
			Mat reflect;
			D3DXMatrixReflect(&reflect,&reflectPlane);
			Vec3 vpos = *m_rend.camera->getPosition();
			vpos.y = m_reflectionHeight - (vpos.y - m_reflectionHeight);

			Mat worldViewProj = m_rend.matWorld * reflect * (*m_rend.camera->getViewProjectionMat());
			m_rend.shader->setMat("g_matWorldViewProj", &worldViewProj);
			m_rend.shader->setMat("g_matViewProj", m_rend.camera->getViewProjectionMat());
			m_rend.shader->setMat("g_matWorld", &m_rend.matWorld);
			m_rend.shader->setVec3("g_viewPos", &vpos);
		}
		/*else if(m_rend.pass == REND_LIGHTVOLUMES)
		{
			m_rend.matWorldViewProj = m_rend.matWorld * (*m_rend.camera->getViewProjectionMat());
			m_rend.shader->setMat("g_matWorldViewProj", &m_rend.matWorldViewProj);
		}*/
		else
		{
			m_rend.matWorldViewProj = m_rend.matWorld * (*m_rend.camera->getViewProjectionMat());
			m_rend.shader->setMat("g_matWorldViewProj", &m_rend.matWorldViewProj);
			m_rend.shader->setMat("g_matViewProj", m_rend.camera->getViewProjectionMat());
			m_rend.shader->setMat("g_matWorld", &m_rend.matWorld);
			m_rend.shader->setVec3("g_viewPos", m_rend.camera->getPosition());
		}
	}
	else if(m_rend.camera)
	{
		m_dev->SetTransform(D3DTS_VIEW,			m_rend.camera->getViewMat());
		m_dev->SetTransform(D3DTS_PROJECTION,	m_rend.camera->getProjectionMat());
		m_dev->SetTransform(D3DTS_WORLD,		&m_rend.matWorld);
	}
}

void Renderer::setMaterial(Material* mat)
{
	if(m_rend.pass != REND_ZFILL)
	{
		if(m_rend.material != mat)
		{
			m_rend.material = mat;
			if(m_rend.shader)
			{
				if(mat->m_splatting)
				{
					m_rend.shader->setTex("IndexMap",				mat->m_indexMap);
					m_rend.shader->setTex("ColorMap1",				mat->m_colorMapSplatting[0]);
					m_rend.shader->setTex("ColorMap2",				mat->m_colorMapSplatting[1]);
					m_rend.shader->setTex("ColorMap3",				mat->m_colorMapSplatting[2]);
					m_rend.shader->setTex("ColorMap4",				mat->m_colorMapSplatting[3]);
					if(mat->m_useNormalMap)
					{
						m_rend.shader->setTex("NormalMap1",			mat->m_normalMapSplatting[0]);
						m_rend.shader->setTex("NormalMap2",			mat->m_normalMapSplatting[1]);
						m_rend.shader->setTex("NormalMap3",			mat->m_normalMapSplatting[2]);
						m_rend.shader->setTex("NormalMap4",			mat->m_normalMapSplatting[3]);
					}
				}
				else
				{
					m_rend.shader->setTex("ColorMap",				mat->m_colorMap);
					
					if(mat->m_useNormalMap)
					{
						m_rend.shader->setTex("NormalMap",			mat->m_normalMap);
						if(mat->m_useNormalMap == DT_PARALLAX_OCCLUSION_MAPPING)
						 m_rend.shader->setF32("g_normalMapSize",	(f32)mat->m_normalMap->width);
					}
				}

				m_rend.shader->setF32("g_textureRepeat",		mat->m_textureRepeat);

				if(mat->m_useAlbedoMap)
				{
					m_rend.shader->setTex("AlbedoMap",					mat->m_albedoMap);
					m_rend.shader->setF32("g_textureAlbedoRepeat",		mat->m_textureAlbedoRepeat);
				}

				if(mat->m_useCubeMap)
					m_rend.shader->setTex("CubeMap",			mat->m_cubeMap);
				if(mat->m_useSpecular)
					m_rend.shader->setF32("g_specularExponent",	mat->m_specularExponent);
				if(mat->m_useDiffuseColor)
					m_rend.shader->setVec4("g_diffuseColor",	&mat->m_diffuseColor);

				if(mat->m_useAnimation)
				{
					m_rend.shader->setF32("g_time",				gEngine.kernel->tmr->getFrameTime());		
				}
			}

			if(mat->m_useFlipBackfaceNormals)
				state.setCullMode(D3DCULL_NONE);
			else if(m_rend.pass == REND_REFLECT)
				state.setCullMode(D3DCULL_CCW);
			else
				state.setCullMode(D3DCULL_CW);
		}
	}
	else if(m_rend.type == REND_LIGHTPREPASS)
	{
		if(m_rend.material != mat)
		{
			m_rend.material = mat;
			if(m_rend.shader)
			{
				if(mat->m_splatting)
				{
					m_rend.shader->setTex("IndexMap",				mat->m_indexMap);
					if(mat->m_useNormalMap)
					{
						m_rend.shader->setTex("NormalMap1",			mat->m_normalMapSplatting[0]);
						m_rend.shader->setTex("NormalMap2",			mat->m_normalMapSplatting[1]);
						m_rend.shader->setTex("NormalMap3",			mat->m_normalMapSplatting[2]);
						m_rend.shader->setTex("NormalMap4",			mat->m_normalMapSplatting[3]);
					}
				}
				else
				{					
					if(mat->m_useNormalMap)
					{
						m_rend.shader->setTex("NormalMap",			mat->m_normalMap);
						if(mat->m_useNormalMap == DT_PARALLAX_OCCLUSION_MAPPING)
						 m_rend.shader->setF32("g_normalMapSize",	(f32)mat->m_normalMap->width);
					}
				}

				m_rend.shader->setF32("g_textureRepeat",		mat->m_textureRepeat);

				if(mat->m_useAnimation)
				{
					m_rend.shader->setF32("g_time",				gEngine.kernel->tmr->getFrameTime());		
				}
			}

			if(mat->m_useFlipBackfaceNormals)
				state.setCullMode(D3DCULL_NONE);
			else if(m_rend.pass == REND_REFLECT)
				state.setCullMode(D3DCULL_CCW);
			else
				state.setCullMode(D3DCULL_CW);
		}
	}
	else if(mat->m_useAlphaTest)
	{
		if(m_rend.material != mat)
		{
			m_rend.material = mat;
			if(m_rend.shader)
			{
				m_rend.shader->setTex("ColorMap",				mat->m_colorMap);
				m_rend.shader->setF32("g_textureRepeat",		mat->m_textureRepeat);

				if(mat->m_useAnimation)
					m_rend.shader->setF32("g_time",				gEngine.kernel->tmr->getFrameTime());
			}

			if(mat->m_useFlipBackfaceNormals)
				state.setCullMode(D3DCULL_NONE);
			else if(m_rend.pass == REND_REFLECT)
				state.setCullMode(D3DCULL_CCW);
			else
				state.setCullMode(D3DCULL_CW);
		}
	}
}

void Renderer::setTexture(u32 sampler, Texture* textureRes)
{
	IDirect3DBaseTexture9* texture = (textureRes)?(textureRes->get()):(0);
	if(m_textures[sampler] != texture)
	{
		m_textures[sampler] = texture;
		m_dev->SetTexture(sampler, texture);
	}
}

void Renderer::setTexture(u32 sampler, IDirect3DBaseTexture9* texture)
{
	if(m_textures[sampler] != texture)
	{
		m_textures[sampler] = texture;
		m_dev->SetTexture(sampler, texture);
	}
}

void Renderer::commitChanges()
{
	_rendererRSChanges += state.commitChanges(m_dev);

	if(m_rend.shader)
		m_rend.shader->commitChanges();
}

void Renderer::cull(RenderList *in, CULL_TYPE cullType)
{
	f32 startTime = gEngine.kernel->tmr->getCurrent();

	in->clearCulledLights();
	
	if(r_cullingLights)
	{
		if(in->m_sunLight)
			in->culledLights.push_back(in->m_sunLight);

		const Plane* frustumPlanes = m_rend.camera->getFrustumPlanes();
		const BoundingSphere* frustumSphere = m_rend.camera->getFrustumSphere();
		for(u32 i = 0; i < in->lights.size(); i++)
		{
			if(in->lights[i]->getEnabled())
			{
				if(in->lights[i]->getType() == LIGHT_POINT)
				{
					if(in->lights[i]->getBoundingBox()->BSphere.intersect(frustumSphere))
						if(in->lights[i]->getBoundingBox()->BSphere.intersect(frustumPlanes))
							in->culledLights.push_back(in->lights[i]);
				}
				else if(in->lights[i]->getType() == LIGHT_SPOT)
				{
					if(in->lights[i]->getBoundingCone()->intersect(frustumSphere))
						in->culledLights.push_back(in->lights[i]);
				}
				else in->culledLights.push_back(in->lights[i]);
			}
		}
	}
	else
	{
		in->culledLights = in->lights;	
		in->culledLights.push_back(in->m_sunLight);
	}

	in->clearCulled();

	for(u32 i = 0; i < in->culledLights.size(); i++)
		if(in->culledLights[i]->castsShadows() && r_enabledShadows && r_enabledShadowMapping && r_enabledShadowMappingDeferred)
			in->culledLights[i]->renderDepth(in);

	in->clearCulled();

	if(r_cullingFrustum)
	{
		//surface
		FrustumCulling::check(m_rend.camera, in->surfaceVector, &in->culledSurfaces);
		in->surfaceOctree->cull(m_rend.camera, &in->culledSurfaces);	
		//entities
		FrustumCulling::check(m_rend.camera, in->entityVector, &in->culledEntities);
		in->entityOctree->cull(m_rend.camera, &in->culledEntities);
		//terrains
		in->terrainOctree->cull(m_rend.camera, &in->culledTerrains);		
	}
	else
	{
		//in->culledSolid.add(in->solid);
		//in->culledTrans.add(in->trans);
	}

	if(r_cullingOcclusion)
	{
		oc->check(in);
	}

	f32 endTime = gEngine.kernel->tmr->getCurrent();

	addTxt("Culling : %fms\n [count: %u surfaces, %u terrains, %u entities, %u lights]", endTime-startTime, in->culledSurfaces.size(),in->culledTerrains.size(), in->culledEntities.size(), in->culledLights.size());
}

void Renderer::render(RenderList *in)
{
	if(r_enabledDeferred)
		m_rend.type = REND_DEFERRED;
	else if(r_enabledLightPrePass)
		m_rend.type = REND_LIGHTPREPASS;
	else
		m_rend.type = REND_FORWARD;

	if(r_forceForward)
		m_rend.type = REND_FORWARD;

	if(m_rend.type == REND_LIGHTPREPASS)
	{
		renderLightPrePass(in);
		renderForwardTrans(in);

		prepareBackBuffers();
	}
	else if(m_rend.type == REND_DEFERRED)
	{
		renderDeferred(in);
		renderForwardTrans(in);	

		enableHDR();

		if(r_enabledVLS && !r_enabledHDRVLS)
		{
			vls->draw();
		}

		disableHDR();		

		if(!m_hdrRendering && gEngine.device->isHDREnabled() && r_hdrRendering && m_accumulationRT_HDR && m_hdrToneMap)
		{
			Vec3 pos = *m_rend.camera->getPosition() - Vec3(*in->m_sunLight->getDir()) * 900.0f;
			m_hdrToneMap->setSunPos( pos );
			m_hdrToneMap->process(m_accumulationRT_HDR);
		}

		prepareBackBuffers();

		if(r_dofEnabled && m_depthOfField)
			m_depthOfField->process();

		if(r_heatHazeEnabled)
		{
			prepareBackBuffers();
			m_heatHaze->process();
		}

		if(gEngine.input->isKeyDown(DIK_Y))
		{
			state.setAlphaBlendEnable(FALSE);
			setShader(m_deferredShaderLight->setTech("EdgeDetection"));
			commitChanges();
			r2d->drawScreenQuadRaw();
			state.setAlphaBlendEnable(TRUE);
			return;
		}
	}
	else if(m_rend.type == REND_FORWARD)
	{
		renderForward(in);
		renderForwardTrans(in);

		disableHDR();		

		if(!m_hdrRendering && gEngine.device->isHDREnabled() && r_hdrRendering && m_accumulationRT_HDR && m_hdrToneMap)
		{
			Vec3 pos = *m_rend.camera->getPosition() - Vec3(*in->m_sunLight->getDir()) * 900.0f;
			m_hdrToneMap->setSunPos( pos );
			m_hdrToneMap->process(m_accumulationRT_HDR);
		}

		prepareBackBuffers();
	}
	else
		addTxt("RenderList has not been rendered. Invalid render type (Deferred/Forward)");
}

void Renderer::renderReflection(RenderList *in)
{
	addTxt("Rend type: REFLECTION");
	
	state.setZEnable(TRUE);
	state.setZWriteEnable(TRUE);
	state.setZFunc(D3DCMP_LESSEQUAL);
	state.setColorWriteEnable(0x0000000f);
	state.setStencilEnable(FALSE);
	state.setAlphaBlendEnable(FALSE);
	state.setMultiSampleAntialias(FALSE);

	m_rend.pass = REND_REFLECT;

	if(r_enabledTerrainReflection)renderListForward(&in->culledTerrains, in->m_sunLight);
	if(r_enabledSurfacesReflection)renderListForward(&in->culledSurfaces, in->m_sunLight);
	if(r_enabledEntitiesReflection)renderListForward(&in->culledEntities, in->m_sunLight);

	////in->renderRange2(25, 200, in->m_sunLight);
	//m_rend.pass = REND_SOLID;
}

void Renderer::renderForward(RenderList *in)
{
	if(!m_forwardShader)return;
	addTxt("Rend type: Forward");

	state.setZEnable(TRUE);
	state.setZWriteEnable(TRUE);
	state.setZFunc(D3DCMP_LESSEQUAL);
	state.setColorWriteEnable(0x0000000f);
	state.setStencilEnable(FALSE);
	state.setAlphaBlendEnable(FALSE);
	state.setMultiSampleAntialias(FALSE);

	if(r_multisample) state.setMultiSampleAntialias(TRUE);
	else state.setMultiSampleAntialias(FALSE);

	m_rend.pass = REND_SOLID;
	m_defaultShader = m_forwardShader;
	if(r_renderTerrain)renderListForward(&in->culledTerrains, in->m_sunLight);
	if(r_renderSurfaces)renderListForward(&in->culledSurfaces, in->m_sunLight);
	if(r_renderEntities)renderListForward(&in->culledEntities, in->m_sunLight);
}

void Renderer::renderForwardTrans(RenderList *in)
{
	if(!m_forwardShader)return;
	addTxt("Rend type: Forward Trans");

	state.setZEnable(TRUE);
	state.setZWriteEnable(TRUE);
	state.setZFunc(D3DCMP_LESSEQUAL);
	state.setColorWriteEnable(0x0000000f);
	state.setStencilEnable(FALSE);
	state.setAlphaBlendEnable(FALSE);
	state.setShadeMode(D3DSHADE_GOURAUD);
	state.setLighting(FALSE);

	if(r_multisample) state.setMultiSampleAntialias(TRUE);
	else state.setMultiSampleAntialias(FALSE);

	m_rend.pass = REND_TRANS;
	m_defaultShader = m_forwardShader;
	if(r_waterRenderingEnabled)
	{
		if(r_highQualityWaterEnabled && in->hqWater)
			in->hqWater->render();
		else if(in->water)
		{
			in->water->render();
		}
	}
}

void Renderer::prepareBackBuffers()
{
	copyBackBuffer(m_backBuffer->getColorTexture());
	this->guassianBlur->process(m_backBuffer, m_backBufferBluredTemp, m_backBufferBlured);
}

RenderTexture* Renderer::getBackBuffer(u32 blur)
{
	if(blur)
	{
		return m_backBufferBlured;
	}
	else
	{
		return m_backBuffer;
	}
}

void Renderer::copyBackBuffer(LPDIRECT3DTEXTURE9 dest)
{
	LPDIRECT3DSURFACE9 backBuffSurf = NULL;
	LPDIRECT3DSURFACE9 destSurf = NULL;
	m_dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffSurf);
	dest->GetSurfaceLevel(0, &destSurf);
	m_dev->StretchRect(backBuffSurf, 0, destSurf, 0, D3DTEXF_NONE);
	backBuffSurf->Release();
	destSurf->Release();
}

void Renderer::setRenderTarget(LPDIRECT3DSURFACE9 rt)
{
	if(!rt) m_dev->SetRenderTarget(0, m_screenRT);
	else m_dev->SetRenderTarget(0, rt);
}

// UTITILIES
void Renderer::addTxt(const c8 *str, ...)
{
	if(r_hidestats)return;
	static c8 szBuf[10000];
	va_list args; va_start(args,str);
	vsprintf_s(szBuf,10000,str,args);
	m_outstr += szBuf;
	m_outstr += "\n";
}

void Renderer::_addTxt(const c8 *str, ...)
{
	if(r_hidestats)return;
	static c8 szBuf[10000];
	va_list args; va_start(args,str);
	vsprintf_s(szBuf,10000,str,args);
	m_outstr2 += szBuf;
	m_outstr2 += "\n";
}


void Renderer::addTxtF(const c8 *str, ...)
{
	if(r_hidestats)return;
	static c8 szBuf[10000];
	va_list args; va_start(args,str);
	vsprintf_s(szBuf,10000,str,args);
	string sstr = szBuf;
	m_fadestr[m_fadestrNum] = sstr;
	m_fadetime[m_fadestrNum] = g_timeFadeMax + g_timeFadeHold + g_timeFadeIn;
	m_fadestrNum++;
}

void Renderer::takeScreenshot()
{
	LPDIRECT3DSURFACE9 backBuff;
	char filename[64];
	for(unsigned int i=0;i<4000000000;i++)
	{
		sprintf_s(filename, 64, "screenshot%.3u.png", i);
		FILE* f = fopen(filename, "r");
		if(f == NULL) break;
		else fclose(f);
	}
	
	m_dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuff);
	D3DXSaveSurfaceToFile(filename, D3DXIFF_PNG, backBuff, NULL, NULL);
	backBuff->Release();
	addTxtF("Screenshot saved to: '%s'", filename);
}

void Renderer::hideStats(i32 hide)
{
	r_hidestats = (hide)?1:0;
}

void Renderer::renderDeferred(RenderList *in)
{
	if(!m_deferredShaderFill || !m_deferredShaderLight || !m_mrt->isInitialized())
		return;
	
	addTxt("Rend type: Deferred");

	if(r_enabledZPrePass && 0)
	{
		////Z-Pre Pass
		state.setZEnable(TRUE);
		state.setZWriteEnable(TRUE);
		state.setZFunc(D3DCMP_LESSEQUAL);
		state.setColorWriteEnable(0x0000000f);
		state.setStencilEnable(FALSE);
		state.setAlphaBlendEnable(FALSE);
		state.setShadeMode(D3DSHADE_GOURAUD);
		state.setLighting(FALSE);
		state.setCullMode(D3DCULL_CW);
		state.setMultiSampleAntialias(FALSE);
		m_rend.pass = REND_ZFILL;
			if(r_renderTerrain)renderListRaw(&in->culledTerrains);
			if(r_renderSurfaces)renderListRaw(&in->culledSurfaces);
			if(r_renderEntities) renderListRaw(&in->culledEntities);
		//G-Buffer Fill
		disableHDR();
		m_mrt->begin(0);
		clear(1,0,0,0);
		state.setZWriteEnable(FALSE);
		state.setMultiSampleAntialias(FALSE);
		m_rend.pass = REND_SOLID;
		 if(r_renderTerrain)renderListDeferred(&in->culledTerrains);
		 if(r_renderSurfaces)renderListDeferred(&in->culledSurfaces);
		 if(r_renderEntities)renderListDeferred(&in->culledEntities);
		m_mrt->end();
		enableHDR();
	}
	else
	{
		//G-Buffer Fill + Z test
		disableHDR();
		m_mrt->begin(0);
		clear(1,0,0,0);
		state.setZEnable(TRUE);
		state.setZWriteEnable(TRUE);
		state.setZFunc(D3DCMP_LESSEQUAL);
		state.setColorWriteEnable(0x0000000f);
		state.setStencilEnable(FALSE);
		state.setAlphaBlendEnable(FALSE);
		state.setShadeMode(D3DSHADE_GOURAUD);
		state.setLighting(FALSE);
		state.setCullMode(D3DCULL_CW);
		state.setMultiSampleAntialias(FALSE);
		m_rend.pass = REND_SOLID;
		 if(r_renderTerrain)renderListDeferred(&in->culledTerrains);
		 if(r_renderSurfaces)renderListDeferred(&in->culledSurfaces);
		 if(r_renderEntities)renderListDeferred(&in->culledEntities);
		m_mrt->end();
		enableHDR();
	}

	//Lighting
	renderDeferredLighting(in);

	static u32 mrtRT = 3;
	if(gEngine.input->isKeyDown(DIK_RSHIFT)){ if(mrtRT>2)mrtRT = 0; else mrtRT++; Sleep(500); }
	if(mrtRT >= 0 && mrtRT < 3)
	{
		setShader(0);
		clear(1,0,0,0);
		state.setAlphaBlendEnable(FALSE);
		r2d->drawScreenQuad(m_mrt->getRT(mrtRT));
	}
}

inline void computeScissorRect(BoundingBox *bb, RECT &rect)
{
	static Mat				world;
	static D3DVIEWPORT9	viewport;
	static Vec3			points[8];
	static Vec3			pPoints[8];
	static Vec3			A, B;
	static RECT			tempRect;
	static Mat				*proj, *view;
	D3DXMatrixTranslation(&world, 0, 0, 0);
	gEngine.device->getDev()->GetViewport(&viewport);
	A = bb->CurrentMin; B = bb->CurrentMax;
	proj = gEngine.renderer->get().camera->getProjectionMat();
	view = gEngine.renderer->get().camera->getViewMat();
	points[0] = A;
	points[1] = Vec3(A.x, A.y, B.z);
	points[2] = Vec3(B.x, A.y, A.z);
	points[3] = Vec3(B.x, A.y, B.z);
	points[4] = Vec3(A.x, B.y, B.z);
	points[5] = Vec3(B.x, B.y, A.z);
	points[6] = Vec3(B.x, B.y, A.z);
	points[7] = B;
	D3DXVec3ProjectArray(pPoints, sizeof(Vec3), points, sizeof(Vec3), &viewport, proj, view, &world, 8);
	//D3DXVec3Project(&pPoints[0], &points[0], &viewport, proj, view, &world);
	tempRect.top	= tempRect.bottom	= pPoints[0].y;
	tempRect.left	= tempRect.right	= pPoints[0].x;
	for(u32 i = 1; i < 8; i++)
	{
		//D3DXVec3Project(&pPoints[i], &points[i], &viewport, proj, view, &world);
		if(pPoints[i].y < tempRect.top)		tempRect.top = pPoints[i].y;
		if(pPoints[i].x < tempRect.left)	tempRect.left = pPoints[i].x;
		if(pPoints[i].y > tempRect.bottom)	tempRect.bottom = pPoints[i].y;
		if(pPoints[i].x > tempRect.right)	tempRect.right = pPoints[i].x;
	}

	u32 w = gEngine.device->getWidth();
	u32 h = gEngine.device->getHeight();

	if(tempRect.top < 0)tempRect.top = 0;
	if(tempRect.bottom < 0)tempRect.bottom = 0;
	if(tempRect.top > h)tempRect.top = h;
	if(tempRect.bottom < h)tempRect.bottom = h;
	if(tempRect.top > tempRect.bottom)tempRect.top = tempRect.bottom;
	if(tempRect.left < 0)tempRect.left = 0;
	if(tempRect.right < 0)tempRect.right = 0;
	if(tempRect.left > w)tempRect.left = w;
	if(tempRect.right < w)tempRect.right = w;
	if(tempRect.left > tempRect.right)tempRect.left = tempRect.right;

	rect = tempRect;
}

inline void determineClipRect(BoundingBox *bb, RECT &rect)
{
	//compute 3D bounding box of light in world space
	static Vec3 bbox3D[8];
	static Vec3 A, B;
	static Vec2 projBox[8];
	static Vec4 projPoint;
	A = bb->CurrentMin; B = bb->CurrentMax;
	bbox3D[0] = A;
	bbox3D[1] = Vec3(A.x, A.y, B.z);
	bbox3D[2] = Vec3(B.x, A.y, A.z);
	bbox3D[3] = Vec3(B.x, A.y, B.z);
	bbox3D[4] = Vec3(A.x, B.y, B.z);
	bbox3D[5] = Vec3(B.x, B.y, A.z);
	bbox3D[6] = Vec3(B.x, B.y, A.z);
	bbox3D[7] = B;

	//project coordinates
	D3DXMATRIX viewProjMat = (*gEngine.renderer->get().camera->getViewMat()) * (*gEngine.renderer->get().camera->getProjectionMat());
	

	u32 w = gEngine.device->getWidth();
	u32 h = gEngine.device->getHeight();
	for (int i = 0; i < 8; ++i)
	{
		D3DXVec3Transform(&projPoint, &bbox3D[i], &viewProjMat);
		projBox[i].x = projPoint.x / projPoint.w;  
		projBox[i].y = projPoint.y / projPoint.w;

		//clip to extents
		if (projBox[i].x < -1.0f)
			projBox[i].x = -1.0f;
		else if (projBox[i].x > 1.0f)
			projBox[i].x = 1.0f;
		if (projBox[i].y < -1.0f)
			projBox[i].y = -1.0f;
		else if (projBox[i].y > 1.0f)
			projBox[i].y = 1.0f;

		//go to pixel coordinates
		projBox[i].x = ((projBox[i].x + 1.0f) / 2.0f) * w;
		projBox[i].y = ((-projBox[i].y + 1.0f) / 2.0f) * h;
	}

	//compute 2D bounding box of projected coordinates
	u32 minX = 0xFFFFFFFF;
	u32 maxX = 0x00000000;
	u32 minY = 0xFFFFFFFF;
	u32 maxY = 0x00000000;
	for (int i = 0; i < 8; ++i)
	{
		u32 x = static_cast<unsigned int>(projBox[i].x);
		u32 y = static_cast<unsigned int>(projBox[i].y);
		if (x < minX)
			minX = x;
		if (x > maxX)
			maxX = x;
		if (y < minY)
			minY = y;
		if (y > maxY)
			maxY = y;
	}

	rect.top    = minY; if(rect.top < 0)rect.top = 0;
	rect.bottom = maxY; if(rect.bottom > h)rect.bottom = h; if(rect.bottom < rect.top)rect.bottom = rect.top + 1;
	rect.left   = minX; if(rect.left < 0)rect.left = 0;
	rect.right  = maxX; if(rect.right > w)rect.right = w; if(rect.right < rect.left)rect.right = rect.left + 1;
}

void Renderer::renderDeferredLighting(RenderList *in)
{
	//LIGHT RENDERING
	m_rend.pass = REND_SOLID;

	m_defaultShader = m_deferredShaderLight;
	
	state.setZEnable(FALSE);
	state.setZWriteEnable(FALSE);
	state.setColorWriteEnable(0x0000000f);
	state.setStencilEnable(FALSE);
	state.setAlphaBlendEnable(FALSE);
	state.setShadeMode(D3DSHADE_GOURAUD);
	state.setLighting(FALSE);
	state.setCullMode(D3DCULL_NONE);
	state.setMultiSampleAntialias(FALSE);
	state.setAlphaBlendEnable(TRUE);
	state.setBlendOp(D3DBLENDOP_ADD);
	state.setSrcBlend(D3DBLEND_ONE);
	state.setDestBlend(D3DBLEND_ONE);

	Mat g_matInvViewProj;
	D3DXMatrixInverse(&g_matInvViewProj, 0, m_rend.camera->getViewProjectionMat());
	m_deferredShaderLight->setTex("MRT0", m_mrt->getRT(0));
	m_deferredShaderLight->setTex("MRT1", m_mrt->getRT(1));
	m_deferredShaderLight->setTex("MRT2", m_mrt->getRT(2));
	m_deferredShaderLight->setTex("RandomMap", rnm);
	m_deferredShaderLight->setVec2("screensize", &Vec2( gEngine.device->getWidth(), gEngine.device->getHeight()));
	m_deferredShaderLight->setVec2("half_texel", &Vec2(0.5f/gEngine.device->getWidth(), 0.5f/gEngine.device->getHeight()));
	m_deferredShaderLight->setMat("g_matInvViewProj", &g_matInvViewProj);
	m_deferredShaderLight->setVec4("g_viewPos", m_rend.camera->getPositionV4());

	BoundingSphere camBSphere(*m_rend.camera->getPosition(), 1.0f);
	
	if(r_enabledVLS || r_enabledSSAO)
	{
		disableHDR();

		if(r_enabledVLS)
		{
			vls->getTexture()->enableRendering(0);
			state.setAlphaBlendEnable(FALSE);
			setShader(m_deferredShaderLight->setTech("AmbientLighting"));
			commitChanges();
			r2d->drawScreenQuadRaw();
			vls->getTexture()->disableRendering();

			if(!r_enabledHDRVLS)
			{
				Vec3 pos = *m_rend.camera->getPosition() - Vec3(*in->m_sunLight->getDir()) * 900.0f;
				vls->setSunPos( pos );
				vls->prepare();
			}
		}

		if(r_enabledSSAO)
		{
			ssaoMap->enableRendering();
			clear(1,0,0);
			setShader(m_deferredShaderLight->setTech("SSAO"));
			m_deferredShaderLight->setMat("g_matViewProjection",	m_rend.camera->getViewProjectionMat());
			m_deferredShaderLight->setMat("g_matInvViewProj",		&g_matInvViewProj);
			m_deferredShaderLight->setTex("DEPTHMAP",				m_mrt->getRT(2));
			m_deferredShaderLight->setTex("NORMALMAP",				m_mrt->getRT(1));
			m_deferredShaderLight->setF32("radius",					r_ssaoRadius);
			m_deferredShaderLight->setF32("fresZsize",				r_ssaoFresZSize);
			m_deferredShaderLight->setF32("aoCap",					r_ssaoCap);
			m_deferredShaderLight->setF32("aoMultiplier",			r_ssaoMultiplier);
			m_deferredShaderLight->setF32("depthTolerance",			r_ssaoDepthTolerance);
			commitChanges();
			r2d->drawScreenQuadRaw();
			ssaoMap->disableRendering();
			//guassianBlur->process(ssaoMap, blurMap[0], ssaoMap);
			enableHDR();
			
			if(gEngine.input->isKeyDown(DIK_M))
			{
				setShader(0);
				clear(1,0,0,0);
				state.setAlphaBlendEnable(FALSE);
				commitChanges();
				r2d->drawScreenQuad(ssaoMap->getColorTexture());
				return;
			}
		}

		enableHDR();
	}

	if(r_enabledSSAO)
	{
		m_deferredShaderLight->setF32("g_ambientColor", 0);
		m_deferredShaderLight->setBool("bSSAO_On", true);
		m_deferredShaderLight->setTex("SSAOMAP", ssaoMap->getColorTexture());
	}
	else
	{
		m_deferredShaderLight->setF32("g_ambientColor", m_ambientColor);
		m_deferredShaderLight->setBool("bSSAO_On", false);
	}

	commitChanges();

	if(!r_enabledZPrePass)
	{
		state.setAlphaBlendEnable(FALSE);
		setShader(m_deferredShaderLight->setTech("AmbientLighting"));
		commitChanges();
		r2d->drawScreenQuadRaw();
		state.setAlphaBlendEnable(TRUE);
	}

	Vec3 cam_pos = *m_rend.camera->getPosition();

	state.setZEnable(FALSE);
	state.setZWriteEnable(FALSE);
	state.setColorWriteEnable(0x0000000f);
	state.setStencilEnable(FALSE);
	state.setAlphaBlendEnable(FALSE);
	state.setShadeMode(D3DSHADE_GOURAUD);
	state.setLighting(FALSE);
	state.setCullMode(D3DCULL_NONE);
	state.setMultiSampleAntialias(FALSE);
	state.setAlphaBlendEnable(TRUE);
	state.setBlendOp(D3DBLENDOP_ADD);
	state.setSrcBlend(D3DBLEND_ONE);
	state.setDestBlend(D3DBLEND_ONE);

	//stats
	u32 pointLightsOptimized = 0;
	u32 pointLightsSize = 0;
	u32 spotLightsOptimized = 0;
	u32 spotLightsSize = 0;
	u32 dirLightsSize = 0;

	u32 StencilID = 0;
	RECT scissorRect;
	u32 stencilEnabled = 0;
	u32 scissorEnabled = 0;
	u32 canOptimize = 0;
	for(u32 i = 0; i < in->culledLights.size(); i++)
	{
		if(in->culledLights[i]->getType() == LIGHT_POINT)
		{
			pointLightsSize++;
			stencilEnabled = 0;
			scissorEnabled = 0;
			canOptimize = (D3DXVec3Length(&Vec3(*in->culledLights[i]->getPos()-cam_pos))>in->culledLights[i]->getRange()*1.2f);
			
			if(r_deferredScissorOptimize && canOptimize)
			{
				scissorEnabled = 1;
				determineClipRect(in->culledLights[i]->getBoundingBox(), scissorRect);
				m_dev->SetScissorRect(&scissorRect);
				state.setScissorTestEnable(TRUE);
			}

			if(r_deferredStencilOptimize && canOptimize)
			{
				stencilEnabled = 1;
				StencilID++;
				setShader(0);		
				state.setZEnable(TRUE);
				state.setZWriteEnable(FALSE);
				state.setAlphaBlendEnable(FALSE);
				state.setStencilEnable(TRUE);
				state.setStencilRef(StencilID);
				state.setZFunc(D3DCMP_LESSEQUAL);
				state.setColorWriteEnable(0);
				state.setShadeMode(D3DSHADE_FLAT);
				state.setStencilFunc(D3DCMP_ALWAYS);
				state.setStencilZFail(D3DSTENCILOP_KEEP);
				state.setStencilPass(D3DSTENCILOP_REPLACE);
				state.setStencilFail(D3DSTENCILOP_KEEP);
				state.setCullMode(D3DCULL_CW);
				commitChanges();
				in->culledLights[i]->renderVolume();
				state.setCullMode(D3DCULL_CCW);
				commitChanges();
				in->culledLights[i]->renderVolume();
				state.setStencilFunc(D3DCMP_EQUAL);
				state.setStencilZFail(D3DSTENCILOP_KEEP);
				state.setStencilPass(D3DSTENCILOP_KEEP);
				state.setStencilFail(D3DSTENCILOP_KEEP);
				state.setZEnable(FALSE);
				state.setShadeMode(D3DSHADE_GOURAUD);
				state.setCullMode(D3DCULL_NONE);
				state.setAlphaBlendEnable(TRUE);
				state.setColorWriteEnable(0x0000000f);
			}
			else
			{
				state.setAlphaBlendEnable(TRUE);
			}

			if(r_deferredDebugComplexity)
			{
				if(stencilEnabled || scissorEnabled)
					setShader(m_deferredShaderLight->setTech("DebugComplexityGreen"));
				else
					setShader(m_deferredShaderLight->setTech("DebugComplexityRed"));
			}
			else
			{
				setShader(m_deferredShaderLight->setTech("PointLighting"));
			}

			m_deferredShaderLight->setVec4("g_lightPos",	in->culledLights[i]->getPosV4());
			m_deferredShaderLight->setVec4("g_lightColor",	in->culledLights[i]->getColor());
			m_deferredShaderLight->setF32 ("g_lightRange",	in->culledLights[i]->getRange());
			commitChanges();
			r2d->drawScreenQuadRaw();
			
			if(scissorEnabled || stencilEnabled)
			{
				pointLightsOptimized++;
				if(scissorEnabled)
					state.setScissorTestEnable(FALSE);
				if(stencilEnabled)
					state.setStencilEnable(FALSE);
			}
		}
		else if(in->culledLights[i]->getType() == LIGHT_DIR)
		{
			dirLightsSize++;
			if(r_deferredDebugComplexity)
			{
				setShader(m_deferredShaderLight->setTech("DebugComplexityBue"));
			}
			else if(in->culledLights[i]->castsShadows() && r_enabledShadows && r_enabledShadowMapping && r_enabledShadowMappingDeferred)
			{
				if(r_enabledShadowMappingPCF)
				{
					if(r_enabledShadowMappingPCFHigh)
					{
						gEngine.renderer->setShader(m_deferredShaderLight->setTech("DirLightingSM_Fast_PCF7x7"));
						// 7x7 PCF
						f32 fTexelSize = (f32)(1.0f/(f32)in->culledLights[i]->getDepthMap()->getParams().width);
						Vec4 vTexCoords[49];
						u32 i = 0;
						for(f32 x = -2.5; x <= 3.5; x++)
							for(f32 y = -2.5; y <= 3.5; y++)
								vTexCoords[i++] = Vec4(fTexelSize * (f32)x, fTexelSize * (f32)y, 0.0f, 0.0f);
						m_deferredShaderLight->setVal("vTexCoords7x7", &vTexCoords, 49 * sizeof(Vec4));
					}
					else if(r_enabledShadowMappingPCFMed)
					{
						gEngine.renderer->setShader(m_deferredShaderLight->setTech("DirLightingSM_Fast_PCF5x5"));
						// 5x5 PCF
						f32 fTexelSize = (f32)(1.0f/(f32)in->culledLights[i]->getDepthMap()->getParams().width);
						Vec4 vTexCoords[25];
						u32 i = 0;
						for(f32 x = -1.5; x <= 2.5; x++)
							for(f32 y = -1.5; y <= 2.5; y++)
								vTexCoords[i++] = Vec4(fTexelSize * (f32)x, fTexelSize * (f32)y, 0.0f, 0.0f);
						m_deferredShaderLight->setVal("vTexCoords5x5", &vTexCoords, 25 * sizeof(Vec4));
					}
					else
					{
						gEngine.renderer->setShader(m_deferredShaderLight->setTech("DirLightingSM_Fast_PCF3x3"));
						// 3x3 PCF
						f32 fTexelSize = (f32)(1.0f/(f32)in->culledLights[i]->getDepthMap()->getParams().width);
						Vec4 vTexCoords[9];
						u32 i = 0;
						for(f32 x = -0.5; x <= 1.5; x++)
							for(f32 y = -0.5; y <= 1.5; y++)
								vTexCoords[i++] = Vec4(fTexelSize * (f32)x, fTexelSize * (f32)y, 0.0f, 0.0f);
						m_deferredShaderLight->setVal("vTexCoords3x3", &vTexCoords, 9 * sizeof(Vec4));
					}
				}
				else
				{
					gEngine.renderer->setShader(m_deferredShaderLight->setTech("DirLightingSM_Fast"));
				}
				
				Mat texViewProj = (*(in->culledLights[i]->getViewProjMat())) * (*(in->culledLights[i]->getDepthMap()->getTexAdjMat()));
				m_deferredShaderLight->setMat("g_matTexture", &texViewProj);
				m_deferredShaderLight->setTex("ShadowMap", in->culledLights[i]->getDepthMap()->getColorTexture());
				m_deferredShaderLight->setF32("zBias", r_shadowMappingZBias);
			}
			else
			{
				setShader(m_deferredShaderLight->setTech("DirLighting"));
			}
	
			m_deferredShaderLight->setVec4("g_fogParams",	&r_fogParams);
			m_deferredShaderLight->setVec3("g_fogColor",	&r_fogColor);

			m_deferredShaderLight->setVec3("g_lightDir",	in->culledLights[i]->getDir());
			m_deferredShaderLight->setVec4("g_lightColor",	in->culledLights[i]->getColor());
			m_deferredShaderLight->setF32("shadowMapDist", g_shadowMapDirViewVolume);
			commitChanges();
			r2d->drawScreenQuadRaw();		
		}
		else if(in->culledLights[i]->getType() == LIGHT_SPOT)
		{
			spotLightsSize++;
			stencilEnabled = 0;
			scissorEnabled = 0;
			canOptimize = (!in->culledLights[i]->getBoundingCone()->intersect(&camBSphere));

			/*if(r_deferredScissorOptimize && canOptimize)
			{
				scissorEnabled = 1;
				determineClipRect(in->culledLights[i]->getBoundingBox(), scissorRect);
				m_dev->SetScissorRect(&scissorRect);
				state.setScissorTestEnable(TRUE);
			
			}*/
			
			if(r_deferredStencilOptimize && canOptimize)
			{
				stencilEnabled = 1;
				StencilID++;
				setShader(0);		
				state.setZEnable(TRUE);
				state.setZWriteEnable(FALSE);
				state.setAlphaBlendEnable(FALSE);
				state.setStencilEnable(TRUE);
				state.setStencilRef(StencilID);
				state.setZFunc(D3DCMP_LESSEQUAL);
				state.setColorWriteEnable(0);
				state.setShadeMode(D3DSHADE_FLAT);
				state.setStencilFunc(D3DCMP_ALWAYS);
				state.setStencilZFail(D3DSTENCILOP_KEEP);
				state.setStencilPass(D3DSTENCILOP_REPLACE);
				state.setStencilFail(D3DSTENCILOP_KEEP);
				state.setCullMode(D3DCULL_CW);
				commitChanges();
				in->culledLights[i]->renderVolume();
				state.setCullMode(D3DCULL_CCW);
				commitChanges();
				in->culledLights[i]->renderVolume();
				state.setStencilFunc(D3DCMP_EQUAL);
				state.setStencilZFail(D3DSTENCILOP_KEEP);
				state.setStencilPass(D3DSTENCILOP_KEEP);
				state.setStencilFail(D3DSTENCILOP_KEEP);
				state.setZEnable(FALSE);
				state.setShadeMode(D3DSHADE_GOURAUD);
				state.setCullMode(D3DCULL_NONE);
				state.setAlphaBlendEnable(TRUE);
				state.setColorWriteEnable(0x0000000f);	
			}
			else
			{
				state.setAlphaBlendEnable(TRUE);
			}

			if(r_deferredDebugComplexity)
			{
				if(stencilEnabled || scissorEnabled)
					setShader(m_deferredShaderLight->setTech("DebugComplexityGreen"));
				else
					setShader(m_deferredShaderLight->setTech("DebugComplexityRed"));
			}
			else if(in->culledLights[i]->castsShadows() && r_enabledShadows && r_enabledShadowMapping && r_enabledShadowMappingDeferred)
			{
				if(r_enabledShadowMappingPCF)
				{
					if(r_enabledShadowMappingPCFHigh)
					{
						gEngine.renderer->setShader(m_deferredShaderLight->setTech("SpotLightingSM_Fast_PCF7x7"));
						// 7x7 PCF
						f32 fTexelSize = (f32)(1.0f/(f32)in->culledLights[i]->getDepthMap()->getParams().width);
						Vec4 vTexCoords[49];
						u32 i = 0;
						for(f32 x = -3; x <= 3; x++)
							for(f32 y = -3; y <= 3; y++)
								vTexCoords[i++] = Vec4(fTexelSize * (f32)x, fTexelSize * (f32)y, 0.0f, 0.0f);
						m_deferredShaderLight->setVal("vTexCoords7x7", &vTexCoords, 49 * sizeof(Vec4));
					}
					else if(r_enabledShadowMappingPCFMed)
					{
						gEngine.renderer->setShader(m_deferredShaderLight->setTech("SpotLightingSM_Fast_PCF5x5"));
						// 5x5 PCF
						f32 fTexelSize = (f32)(1.0f/(f32)in->culledLights[i]->getDepthMap()->getParams().width);
						Vec4 vTexCoords[25];
						u32 i = 0;
						for(f32 x = -2; x <= 2; x++)
							for(f32 y = -2; y <= 2; y++)
								vTexCoords[i++] = Vec4(fTexelSize * (f32)x, fTexelSize * (f32)y, 0.0f, 0.0f);
						m_deferredShaderLight->setVal("vTexCoords5x5", &vTexCoords, 25 * sizeof(Vec4));
					}
					else
					{
						gEngine.renderer->setShader(m_deferredShaderLight->setTech("SpotLightingSM_Fast_PCF3x3"));
						// 3x3 PCF
						f32 fTexelSize = (f32)(1.0f/(f32)in->culledLights[i]->getDepthMap()->getParams().width);
						Vec4 vTexCoords[9];
						u32 i = 0;
						for(f32 x = -1; x <= 1; x++)
							for(f32 y = -1; y <= 1; y++)
								vTexCoords[i++] = Vec4(fTexelSize * (f32)x, fTexelSize * (f32)y, 0.0f, 0.0f);
						m_deferredShaderLight->setVal("vTexCoords3x3", &vTexCoords, 9 * sizeof(Vec4));
					}
				}
				else
				{
					gEngine.renderer->setShader(m_deferredShaderLight->setTech("SpotLightingSM_Fast"));
				}

				Mat texViewProj = (*(in->culledLights[i]->getViewProjMat())) * (*(in->culledLights[i]->getDepthMap()->getTexAdjMat()));
				m_deferredShaderLight->setMat("g_matTexture", &texViewProj);
				m_deferredShaderLight->setTex("ShadowMap", in->culledLights[i]->getDepthMap()->getColorTexture());
				m_deferredShaderLight->setF32("zBias", r_shadowMappingZBias);
			}
			else
			{
				setShader(m_deferredShaderLight->setTech("SpotLighting"));
			}

			m_deferredShaderLight->setVec4("g_lightPos",		in->culledLights[i]->getPosV4());
			m_deferredShaderLight->setVec3("g_lightDir",		in->culledLights[i]->getDir());
			m_deferredShaderLight->setVec4("g_lightColor",		in->culledLights[i]->getColor());
			m_deferredShaderLight->setF32 ("g_lightRange",		in->culledLights[i]->getRange());
			m_deferredShaderLight->setF32 ("g_lightCosPhiHalf",	in->culledLights[i]->getCosPhiHalf());
			commitChanges();
			r2d->drawScreenQuadRaw();

			if(scissorEnabled || stencilEnabled)
			{
				spotLightsOptimized++;
				if(scissorEnabled)
					state.setScissorTestEnable(FALSE);
				if(stencilEnabled)
					state.setStencilEnable(FALSE);
			}
		}
	}
	
	state.setZEnable(FALSE);
	state.setZWriteEnable(FALSE);
	state.setColorWriteEnable(0x0000000f);
	state.setStencilEnable(FALSE);
	state.setAlphaBlendEnable(FALSE);
	state.setShadeMode(D3DSHADE_GOURAUD);
	state.setLighting(FALSE);
	state.setCullMode(D3DCULL_NONE);
	state.setMultiSampleAntialias(FALSE);
	state.setAlphaBlendEnable(TRUE);
	state.setBlendOp(D3DBLENDOP_ADD);
	state.setSrcBlend(D3DBLEND_ONE);
	state.setDestBlend(D3DBLEND_ONE);
	m_rend.pass	= REND_SOLID;

	f32 pointOptimizeRatio = ((f32)pointLightsOptimized)/((f32)pointLightsSize);
	f32 spotOptimizeRatio = ((f32)spotLightsOptimized)/((f32)spotLightsSize);
	addTxt("Deferred lighting proccess:");
	addTxt("-Renderer point: %u (%u optimized - %.2f)",		pointLightsSize,	pointLightsOptimized,	pointOptimizeRatio);
	addTxt("-Renderer spot : %u (%u optimized - %.2f)",		spotLightsSize,		spotLightsOptimized,	spotOptimizeRatio);
	addTxt("-Renderer dir  : %u", dirLightsSize);
}

void Renderer::renderLightPrePass(RenderList *in)
{
	addTxt("Rend type: Light Pre Pass");
	disableHDR();
	
	//Z FILL -> Depth + Normals Rendering...
	m_depthNormals->enableRendering(0);
	setShader(0);
	clear(1,1,0,0);

	state.setZEnable(TRUE);
	state.setZWriteEnable(TRUE);
	state.setZFunc(D3DCMP_LESSEQUAL);
	state.setColorWriteEnable(0x0000000f);
	state.setStencilEnable(FALSE);
	state.setAlphaBlendEnable(FALSE);
	state.setShadeMode(D3DSHADE_GOURAUD);
	state.setLighting(FALSE);
	state.setCullMode(D3DCULL_CW);
	state.setMultiSampleAntialias(FALSE);

	if(r_multisample) state.setMultiSampleAntialias(TRUE);
	else state.setMultiSampleAntialias(FALSE);

	m_rend.pass = REND_ZFILL;
	if(r_renderTerrain)renderListLightPrePass(&in->culledTerrains);
	if(r_renderSurfaces)renderListLightPrePass(&in->culledSurfaces);
	if(r_renderEntities)renderListLightPrePass(&in->culledEntities);
	m_depthNormals->disableRendering();

	m_lightingLPP->enableRendering();
	renderLightPrePassLighting(in);
	m_lightingLPP->disableRendering();

	setShader(0);
	state.setAlphaBlendEnable(FALSE);
	commitChanges();
	r2d->drawScreenQuad(m_depthNormals->getColorTexture());
}

void Renderer::renderLightPrePassLighting(RenderList *in)
{
}

Shader* Renderer::beginGuiShader()
{
	r2d->begin();
	setShader(m_guiShader->setTech("GUI"));
	m_guiShader->setTex("Tex0", getBackBuffer(1)->getColorTexture());
	m_guiShader->setF32("time", gEngine.kernel->tmr->getAppTime() * 0.01f);
	commitChanges();
	return m_guiShader;

}
void Renderer::endGuiShader()
{
	setShader(0);
	r2d->end();
}

/*
bool smShader = false;
if(in->culledLights[i]->castsShadows() && r_enabledShadows && r_enabledShadowMapping && !r_enabledShadowMappingDeferred)
{
	sm->prepareShadowMap(in, in->culledLights[i], screenMap);

	if(r_enabledSMScreenSpaceBlur)
		guassianBlur->process(screenMap, blurMap[0], screenMap);

	smShader = true;
}

if(smShader)
{
	setShader(m_deferredShaderLight->setTech("DirLightingSM"));
	m_deferredShaderLight->setTex("ShadowMap", screenMap->getColorTexture());	
}
else if(r_enabledVarianceShadowMapping && !r_enabledExponentialShadowMapping)
{
	in->culledLights[i]->renderDepth(in);

	if(r_enabledBlurVarianceShadowMaps)
		guassianBlur->processCC(in->culledLights[i]->getDepthMap(), blurMapVSM[0], in->culledLights[i]->getDepthMap());

	if(r_enabledBoxFilterVarianceShadowMaps)
	{
		setShader(m_deferredShaderLight->setTech("DirLightingVSMBoxFilter"));
		
		// 3x3 PCF
		f32 fTexelSize = (f32)(1.0f/(f32)in->culledLights[i]->getDepthMap()->getParams().width);
		addTxt("DirLightingVSMBoxFilter : fTexelSize = %f", fTexelSize);
		Vec4 vTexCoords[9];
		u32 i = 0;
		for(f32 x = -1; x <= 1; x++)
			for(f32 y = -1; y <= 1; y++)
				vTexCoords[i++] = Vec4(fTexelSize * x, fTexelSize * y, 0.0f, 0.0f);
		m_deferredShaderLight->setVal("vsmBoxFilter3x3TexCoords", &vTexCoords, 9 * sizeof(Vec4));					
	}
	else
	{
		setShader(m_deferredShaderLight->setTech("DirLightingVSM"));
	}

	m_deferredShaderLight->setTex("VarianceShadowMap", in->culledLights[i]->getDepthMap()->getColorTexture());
	Mat texViewProj = (*(in->culledLights[i]->getViewProjMat())) * (*(in->culledLights[i]->getDepthMap()->getTexAdjMat()));
	m_deferredShaderLight->setMat("g_matTexture", &texViewProj);
}
else if(r_enabledExponentialShadowMapping)
{
	in->m_sunLight->renderDepth(in);

	guassianBlur->processLog(in->culledLights[i]->getDepthMap(), blurMap[0], in->culledLights[i]->getDepthMap());

	setShader(m_deferredShaderLight->setTech("DirLightingESM"));
		
	m_deferredShaderLight->setTex("ExponentialShadowMap", in->culledLights[i]->getDepthMap()->getColorTexture());
	Mat texViewProj = (*(in->culledLights[i]->getViewProjMat())) * (*(in->culledLights[i]->getDepthMap()->getTexAdjMat()));
	m_deferredShaderLight->setMat("g_matTexture", &texViewProj);
}
else if(in->culledLights[i]->castsShadows() && r_enabledShadows && r_enabledShadowMapping && r_enabledShadowMappingDeferred)
{
	in->culledLights[i]->renderDepth(in);

	gEngine.renderer->get().light = 0;
	gEngine.renderer->get().pass = REND_SOLID;

	if(r_enabledShadowMappingPCF)
	{
		if(r_enabledShadowMappingPCFHigh)
		{
			gEngine.renderer->setShader(m_deferredShaderLight->setTech("DirLightingSM_Fast_PCF7x7"));
			// 7x7 PCF
			f32 fTexelSize = (f32)(1.0f/(f32)in->culledLights[i]->getDepthMap()->getParams().width);
			Vec4 vTexCoords[49];
			u32 i = 0;
			for(f32 x = -2.5; x <= 3.5; x++)
				for(f32 y = -2.5; y <= 3.5; y++)
					vTexCoords[i++] = Vec4(fTexelSize * (f32)x, fTexelSize * (f32)y, 0.0f, 0.0f);
			m_deferredShaderLight->setVal("vTexCoords7x7", &vTexCoords, 49 * sizeof(Vec4));
		}
		else if(r_enabledShadowMappingPCFMed)
		{
			gEngine.renderer->setShader(m_deferredShaderLight->setTech("DirLightingSM_Fast_PCF5x5"));
			// 5x5 PCF
			f32 fTexelSize = (f32)(1.0f/(f32)in->culledLights[i]->getDepthMap()->getParams().width);
			Vec4 vTexCoords[25];
			u32 i = 0;
			for(f32 x = -1.5; x <= 2.5; x++)
				for(f32 y = -1.5; y <= 2.5; y++)
					vTexCoords[i++] = Vec4(fTexelSize * (f32)x, fTexelSize * (f32)y, 0.0f, 0.0f);
			m_deferredShaderLight->setVal("vTexCoords5x5", &vTexCoords, 25 * sizeof(Vec4));
		}
		else
		{
			gEngine.renderer->setShader(m_deferredShaderLight->setTech("DirLightingSM_Fast_PCF3x3"));
			// 3x3 PCF
			f32 fTexelSize = (f32)(1.0f/(f32)in->culledLights[i]->getDepthMap()->getParams().width);
			Vec4 vTexCoords[9];
			u32 i = 0;
			for(f32 x = -0.5; x <= 1.5; x++)
				for(f32 y = -0.5; y <= 1.5; y++)
					vTexCoords[i++] = Vec4(fTexelSize * (f32)x, fTexelSize * (f32)y, 0.0f, 0.0f);
			m_deferredShaderLight->setVal("vTexCoords3x3", &vTexCoords, 9 * sizeof(Vec4));
		}
	}
	else
	{
		gEngine.renderer->setShader(m_deferredShaderLight->setTech("DirLightingSM_Fast"));
	}
	
	Mat texViewProj = (*(in->culledLights[i]->getViewProjMat())) * (*(in->culledLights[i]->getDepthMap()->getTexAdjMat()));
	m_deferredShaderLight->setMat("g_matTexture", &texViewProj);
	m_deferredShaderLight->setTex("ShadowMap", in->culledLights[i]->getDepthMap()->getColorTexture());
	m_deferredShaderLight->setF32("zBias", r_shadowMappingZBias);
}
else
{
	setShader(m_deferredShaderLight->setTech("DirLighting"));
}
*/


u32 Renderer::renderListRaw(EntityList<Surface> *in)
{
	if(m_rend.pass == REND_SM)
	{
		for(u32 i = 0; i < in->dataSize; i++)
		{	
			Surface *surface = in->data[i];
			if(!surface)continue;
			if(r_geometryInstancing && surface->allowInstancing && surface->next)
			{
				setShader(surface->shader->setTech("RenderDepthInstancing"));
				setMaterial(surface->material);
				setWorld(&surface->world);
				if(surface->material->m_type == MT_LEAF)
				{
					surface->shader->setVec3("g_boundingBoxSphereCenter", &surface->sphereCenter);
					surface->shader->setF32("g_boundingBoxSphereRadius", surface->sphereRadius);
				}
				commitChanges();
				surface->mesh->renderInstanced(surface);
			}
			else
			{
				for(;surface;surface = surface->next)
				{
					setShader(surface->shader->setTech("RenderDepth"));
					setMaterial(surface->material);
					setWorld(&surface->world);
					if(surface->material->m_type == MT_LEAF)
					{
						surface->shader->setVec3("g_boundingBoxSphereCenter", &surface->sphereCenter);
						surface->shader->setF32("g_boundingBoxSphereRadius", surface->sphereRadius);
					}
					commitChanges();
					surface->mesh->render();
				}
			}
		}
	}
	else
	{
		for(u32 i = 0; i < in->dataSize; i++)
		{	
			Surface *surface = in->data[i];
			if(!surface)continue;
			if(r_geometryInstancing && surface->allowInstancing && surface->next)
			{
				setShader(surface->shader->setTech("RenderRawInstancing"));
				setMaterial(surface->material);
				setWorld(&surface->world);
				if(surface->material->m_type == MT_LEAF)
				{
					surface->shader->setVec3("g_boundingBoxSphereCenter", &surface->sphereCenter);
					surface->shader->setF32("g_boundingBoxSphereRadius", surface->sphereRadius);
				}
				commitChanges();
				surface->mesh->renderInstanced(surface);
			}
			else
			{
				for(;surface;surface = surface->next)
				{
					setShader(surface->shader->setTech("RenderRaw"));
					setMaterial(surface->material);
					setWorld(&surface->world);
					if(surface->material->m_type == MT_LEAF)
					{
						surface->shader->setVec3("g_boundingBoxSphereCenter", &surface->sphereCenter);
						surface->shader->setF32("g_boundingBoxSphereRadius", surface->sphereRadius);
					}
					commitChanges();
					surface->mesh->render();

					if(g_rendObjDebugBB)
						gEngine.renderer->drawBoundingBox(&surface->currentNode->boundingBox, Vec4(1,0,0,1));
				}
			}
		}
	}
	return 1;
}

u32 Renderer::renderListForward(EntityList<Surface> *in, Light* light)
{
	for(u32 i = 0; i < in->dataSize; i++)
	{	
		Surface *surface = in->data[i];
		if(!surface)continue;
		if(r_geometryInstancing && surface->allowInstancing && surface->next)
		{
			setShader(surface->shader->setTech("RenderForwardInstancing"));
			setMaterial(surface->material);
			setupLight(light);
			setWorld(&surface->world);
			if(surface->material->m_type == MT_LEAF)
			{
				surface->shader->setVec3("g_boundingBoxSphereCenter", &surface->sphereCenter);
				surface->shader->setF32("g_boundingBoxSphereRadius", surface->sphereRadius);
			}
			commitChanges();
			surface->mesh->renderInstanced(surface);
		}
		else
		{
			for(;surface;surface = surface->next)
			{
				setShader(surface->shader->setTech("RenderForward"));
				setMaterial(surface->material);
				setupLight(light);
				setWorld(&surface->world);
				if(surface->material->m_type == MT_LEAF)
				{
					surface->shader->setVec3("g_boundingBoxSphereCenter", &surface->sphereCenter);
					surface->shader->setF32("g_boundingBoxSphereRadius", surface->sphereRadius);
				}
				commitChanges();
				surface->mesh->render();
			}
		}
	}
	return 0;
}


u32 Renderer::renderListDeferred(EntityList<Surface> *in)
{
	for(u32 i = 0; i < in->dataSize; i++)
	{	
		Surface *surface = in->data[i];
		if(!surface)continue;
		if(r_geometryInstancing && surface->allowInstancing && surface->next)
		{
			setShader(surface->shader->setTech("RenderDeferredInstancing"));
			setMaterial(surface->material);
			setWorld(&surface->world);
			if(surface->material->m_type == MT_LEAF)
			{
				surface->shader->setVec3("g_boundingBoxSphereCenter", &surface->sphereCenter);
				surface->shader->setF32("g_boundingBoxSphereRadius", surface->sphereRadius);
			}
			commitChanges();
			surface->mesh->renderInstanced(surface);
		}
		else
		{
			for(;surface;surface = surface->next)
			{
				setShader(surface->shader->setTech("RenderDeferred"));
				setMaterial(surface->material);
				setWorld(&surface->world);
				if(surface->material->m_type == MT_LEAF)
				{
					surface->shader->setVec3("g_boundingBoxSphereCenter", &surface->sphereCenter);
					surface->shader->setF32("g_boundingBoxSphereRadius", surface->sphereRadius);
				}
				commitChanges();
				surface->mesh->render();
			}
		}
	}

	return 1;
}

u32 Renderer::renderListLightPrePass(EntityList<Surface> *in)
{
	if(m_rend.pass == REND_ZFILL)
	{
		for(u32 i = 0; i < in->dataSize; i++)
		{	
			Surface *surface = in->data[i];
			if(!surface)continue;
			if(r_geometryInstancing && surface->allowInstancing && surface->next)
			{
				setShader(surface->shader->setTech("RenderDepthNormalsInstancing"));
				setMaterial(surface->material);
				setWorld(&surface->world);
				if(surface->material->m_type == MT_LEAF)
				{
					surface->shader->setVec3("g_boundingBoxSphereCenter", &surface->sphereCenter);
					surface->shader->setF32("g_boundingBoxSphereRadius", surface->sphereRadius);
				}
				commitChanges();
				surface->mesh->renderInstanced(surface);
			}
			else
			{
				for(;surface;surface = surface->next)
				{
					setShader(surface->shader->setTech("RenderDepthNormals"));
					setMaterial(surface->material);
					setWorld(&surface->world);
					if(surface->material->m_type == MT_LEAF)
					{
						surface->shader->setVec3("g_boundingBoxSphereCenter", &surface->sphereCenter);
						surface->shader->setF32("g_boundingBoxSphereRadius", surface->sphereRadius);
					}
					commitChanges();
					surface->mesh->render();
				}
			}
		}
	}
	else
	{
		for(u32 i = 0; i < in->dataSize; i++)
		{	
			Surface *surface = in->data[i];
			if(!surface)continue;
			if(r_geometryInstancing && surface->allowInstancing && surface->next)
			{
				setShader(surface->shader->setTech("RenderLPPInstancing"));
				setMaterial(surface->material);
				setWorld(&surface->world);
				if(surface->material->m_type == MT_LEAF)
				{
					surface->shader->setVec3("g_boundingBoxSphereCenter", &surface->sphereCenter);
					surface->shader->setF32("g_boundingBoxSphereRadius", surface->sphereRadius);
				}
				commitChanges();
				surface->mesh->renderInstanced(surface);
			}
			else
			{
				for(;surface;surface = surface->next)
				{
					setShader(surface->shader->setTech("RenderLPP"));
					setMaterial(surface->material);
					setWorld(&surface->world);
					if(surface->material->m_type == MT_LEAF)
					{
						surface->shader->setVec3("g_boundingBoxSphereCenter", &surface->sphereCenter);
						surface->shader->setF32("g_boundingBoxSphereRadius", surface->sphereRadius);
					}
					commitChanges();
					surface->mesh->render();
				}
			}
		}
	}

	return 1;
}

//////////////////////////////////////

u32 Renderer::renderListRaw(EntityList<Terrain> *in)
{
	if(!in->data)return 0;
	Terrain *terrain = in->data[0];
	if(!terrain)return 0;
	for(;terrain;terrain = terrain->next)
	{
		terrain->renderRaw();
	}
	return 1;
}

u32 Renderer::renderListForward(EntityList<Terrain> *in, Light* light)
{
	if(!in->data)return 0;
	Terrain *terrain = in->data[0];
	if(!terrain)return 0;
	for(;terrain;terrain = terrain->next)
	{
		terrain->renderForward(light);
	}
	return 1;
}

u32 Renderer::renderListDeferred(EntityList<Terrain> *in)
{
	if(!in->data)return 0;
	Terrain *terrain = in->data[0];
	if(!terrain)return 0;
	for(;terrain;terrain = terrain->next)
	{
		terrain->renderDeferred();
	}
	return 1;
}

u32 Renderer::renderListLightPrePass(EntityList<Terrain> *in)
{
	if(!in->data)return 0;
	/*Terrain *terrain = in->data[0];
	if(!terrain)return 0;
	for(;terrain;terrain = terrain->next)
	{
		terrain->renderDeferred();
	}*/
	return 1;
}

//////////////////////////////////////////////////////////////////

u32 Renderer::renderListRaw(EntityList<IEntity> *in)
{
	for(u32 i = 0; i < in->dataSize; i++)
	{	
		IEntity *entity = in->data[i];
		if(!entity)continue;
		for(;entity;entity = entity->next)
		{
			entity->renderRaw();
		}
	}
	return 1;
}

u32 Renderer::renderListForward(EntityList<IEntity> *in, Light* light)
{
	for(u32 i = 0; i < in->dataSize; i++)
	{	
		IEntity *entity = in->data[i];
		if(!entity)continue;
		for(;entity;entity = entity->next)
		{
			entity->renderForward(light);
		}
	}
	return 1;
}

u32 Renderer::renderListDeferred(EntityList<IEntity> *in)
{
	for(u32 i = 0; i < in->dataSize; i++)
	{	
		IEntity *entity = in->data[i];
		if(!entity)continue;
		for(;entity;entity = entity->next)
		{
			entity->renderDeferred();
		}
	}
	return 1;
}

u32 Renderer::renderListLightPrePass(EntityList<IEntity> *in)
{
	//for(u32 i = 0; i < in->dataSize; i++)
	//{	
	//	IEntity *entity = in->data[i];
	//	if(!entity)continue;
	//	for(;entity;entity = entity->next)
	//	{
	//		//if(entity->rendEnabled())
	//		entity->renderDeferred();
	//	}
	//}
	return 1;
}

#include "Scene.h"

Bloom * m_bloom;

Scene::Scene(ICamera* camera, u32 skybox)
	: m_camera(camera)
{
	g_sunDir = Vec3(0.1,-1,0.2);
	g_sunColor = Vec4(0.8,0.9,1.0,1);
	g_skyLight = new Light(LIGHT_DIR, 1, Vec4(1,0,0,1), Vec3(0,0,0), 1000, g_sunDir);
	g_skyFront = Vec4(0.8,0.7,0.2,1);
	g_skyBack  = Vec4(0.6,0.6,0.8,1);

	m_renderList = new RenderList();
	m_renderList->clear();
	m_renderList->m_sunLight = g_skyLight;
	//add(g_skyLight);

	m_sky = 0;

	if(skybox)
	{
		m_sky = new SkyDome(gEngine.resMgr->load<Texture>("sky/clouds2.png"), gEngine.resMgr->load<Texture>("sky/clouds.png"), 2.0f, 0.7f, g_skyFront, g_skyBack, 0.06, 0.03, Vec2(0, 1), 24, 24, 1, 2.0);
		m_sky->setSunLight(g_skyLight);
	}
	
	m_bloom = new Bloom();
}

Scene::~Scene()
{
	delete(m_bloom);
	delete(m_sky);
	delete(m_renderList);
}

void Scene::add(Surface* entity, u32 dynamic)
{
	if(entity)
	{
		m_renderList->add(entity, dynamic);
	}
}

void Scene::add(Terrain* entity)
{
	if(entity)
	{
		m_renderList->add(entity);
	}
}

void Scene::addWater(f32 H)
{
	//simple water
	m_renderList->water = new Water(H);
	m_renderList->water->setSunLight(g_skyLight);

	//high quality water
	m_renderList->hqWater = new HQWater(H);
	m_renderList->hqWater->setSunLight(g_skyLight);

	//bottom
	add(new Surface("%gm plane 30000 10", new Material(MT_DEFAULT, ""), Vec3(-15000,0,-15000), Vec3(0,0,0), 1), 1);
}

void Scene::add(IEntity* entity, u32 dynamic)
{
	if(entity)
	{
		m_renderList->add(entity, dynamic);
	}
}

void Scene::add(Light* light)
{
	if(light)
	{
		m_renderList->add(light);
	}
}

void Scene::update()
{
	if(gEngine.desktop->isFocused() && gEngine.input->isKeyDown(DIK_M) && !gEngine.input->isKeyDown(DIK_H))
		g_sunDir = -*m_camera->getViewDirection();

	if(r_sunColorEnabled)
		g_skyLight->setColor(&Vec4(r_sunColor, 1));
	else
		g_skyLight->setColor(&g_sunColor);

	g_skyLight->setDir(&g_sunDir);

	Vec4 sc = *g_skyLight->getColor();
	Vec3 ld = g_sunDir;
	Vec3 hd = Vec3(0, 1, 0);
	D3DXVec3Normalize(&ld, &ld);
	D3DXVec3Normalize(&hd, &hd);
	f32 dot = D3DXVec3Dot(&hd, &ld);
	dot = (dot < 0)?(0):(dot);
	m_bright = pow(1-dot,100);
	sc = Vec4(sc.x, sc.y, sc.z, m_bright);
	g_skyLight->setColor(&sc);

	gEngine.renderer->setSunBright(m_bright);
	gEngine.renderer->setAmbientColor( 0.2 * m_bright + 0.35 * (1 - m_bright) );

	gEngine.renderer->addTxt("SunLight color: %f %f %f", g_skyLight->getColor()->x, g_skyLight->getColor()->y, g_skyLight->getColor()->z);

	if(m_sky)
	{
		if(r_skyColorEnabled)
		{
			m_sky->setColorFront(Vec4(r_skyColorFront.x, r_skyColorFront.y, r_skyColorFront.z, 1) * m_bright + Vec4(0.3,0.4,0.5,1) * (1-m_bright));
			m_sky->setColorBack(Vec4(r_skyColorBack.x, r_skyColorBack.y, r_skyColorBack.z, 1) * m_bright + Vec4(0.2,0.2,0.2,1) * (1-m_bright));
		}
		else
		{
			m_sky->setColorFront(g_skyFront * m_bright + Vec4(0.3,0.4,0.5,1) * (1-m_bright));
			m_sky->setColorBack(g_skyBack * m_bright + Vec4(0.2,0.2,0.2,1) * (1-m_bright));
		}

		m_sky->update(m_camera);
	}
	m_renderList->updateLights(m_camera);
}

void Scene::preRender()
{
	gEngine.renderer->setCamera(m_camera);
	
	//if(gEngine.input->isKeyDown(DIK_F2))
	{
		gEngine.resMgr->createLoadingThread();
	}

	gEngine.renderer->cull(m_renderList);
	m_renderList->sortCulledLights();
}

void Scene::render()
{
	gEngine.renderer->setCamera(m_camera);

	gEngine.renderer->getReflectionTexture()->enableRendering();
	gEngine.renderer->clear(1,1,1,0);
	gEngine.renderer->get().pass = REND_REFLECT;
	gEngine.renderer->state.setClipPlaneEnable(0);
	if(m_sky && r_renderSky && r_enabledSkyReflection)
		m_sky->render();
	gEngine.renderer->renderReflection(m_renderList);
	gEngine.renderer->getReflectionTexture()->disableRendering();

	gEngine.renderer->get().pass = REND_SOLID;
	gEngine.renderer->state.setClipPlaneEnable(0);

	if(r_enabledReflectionBlur)
	{
		gEngine.renderer->guassianBlur->process(gEngine.renderer->m_reflection, gEngine.renderer->m_reflectionBlur, gEngine.renderer->m_reflection);
	}

	if(r_enabledVLS && (gEngine.renderer->get().type == REND_DEFERRED || gEngine.renderer->get().type == REND_LIGHTPREPASS))
	{
		gEngine.renderer->vls->getTexture()->enableRendering();
		//RENDER SUN ONLY
		if(m_sky)
			m_sky->renderSun();
		gEngine.renderer->vls->getTexture()->disableRendering();
	}

	gEngine.renderer->beginSceneRender();
	
	if(m_sky && r_renderSky)
		m_sky->render();

	gEngine.renderer->render(m_renderList);

	if(g_rendOctreeDebugBB)
	{
		m_renderList->surfaceOctree->renderBB(Vec4(1,0,0,1));
		m_renderList->entityOctree->renderBB(Vec4(0,1,0,1));
		m_renderList->terrainOctree->renderBB(Vec4(0,0,1,1));
	}

	if(r_bloomEnabled)
		m_bloom->process();

	if(gEngine.input->isKeyDown(DIK_R))
	{
		gEngine.renderer->r2d->drawQuad(gEngine.renderer->getReflectionTexture()->getColorTexture(), 0, 0, 512, 512, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
	}

	if(gEngine.input->isKeyDown(DIK_T))
	{
		gEngine.renderer->r2d->drawQuad(gEngine.renderer->vls->getTexture()->getColorTexture(), 0, 0, 512, 512, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
	}
}

void Scene::load()
{
	if(IDYES == MessageBox(0, "Click YES button to load 50052 point lights\r\nor\r\nNO button to load 8100 point lights.", "XGINE Tester App", MB_YESNO))
	{
		Texture *colorMap = 0;
		D3DLOCKED_RECT lockedRect;
		colorMap = gEngine.resMgr->load<Texture>("colorMap258x194.bmp", 0);
		if(!colorMap)return;
		if(!colorMap->get() || FAILED(colorMap->get()->LockRect(0, &lockedRect, 0, D3DLOCK_READONLY)))return;
		for(int y = 0; y < 194; y++)
		{
			for(int x = 0; x < 258; x++)
			{
				Light *light = new Light(LIGHT_POINT, 0, Vec4(1.5,1.5,1.5, 2.0f), Vec3(x*15.0f,65,y*15.0f), 20);
				u32 c = ((u32*)lockedRect.pBits)[y * colorMap->width + x];
				D3DXCOLOR cc = D3DXCOLOR(c);
				//light->setColor(&Vec4(((f32)(rand()%100))/80.0f,((f32)(rand()%100))/80.0f,((f32)(rand()%100))/80.0f,1));
				light->setColor(&Vec4(cc.r+0.5f, cc.g+0.5f, cc.b+0.5f, 1.2f));
				add(light);
			}
		}
		colorMap->get()->UnlockRect(0);
		gEngine.resMgr->release(colorMap);
	}
	else
	{
		Texture *colorMap = 0;
		D3DLOCKED_RECT lockedRect;
		colorMap = gEngine.resMgr->load<Texture>("colorMap151x114.bmp", 0);
		if(!colorMap)return;
		if(!colorMap->get() || FAILED(colorMap->get()->LockRect(0, &lockedRect, 0, D3DLOCK_READONLY)))return;
		for(int y = 0; y < 90; y++)
		{
			for(int x = 0; x < 90; x++)
			{
				Light *light = new Light(LIGHT_POINT, 0, Vec4(1.5,1.5,1.5, 2.0f), Vec3(x*15.0f,65,y*15.0f), 20);
				u32 c = ((u32*)lockedRect.pBits)[y * colorMap->width + x];
				D3DXCOLOR cc = D3DXCOLOR(c);
				//light->setColor(&Vec4(((f32)(rand()%100))/80.0f,((f32)(rand()%100))/80.0f,((f32)(rand()%100))/80.0f,1));
				light->setColor(&Vec4(cc.r+0.5f, cc.g+0.5f, cc.b+0.5f, 1.2f));
				add(light);
			}
		}
		colorMap->get()->UnlockRect(0);
		gEngine.resMgr->release(colorMap);
	}
	
	Surface* ent = new Surface("waterPlane4.x", new Material(MT_DEFAULT, "brick2.jpg", "brick2_n.jpg", "", 30.0f,1,100,1,DT_RELIEF_MAPPING,"",Vec4(1.2,1.2,1.2,0.8),1), Vec3(-1000,60,-1000), Vec3(0,0,0), 2);
	add(ent, 0);
}
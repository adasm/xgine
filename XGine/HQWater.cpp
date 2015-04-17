#include "HQWater.h"

struct CUSTOM_VERTEX_WATER
{
	Vec3	pos;
	Vec2	texCoord;
	Vec3	normal;
	Vec3	binormal;
	Vec3	tangent;
};

const u32 g_waterNormalMapSize = 512.0f;

HQWater::HQWater(f32 H)
{
	m_waterY = H;

	m_shader = gEngine.shaderMgr->load("HQWater.fx");

	if(!m_shader)
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "HQWater", "Couldn't load shader for HQWater!");
		m_shader = 0;
	}

	position = Vec3(0,H,0);
	rotation = Vec3(0,0,0);

	m_hmap			= gEngine.resMgr->load<Texture>("water/hmap.jpg");
	m_hmap2			= gEngine.resMgr->load<Texture>("water/hmap2.png");		
	m_dhmap			= gEngine.resMgr->load<Texture>("water/hmap.png");
	m_fresnel		= gEngine.resMgr->load<Texture>("water/fresnel.bmp");
	m_foam			= gEngine.resMgr->load<Texture>("water/foam.jpg");

	if(!m_hmap || !m_hmap2 || !m_dhmap || !m_fresnel || !m_foam)
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "HQWater", "Couldn't create all textures for HQWater!");
		m_hmap = m_hmap2 = m_dhmap = m_fresnel = m_foam = 0;
	}

	RenderTexture::CreationParams cp;
	cp.width = g_waterNormalMapSize;
	cp.height = g_waterNormalMapSize;
	cp.colorFormat = D3DFMT_A32B32G32R32F;
	cp.depthFormat = D3DFMT_D24S8;

	m_normalMap = new RenderTexture();
	if(!m_normalMap->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "HQWater", "Couldn't create normalMap texture for HQWater!");
		m_normalMap = 0;
	}
	m_detailNormalMap = new RenderTexture();
	if(!m_detailNormalMap->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "HQWater", "Couldn't create detailNormalMap texture for HQWater!");
		m_detailNormalMap = 0;
	}

	m_mustUpdateNormalMap			= 1;
	m_mustUpdateDetailNormalMap		= 1;

	m_updatedNormalMapType			= 1000;

	mesh			= gEngine.resMgr->load<Mesh>("%gm plane 1000 120");
	meshMed			= gEngine.resMgr->load<Mesh>("%gm plane 2000 80");
	meshLow			= gEngine.resMgr->load<Mesh>("%gm plane 6000 40");

	if(!mesh || !meshMed || !meshLow)
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "HQWater", "Couldn't create meshes for HQWater!");
		mesh = meshMed = meshLow = 0;
	}

	Vec3 minbb = Vec3(-13000,0,-13000);
	Vec3 maxbb = Vec3(13000,20,13000);
	boundingBox = BoundingBox(minbb, maxbb);
	boundingBox.setTransform(Vec3(0,0,0));
}

HQWater::~HQWater()
{
	if(m_hmap){ gEngine.resMgr->release(m_hmap); m_hmap = 0; }
	if(m_hmap2){ gEngine.resMgr->release(m_hmap2); m_hmap = 0; }
	if(m_dhmap){ gEngine.resMgr->release(m_dhmap); m_dhmap = 0; }
	if(m_fresnel){ gEngine.resMgr->release(m_fresnel); m_fresnel = 0; }
	if(m_foam){ gEngine.resMgr->release(m_foam); m_foam = 0; }
	if(m_normalMap){ m_normalMap->close(); delete(m_normalMap); m_normalMap = 0; }
	if(m_detailNormalMap){ m_detailNormalMap->close(); delete(m_detailNormalMap); m_detailNormalMap = 0; }
	if(mesh){ gEngine.resMgr->release(mesh); mesh = 0; }
	if(meshMed){ gEngine.resMgr->release(meshMed); meshMed = 0; }
	if(meshLow){ gEngine.resMgr->release(meshLow); meshLow = 0; }
}

void HQWater::update(ICamera* cam)
{
	position.x = cam->getPosition()->x;
	position.z = cam->getPosition()->z;
	position.y = m_waterY;

	Vec3 axis;
	f32	 angle;
	Vec3 camDir = Vec3(cam->getViewDirection()->x, 0, cam->getViewDirection()->z);
	D3DXVec3Normalize(&camDir, &camDir);
	Vec3 planeDir = Vec3(1,0,1);
	D3DXVec3Normalize(&planeDir, &planeDir);
	D3DXVec3Cross(&axis, &planeDir, &camDir);
	angle = acos(D3DXVec3Dot(&planeDir, &camDir));
	D3DXMatrixRotationAxis(&rot, &axis, angle);
	worldMat(&meshWorld, position);
	meshWorld = rot * meshWorld;

	position.y = 0;	
	worldMat(&world, position, rotation);
	D3DXMatrixInverse(&invWorld, 0, &world);
	boundingBox.setTransform(position);
}

void HQWater::updateNormalMap()
{
	Texture* hm = (r_waterOcean)?(m_hmap2):(m_hmap);
	if(hm->get() == 0)return;

	m_normalMap->enableRendering(0);
	gEngine.renderer->clear(1,0,0,0);
	gEngine.renderer->setShader(m_shader->setTech("GenerateNM"));
	m_shader->setTex("HMap",			hm);
	m_shader->setVec2("HMTexelSize",	&Vec2(1.0f/(f32)hm->width, 1.0f/(f32)hm->height) );
	m_shader->setF32("m_waterY",		m_waterY);
	m_shader->setF32("Amplitude",		r_waterAmplitude);
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawQuadTextured(g_waterNormalMapSize, g_waterNormalMapSize);
	m_normalMap->disableRendering();

	m_updatedNormalMapType = r_waterOcean;
	m_mustUpdateNormalMap = 0;

	gEngine.renderer->addTxtF("Updated HQWater Normal Map!");
}

void HQWater::updateDetailNormalMap()
{
	if(m_dhmap->get() == 0)return;

	m_detailNormalMap->enableRendering(0);
	gEngine.renderer->clear(1,0,0,0);
	gEngine.renderer->setShader(m_shader->setTech("GenerateNM"));
	m_shader->setTex("HMap",			m_dhmap);
	m_shader->setVec2("HMTexelSize",	&Vec2(1.0f/(f32)m_dhmap->width, 1.0f/(f32)m_dhmap->height) );
	m_shader->setF32("m_waterY",		m_waterY);
	m_shader->setF32("Amplitude",		r_waterAmplitude);
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawQuadTextured(g_waterNormalMapSize, g_waterNormalMapSize);
	m_detailNormalMap->disableRendering();

	m_mustUpdateDetailNormalMap	= 0;

	gEngine.renderer->addTxtF("Updated HQWater Detail Normal Map!");
}	

u32 HQWater::render()
{
	if(!m_shader || !m_hmap || !m_hmap2 || !m_dhmap || !m_fresnel || !m_foam || !m_normalMap || !m_detailNormalMap || !mesh || !meshMed || !meshLow)
		return 0;
	else
	{
		update(gEngine.renderer->get().camera);
		
		static f32 lastWaterAmp = 0;
		if(lastWaterAmp != r_waterAmplitude){ lastWaterAmp = r_waterAmplitude; m_mustUpdateNormalMap = 1; }

		m_waterY = 3*r_waterAmplitude;
		gEngine.renderer->setReflectionHeight(m_waterY);

		if(m_updatedNormalMapType != r_waterOcean)m_mustUpdateNormalMap = m_mustUpdateDetailNormalMap = 1;

		if(m_mustUpdateNormalMap || m_mustUpdateDetailNormalMap)
		{
			gEngine.renderer->disableHDR();

			if(m_mustUpdateNormalMap)updateNormalMap();
			if(m_mustUpdateDetailNormalMap)updateDetailNormalMap();

			gEngine.renderer->enableHDR();
		}	

		Mat g_matInvViewProj;
		D3DXMatrixInverse(&g_matInvViewProj, 0, gEngine.renderer->get().camera->getViewProjectionMat());
		
		if(gEngine.renderer->get().type == REND_DEFERRED)
			gEngine.renderer->setShader(m_shader->setTech("WaterDeferred"));
		else
			gEngine.renderer->setShader(m_shader->setTech("Water"));

		m_shader->setTex("DEPTHMAP",				gEngine.renderer->getMRT()->getRT(2));
		m_shader->setF32("m_waterY",				m_waterY);
		m_shader->setMat("g_matInvViewProj",		&g_matInvViewProj);
		m_shader->setTex("ReflectionMap",			gEngine.renderer->getReflectionTexture()->getColorTexture());
		if(gEngine.renderer->get().type == REND_DEFERRED)
			m_shader->setTex("RefractionMap",			gEngine.renderer->getMRT()->getRT(0));
		else
			m_shader->setTex("RefractionMap",			gEngine.renderer->getBackBuffer(0)->getColorTexture());
		m_shader->setTex("HMap",					m_normalMap->getColorTexture());
		m_shader->setTex("DetailNormalMap",			m_detailNormalMap->getColorTexture());
		m_shader->setF32("detailNormalMapScale",	r_waterDetailNormalMapScale);
		m_shader->setTex("Fresnel",					m_fresnel);
		m_shader->setTex("Foam",					m_foam);
		m_shader->setF32("Amplitude",				r_waterAmplitude);
		m_shader->setVec3("g_waterColor",			&r_waterColor);
		m_shader->setF32("h0coef",					r_waterH0coef);
		m_shader->setF32("h1coef",					r_waterH1coef);
		m_shader->setF32("h2coef",					r_waterH2coef);
		m_shader->setF32("h3coef",					r_waterH3coef);
		m_shader->setF32("detailNormalMapCoef",		r_waterDetailNormalMapCoef);
		m_shader->setF32("g_R0",					r_waterFresnelR0);
		m_shader->setF32("g_refractionStrength",	r_waterRefractionStrength);
		m_shader->setF32("g_waterSpecularExp",		r_waterSpecularExp);
		m_shader->setF32("g_hMapTextureSize",		g_waterNormalMapSize);
		m_shader->setF32("g_hMapTextureTexelSize",	1.0f/(f32)g_waterNormalMapSize);
		m_shader->setVec2("half_texel",				&Vec2(0.5f/(f32)gEngine.device->getWidth(), 0.5f/(f32)gEngine.device->getHeight()));
		m_shader->setF32("g_time",					gEngine.kernel->tmr->getFrameTime() * 0.001f);
		m_shader->setF32("g_refExp",				r_refExp);

		if(g_sunLight)
		{
			m_shader->setVec3("g_sunDir",	g_sunLight->getDir());
			m_shader->setVec4("g_sunColor", g_sunLight->getColor());
		}
		else
		{
			m_shader->setVec3("g_sunDir",	&Vec3(0, -1, 0));
			m_shader->setVec4("g_sunColor", &Vec4(1,1,1,1));
		}

		m_shader->setVec4("g_fogParams",	&r_fogParams);
		m_shader->setVec3("g_fogColor",		&r_fogColor);
		

		gEngine.renderer->state.setAlphaBlendEnable(TRUE);
		gEngine.renderer->state.setBlendOp(D3DBLENDOP_ADD);
		gEngine.renderer->state.setSrcBlend(D3DBLEND_SRCALPHA);
		gEngine.renderer->state.setDestBlend(D3DBLEND_INVSRCALPHA);

		Mat trans;
		position.y = 0;
		for(f32 x = 0; x < 9000;)
		{
			for(f32 y = 0; y < 9000;)
			{
				if(x >= 3000 || y >= 3000)
				{
					worldMat(&trans, Vec3(x - 100.0f, -2, y - 100.0f));
					worldMat(&meshWorld, position);
					meshWorld = trans * rot * meshWorld;
					gEngine.renderer->setWorld(&meshWorld);
					gEngine.renderer->commitChanges();
					meshLow->render();
				}
				else if(x >= 1000 || y >= 1000)
				{
					worldMat(&trans, Vec3(x - 100.0f, -1, y - 100.0f));
					worldMat(&meshWorld, position);
					meshWorld = trans * rot * meshWorld;
					gEngine.renderer->setWorld(&meshWorld);
					gEngine.renderer->commitChanges();
					meshMed->render();
				}
				else
				{
					worldMat(&trans, Vec3(x - 100.0f, 0, y - 100.0f));
					worldMat(&meshWorld, position);
					meshWorld = trans * rot * meshWorld;
					gEngine.renderer->setWorld(&meshWorld);
					gEngine.renderer->commitChanges();
					mesh->render();
				}

				if(y >= 3000)y+=6000;
				else if(y >= 1000)y+=2000;
				else y+= 1000;
			}

			if(x >= 3000)x+=6000;
			else if(x >= 1000)x+=2000;
			else x+= 1000;
		}

		return 1;
	}
}
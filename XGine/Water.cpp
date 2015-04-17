#include "Water.h"

struct CUSTOM_VERTEX_WATER
{
	Vec3	pos;
	Vec2	texCoord;
	Vec3	normal;
	Vec3	binormal;
	Vec3	tangent;
};

Water::Water(f32 H)
{
	m_waterY = H;

	m_shader		= gEngine.shaderMgr->load("Water.fx");
	position = Vec3(0,H,0);
	rotation = Vec3(0,0,0);

	m_texture1		= gEngine.resMgr->load<Texture>("water/water1.jpg");
	m_texture2		= gEngine.resMgr->load<Texture>("water/water2.jpg");
	m_texture3		= gEngine.resMgr->load<Texture>("water/foam.jpg");

	worldMat(&world, position, rotation);
	D3DXMatrixInverse(&invWorld, 0, &world);

	CUSTOM_VERTEX_WATER	*v;
	WORD				*indices;

	m_vbSize = 8;
	if(FAILED(gEngine.device->getDev()->CreateVertexBuffer(m_vbSize * sizeof(CUSTOM_VERTEX_WATER), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_vb, NULL)))
		{ gEngine.kernel->log->prnEx(LT_ERROR, "Water", "Could not create vertex buffer for water"); return; }
	
	m_ibSize = 12;
	if(FAILED(gEngine.device->getDev()->CreateIndexBuffer(m_ibSize * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ib, NULL)))
		{ gEngine.kernel->log->prnEx(LT_ERROR, "Water", "Could not border index buffer for water");  return; }

	if(FAILED(D3DXCreateTexture(gEngine.device->getDev(), gEngine.device->getWidth(), gEngine.device->getHeight(), D3DX_DEFAULT, D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP ,D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_backBuffer)))
		{ gEngine.kernel->log->prnEx(LT_ERROR, "Water", "Could not create back buffer texture - water"); return; }

	Vec3 p0 = Vec3(-13000,-1,-13000);
	Vec3 p1 = Vec3(13000,1,13000);
	f32	maxTexCoord = 100.0f;

	Vec3 minbb = p0, maxbb = p1;

	m_vb->Lock(0, 0, (void**)&v, 0);
	m_ib->Lock(0, 0, (void**)&indices, 0);

	for(u32 i = 0; i < 2; i++)
	{
		float h = (i==0)?(H):(-1.0f-H);

		v[i*4+0].pos = Vec3(p0.x,h,p0.z);
		v[i*4+0].texCoord = Vec2(0,0);
		v[i*4+0].normal = Vec3(0,1,0);
		v[i*4+0].binormal = Vec3(0,0,-1);
		v[i*4+0].tangent = Vec3(1,0,0);

		v[i*4+01].pos = Vec3(p0.x,h,p1.z);
		v[i*4+01].texCoord = Vec2(0,maxTexCoord);
		v[i*4+01].normal = Vec3(0,1,0);
		v[i*4+01].binormal = Vec3(0,0,-1);
		v[i*4+01].tangent = Vec3(1,0,0);
		
		v[i*4+02].pos = Vec3(p1.x,h,p1.z);
		v[i*4+02].texCoord = Vec2(maxTexCoord,maxTexCoord);
		v[i*4+02].normal = Vec3(0,1,0);
		v[i*4+02].binormal = Vec3(0,0,-1);
		v[i*4+02].tangent = Vec3(1,0,0);
		
		v[i*4+03].pos = Vec3(p1.x,h,p0.z);
		v[i*4+03].texCoord = Vec2(maxTexCoord,0);
		v[i*4+03].normal = Vec3(0,1,0);
		v[i*4+03].binormal = Vec3(0,0,-1);
		v[i*4+03].tangent = Vec3(1,0,0);
	}
	
	indices[0] = 2;
	indices[1] = 1;
	indices[2] = 0;
	indices[3] = 2;
	indices[4] = 0;
	indices[5] = 3;

	indices[6] = 6;
	indices[7] = 5;
	indices[8] = 4;
	indices[9] = 6;
	indices[10] = 4;
	indices[11] = 7;

	m_vb->Unlock();
	m_ib->Unlock();

	boundingBox = BoundingBox(minbb, maxbb);
	boundingBox.setTransform(Vec3(0,0,0));
}

Water::~Water()
{
	gEngine.resMgr->release(m_texture1);
	gEngine.resMgr->release(m_texture2);
	gEngine.resMgr->release(m_texture3);
	DXRELEASE(m_backBuffer);
	DXRELEASE(m_vb);
	DXRELEASE(m_ib);
}

void Water::update(ICamera* cam)
{
	position.x = cam->getPosition()->x;
	position.z = cam->getPosition()->z;
	position.y = 0;
	worldMat(&world, position, rotation);
	D3DXMatrixInverse(&invWorld, 0, &world);
	boundingBox.setTransform(position);
}

u32 Water::render()
{
	if(m_vb && m_ib && m_shader)
	{
		update(gEngine.renderer->get().camera);

		if(gEngine.renderer->get().type == REND_DEFERRED)
		{
			gEngine.renderer->setShader(m_shader->setTech("WaterDeferred"));
			m_shader->setTex("DEPTHMAP", gEngine.renderer->getMRT()->getRT(2));
			m_shader->setF32("m_waterY", m_waterY);
			Mat g_matInvViewProj;
			D3DXMatrixInverse(&g_matInvViewProj, 0, gEngine.renderer->get().camera->getViewProjectionMat());
			m_shader->setMat("g_matInvViewProj", &g_matInvViewProj);
		}
		else
			gEngine.renderer->setShader(m_shader->setTech("Water"));	
		gEngine.renderer->setWorld(&world);

		if(gEngine.renderer->get().type == REND_DEFERRED)
			m_shader->setTex("RefractionMap",			gEngine.renderer->getMRT()->getRT(0));
		else
			m_shader->setTex("RefractionMap",			gEngine.renderer->getBackBuffer(0)->getColorTexture());

		m_shader->setTex("NormalMap1", m_texture1);
		m_shader->setTex("NormalMap2", m_texture2);
		m_shader->setTex("NormalMap3", m_texture3);
		m_shader->setVec2("half_texel", &Vec2(0.5f/(f32)gEngine.device->getWidth(), 0.5f/(f32)gEngine.device->getHeight()));

		m_shader->setF32("g_time", gEngine.kernel->tmr->getAppTime() * 0.001f);
		if(g_sunLight)
		{
			m_shader->setVec3("g_sunDir", g_sunLight->getDir());
			m_shader->setVec4("g_sunColor", g_sunLight->getColor());
		}
		else
		{
			m_shader->setVec3("g_sunDir", &Vec3(0, -1, 0));
			m_shader->setVec4("g_sunColor", &Vec4(1,1,1,1));
		}

		gEngine.renderer->state.setAlphaBlendEnable(TRUE);
		gEngine.renderer->state.setBlendOp(D3DBLENDOP_ADD);
		gEngine.renderer->state.setSrcBlend(D3DBLEND_SRCALPHA);
		gEngine.renderer->state.setDestBlend(D3DBLEND_INVSRCALPHA);

		gEngine.renderer->commitChanges();

			gEngine.device->getDev()->SetVertexDeclaration(gEngine.renderer->m_vd[VD_DEFAULT]);
			gEngine.device->getDev()->SetStreamSource(0, m_vb, 0, sizeof(CUSTOM_VERTEX_WATER));
			gEngine.device->getDev()->SetIndices(m_ib);
			gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vbSize, 0, 2);
		return 1;
	}
	return 0;
}
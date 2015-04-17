#include "Particle.h"

struct XGINE_API VertexBillboard
{
	Vec3 pos;
	Vec2 expandStart;
};

static D3DXMATRIX matWorldViewProj, matWorldInv;
static UINT Passes, i;
static LPD3DXEFFECT lpEffect;
static D3DXVECTOR4 tmp;

Vegetation::Vegetation(const char* textureName, Vec3 *positions, u32 num, u32 size, BillboardRotation rot)
	: m_num(num), m_rot(rot)
{
	mesh = 0;

	position = Vec3(0,0,0);

	const D3DXVECTOR2 expandDirs[4] = { D3DXVECTOR2(-1,-1), D3DXVECTOR2(1,-1), D3DXVECTOR2(-1,1), D3DXVECTOR2(1,1) };
	size /= 2.0f;

	gEngine.device->getDev()->CreateVertexBuffer(sizeof(VertexBillboard) * m_num * 4, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_vb, 0);
	gEngine.device->getDev()->CreateIndexBuffer(m_num*6*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ib, 0);
	VertexBillboard* data;
	WORD* idata;
	m_vb->Lock(0, 0, (void**)&data, 0);
	m_ib->Lock(0, 0, (void**)&idata, 0);
	Vec3 min = Vec3(9999999,999999,9999999), max = Vec3(-9999999,-999999,-9999999);
	for(i32 i = 0, p = 0; i < m_num * 4; ++i)
	{
		if(!(i%4))
		{
			data[i].pos = positions[p++];

			if(data[i].pos.x < min.x)min.x = data[i].pos.x;
			if(data[i].pos.y < min.y)min.y = data[i].pos.y;
			if(data[i].pos.z < min.z)min.z = data[i].pos.z;

			if(data[i].pos.x > max.x)max.x = data[i].pos.x;
			if(data[i].pos.y > max.y)max.y = data[i].pos.y;
			if(data[i].pos.z > max.z)max.z = data[i].pos.z;

			idata[6*(i/4)]		= 4*(i/4)+1;
			idata[6*(i/4)+1]	= 4*(i/4);
			idata[6*(i/4)+2]	= 4*(i/4)+3;
			idata[6*(i/4)+3]	= 4*(i/4);
			idata[6*(i/4)+4]	= 4*(i/4)+2;
			idata[6*(i/4)+5]	= 4*(i/4)+3;
		}
		else
			data[i] = data[i-1];

		data[i].expandStart = expandDirs[i%4] * size;
	}
	m_vb->Unlock();
	m_ib->Unlock();

	worldInvMat(&world, &invWorld, position);
	

	m_texture	= gEngine.resMgr->load<Texture>(textureName);
	shader	= gEngine.shaderMgr->load("Billboards.fx");

	// Calculate estimated bounding box
	
	boundingBox.reset(min - Vec3(size,size,size), max + Vec3(size,size,size));
	boundingBox.setTransform(position);
}

Vegetation::~Vegetation()
{
	DXRELEASE(m_vb);
	DXRELEASE(m_ib);
	gEngine.resMgr->release(m_texture);
}

void Vegetation::update(ICamera* cam)
{
}

void Vegetation::updateOrigin()
{
	worldInvMat(&world, &invWorld, position);
	boundingBox.setTransform(position);
}

REND_TYPE Vegetation::getRendType()
{ 
	return REND_DEFERRED; 
}

u32 Vegetation::renderForward(Light* light)
{
	if(shader && m_vb && m_ib && m_texture)
	{
		LPDIRECT3DDEVICE9 dev = gEngine.device->getDev();
		ICamera *currCam = gEngine.renderer->get().camera;

		gEngine.renderer->setShader(shader->setTech("Billboards"));
		
		
		// settings:
		
		update(currCam);
		static Mat worldViewProj;
		worldViewProj = world * (*currCam->getViewProjectionMat());
		shader->setMat("g_matWorldViewProj", &worldViewProj);
		if(m_rot == BILLBOARDROT_ALL)
		{
			D3DXVec4Transform(&tmp, currCam->getViewUp(), &invWorld);
			shader->setVec4("g_viewUp", &tmp);
			D3DXVec4Transform(&tmp, currCam->getViewLeft(), &invWorld);
			shader->setVec4("g_viewLeft", &tmp);
		}
		else if(m_rot == BILLBOARDROT_LEFT)
		{
			shader->setVec4("g_viewUp", &Vec4(0,1,0,1));
			D3DXVec4Transform(&tmp, currCam->getViewLeft(), &invWorld);
			shader->setVec4("g_viewLeft", &tmp);
		}
		else if(m_rot == BILLBOARDROT_UP)
		{
			D3DXVec4Transform(&tmp, currCam->getViewUp(), &invWorld);
			shader->setVec4("g_viewUp", &tmp);
			shader->setVec4("g_viewLeft", &Vec4(1,0,0,1));
		}
		else
		{
			shader->setVec4("g_viewUp", &Vec4(0,1,0,1));
			shader->setVec4("g_viewLeft", &Vec4(1,0,0,1));
		}
		shader->setTex("ColorMap", m_texture);
		
		{// rendering:
			gEngine.renderer->state.setAlphaBlendEnable(FALSE);
			gEngine.renderer->commitChanges();
			dev->SetVertexDeclaration(gEngine.renderer->m_vd[VD_VEGETATION]);
			dev->SetStreamSource(0, m_vb, 0, sizeof(VertexBillboard));
			dev->SetIndices(m_ib);
			dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_num*4, 0, m_num*2);
			gEngine.renderer->addRenderedCounts(m_num*4, m_num*2);
		}

		return 1;
	}
	return 0;
}

u32 Vegetation::renderDeferred()
{
	if(shader && m_vb && m_ib && m_texture)
	{
		LPDIRECT3DDEVICE9 dev = gEngine.device->getDev();
		ICamera *currCam = gEngine.renderer->get().camera;

		gEngine.renderer->setShader(shader->setTech("BillboardsDeferred"));
		
		
		// settings:
		
		update(currCam);
		static Mat worldViewProj;
		worldViewProj = world * (*currCam->getViewProjectionMat());
		shader->setMat("g_matWorldViewProj", &worldViewProj);
		if(m_rot == BILLBOARDROT_ALL)
		{
			D3DXVec4Transform(&tmp, currCam->getViewUp(), &invWorld);
			shader->setVec4("g_viewUp", &tmp);
			D3DXVec4Transform(&tmp, currCam->getViewLeft(), &invWorld);
			shader->setVec4("g_viewLeft", &tmp);
		}
		else if(m_rot == BILLBOARDROT_LEFT)
		{
			shader->setVec4("g_viewUp", &Vec4(0,1,0,1));
			D3DXVec4Transform(&tmp, currCam->getViewLeft(), &invWorld);
			shader->setVec4("g_viewLeft", &tmp);
		}
		else if(m_rot == BILLBOARDROT_UP)
		{
			D3DXVec4Transform(&tmp, currCam->getViewUp(), &invWorld);
			shader->setVec4("g_viewUp", &tmp);
			shader->setVec4("g_viewLeft", &Vec4(1,0,0,1));
		}
		else
		{
			shader->setVec4("g_viewUp", &Vec4(0,1,0,1));
			shader->setVec4("g_viewLeft", &Vec4(1,0,0,1));
		}
		shader->setTex("ColorMap", m_texture);
		
		{// rendering:
			gEngine.renderer->state.setAlphaBlendEnable(FALSE);
			gEngine.renderer->commitChanges();
			dev->SetVertexDeclaration(gEngine.renderer->m_vd[VD_VEGETATION]);
			dev->SetStreamSource(0, m_vb, 0, sizeof(VertexBillboard));
			dev->SetIndices(m_ib);
			dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_num*4, 0, m_num*2);
			gEngine.renderer->addRenderedCounts(m_num*4, m_num*2);
		}

		return 1;
	}
	return 0;
}

u32 Vegetation::renderRaw()
{
	if(gEngine.renderer->get().pass == REND_SM)
	{
		return 0;
	}
	else if(gEngine.renderer->get().pass == REND_UNLIT)
	{
		return 0;
	}

	if(shader && m_vb && m_ib && m_texture)
	{
		LPDIRECT3DDEVICE9 dev = gEngine.device->getDev();
		ICamera *currCam = gEngine.renderer->get().camera;

		gEngine.renderer->setShader(shader->setTech("BillboardsRaw"));
		
		
		// settings:
		
		update(currCam);
		static Mat worldViewProj;
		worldViewProj = world * (*currCam->getViewProjectionMat());
		shader->setMat("g_matWorldViewProj", &worldViewProj);
		if(m_rot == BILLBOARDROT_ALL)
		{
			D3DXVec4Transform(&tmp, currCam->getViewUp(), &invWorld);
			shader->setVec4("g_viewUp", &tmp);
			D3DXVec4Transform(&tmp, currCam->getViewLeft(), &invWorld);
			shader->setVec4("g_viewLeft", &tmp);
		}
		else if(m_rot == BILLBOARDROT_LEFT)
		{
			shader->setVec4("g_viewUp", &Vec4(0,1,0,1));
			D3DXVec4Transform(&tmp, currCam->getViewLeft(), &invWorld);
			shader->setVec4("g_viewLeft", &tmp);
		}
		else if(m_rot == BILLBOARDROT_UP)
		{
			D3DXVec4Transform(&tmp, currCam->getViewUp(), &invWorld);
			shader->setVec4("g_viewUp", &tmp);
			shader->setVec4("g_viewLeft", &Vec4(1,0,0,1));
		}
		else
		{
			shader->setVec4("g_viewUp", &Vec4(0,1,0,1));
			shader->setVec4("g_viewLeft", &Vec4(1,0,0,1));
		}
		shader->setTex("ColorMap", m_texture);
		
		{// rendering:
			gEngine.renderer->state.setAlphaBlendEnable(FALSE);
			gEngine.renderer->commitChanges();
			dev->SetVertexDeclaration(gEngine.renderer->m_vd[VD_VEGETATION]);
			dev->SetStreamSource(0, m_vb, 0, sizeof(VertexBillboard));
			dev->SetIndices(m_ib);
			dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_num*4, 0, m_num*2);
			gEngine.renderer->addRenderedCounts(m_num*4, m_num*2);
		}

		return 1;
	}
	return 0;
}
#include "Particle.h"

struct XGINE_API VertexParticle
{
	D3DCOLOR	diffuseStart;
	D3DCOLOR	diffuseEnd;
	D3DXVECTOR2 expandStart;
	D3DXVECTOR4 seDeLtAv; //sizeEndFactor,delay,lifeTime,angularVel
	D3DXVECTOR3 velStart;
	D3DXVECTOR3 velEnd;
};

static D3DXMATRIX matWorldViewProj, matWorldInv;
static UINT Passes, i;
static LPD3DXEFFECT lpEffect;
static D3DXVECTOR4 tmp;

inline float randf(float center, float randomness)
{
	return center + randomness * ((rand() % 100) / 100.0f - 0.5f);
}

inline D3DXVECTOR3 randv3(D3DXVECTOR3* center, D3DXVECTOR3* randomness)
{
	return D3DXVECTOR3(randf(center->x, randomness->x), randf(center->y, randomness->y), randf(center->z, randomness->z));
}

void CalculateMinMax(const float& vS, const float& vE, const float& l, float* min, float* max)
{
	if(vS * vE < 0)
	{
		float l0 = -(vS * l) / (vE - vS); //m-ce zerowe
		if(vS > 0)
		{
			*max = 0.5f * l0 * vS;
			*min = *max + 0.5f * (l - l0) * vE;
		}
		else
		{
			*min = 0.5f * l0 * vS;
			*max = *min + 0.5f * (l - l0) * vE;
		}
	}
	else if(vS > 0)
	{
		*min = 0;
		*max = 0.5f * l * (vS + vE);
	}
	else
	{
		*max = 0;
		*min = 0.5f * l * (vS + vE);
	}
}

Particle::Particle(	Vec3* origin, const char* textureName, ParticleBlending blending, i32 num,
					f32 lifeTime, f32 lifeTimeRandomness,
					Vec3* velocityStart, Vec3* velocityEnd, Vec3* velocityRandomness,
					Vec4* diffuseStart,  Vec4* diffuseEnd,  Vec4* diffuseRandomness,
					f32 sizeStart, f32 sizeEnd, f32 sizeRandomness,
					f32 angularVel, f32 angularVelRandomness, BillboardRotation rot )
	: m_elapsedTime(lifeTime), m_num(num), m_blending(blending), m_rot(rot)
{
	position = *origin;
	mesh = 0;

	const D3DXVECTOR2 expandDirs[4] = { D3DXVECTOR2(-1,-1), D3DXVECTOR2(1,-1), D3DXVECTOR2(-1,1), D3DXVECTOR2(1,1) };
	sizeStart /= 2.0f; //cuz expand is -1..1

	gEngine.device->getDev()->CreateVertexBuffer(sizeof(VertexParticle) * m_num * 4, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_vb, 0);
	gEngine.device->getDev()->CreateIndexBuffer(m_num*6*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ib, 0);
	VertexParticle* data;
	WORD* idata;
	m_vb->Lock(0, 0, (void**)&data, 0);
	m_ib->Lock(0, 0, (void**)&idata, 0);
	f32 lastSizeStart;
	for(i32 i = 0; i < m_num * 4; ++i)
	{
		if(!(i%4))
		{
			lastSizeStart = randf(sizeStart, sizeRandomness);
			data[i].velStart = randv3(velocityStart, velocityRandomness);
			data[i].velEnd = randv3(velocityEnd, velocityRandomness);
			data[i].seDeLtAv = Vec4(
				randf(sizeEnd, sizeRandomness) / lastSizeStart, //sizeEnd
				((i/4) / (float)(m_num)) * lifeTime, //delay
				randf(lifeTime, lifeTimeRandomness), //lifeTime
				randf(angularVel, angularVelRandomness)); //angularVel
			data[i].diffuseStart = D3DCOLOR_COLORVALUE(randf(diffuseStart->x, diffuseRandomness->x),
				randf(diffuseStart->y, diffuseRandomness->y), randf(diffuseStart->z, diffuseRandomness->z),
				randf(diffuseStart->w, diffuseRandomness->w));
			data[i].diffuseEnd = D3DCOLOR_COLORVALUE(randf(diffuseEnd->x, diffuseRandomness->x),
				randf(diffuseEnd->y, diffuseRandomness->y), randf(diffuseEnd->z, diffuseRandomness->z),
				randf(diffuseEnd->w, diffuseRandomness->w));
			
			idata[6*(i/4)]		= 4*(i/4)+1;
			idata[6*(i/4)+1]	= 4*(i/4);
			idata[6*(i/4)+2]	= 4*(i/4)+3;
			idata[6*(i/4)+3]	= 4*(i/4);
			idata[6*(i/4)+4]	= 4*(i/4)+2;
			idata[6*(i/4)+5]	= 4*(i/4)+3;
		}
		else
			data[i] = data[i-1];

		data[i].expandStart = expandDirs[i%4] * lastSizeStart;
	}
	m_vb->Unlock();
	m_ib->Unlock();

	worldInvMat(&world, &invWorld, position);
	

	m_texture	= gEngine.resMgr->load<Texture>(textureName);
	shader	= gEngine.shaderMgr->load("Particles.fx");

	// Calculate estimated bounding box
	float l = lifeTime + lifeTimeRandomness * 0.5f;
	Vec3 min, max;
	CalculateMinMax(velocityStart->x, velocityEnd->x, l, &min.x, &max.x);
	CalculateMinMax(velocityStart->y, velocityEnd->y, l, &min.y, &max.y);
	CalculateMinMax(velocityStart->z, velocityEnd->z, l, &min.z, &max.z);
	boundingBox.reset(min - *velocityRandomness * 0.5f, max + *velocityRandomness * 0.5f);
	boundingBox.setTransform(position);
}

Particle::~Particle()
{
	DXRELEASE(m_vb);
	DXRELEASE(m_ib);
	gEngine.resMgr->release(m_texture);
}

void Particle::update(ICamera* cam)
{
	m_elapsedTime += gEngine.kernel->tmr->get() * 0.001f;
}

void Particle::updateOrigin()
{
	worldInvMat(&world, &invWorld, position);
	boundingBox.setTransform(position);
}

REND_TYPE Particle::getRendType()
{ 
	return REND_FORWARD_TRANS; 
}

u32 Particle::renderForward(Light* light)
{
	if(shader && m_vb && m_ib && m_texture)
	{
		LPDIRECT3DDEVICE9 dev = gEngine.device->getDev();
		ICamera *currCam = gEngine.renderer->get().camera;

		if(r_enabledSoftParticles && gEngine.renderer->get().type == REND_DEFERRED)
		{
			gEngine.renderer->setShader(shader->setTech("SoftParticles"));
			shader->setTex("MRT2", gEngine.renderer->getMRT()->getRT(2));
			shader->setVec2("half_texel", &Vec2(0.5f/(f32)gEngine.device->getWidth(), 0.5f/(f32)gEngine.device->getHeight()));
			shader->setMat("g_matViewProj", currCam->getViewProjectionMat());
			Mat g_matInvViewProj;
			D3DXMatrixInverse(&g_matInvViewProj, 0, currCam->getViewProjectionMat());
			shader->setMat("g_matInvViewProj", &g_matInvViewProj);

		}
		else
			gEngine.renderer->setShader(shader->setTech("Particles"));
		
		
		// settings:
		
		update(currCam);
		static Mat worldViewProj;
		worldViewProj = world * (*currCam->getViewProjectionMat());
		shader->setMat("g_matWorldViewProj", &worldViewProj);
		shader->setF32("g_time", m_elapsedTime);
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
			gEngine.renderer->state.setZEnable(TRUE);
			gEngine.renderer->state.setZWriteEnable(TRUE);
			gEngine.renderer->state.setAlphaBlendEnable(TRUE);
			if(m_blending == PARTICLEB_DEF)
			{
				gEngine.renderer->state.setBlendOp(D3DBLENDOP_ADD);
				gEngine.renderer->state.setSrcBlend(D3DBLEND_SRCALPHA);
				gEngine.renderer->state.setDestBlend(D3DBLEND_INVSRCALPHA);
			}
			else if(m_blending == PARTICLEB_ADD)
			{
				gEngine.renderer->state.setBlendOp(D3DBLENDOP_ADD);
				gEngine.renderer->state.setSrcBlend(D3DBLEND_SRCALPHA);
				gEngine.renderer->state.setDestBlend(D3DBLEND_ONE);
			}
			else if(m_blending == PARTICLEB_SUB)
			{
				gEngine.renderer->state.setBlendOp(D3DBLENDOP_SUBTRACT);
				gEngine.renderer->state.setSrcBlend(D3DBLEND_SRCALPHA);
				gEngine.renderer->state.setDestBlend(D3DBLEND_ONE);
			}
			else if(m_blending == PARTICLEB_RSUB)
			{
				gEngine.renderer->state.setBlendOp(D3DBLENDOP_REVSUBTRACT);
				gEngine.renderer->state.setSrcBlend(D3DBLEND_SRCALPHA);
				gEngine.renderer->state.setDestBlend(D3DBLEND_DESTALPHA);
			}
			gEngine.renderer->commitChanges();
			dev->SetVertexDeclaration(gEngine.renderer->m_vd[VD_PARTICLE]);
			dev->SetStreamSource(0, m_vb, 0, sizeof(VertexParticle));
			dev->SetIndices(m_ib);
			dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_num*4, 0, m_num*2);
			gEngine.renderer->addRenderedCounts(m_num*4, m_num*2);
		}


		gEngine.renderer->state.setZEnable(TRUE);
		gEngine.renderer->state.setZWriteEnable(TRUE);
		gEngine.renderer->state.setAlphaBlendEnable(FALSE);
		return 1;
	}
	return 0;
}
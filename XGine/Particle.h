#pragma once
#include "XGine.h"

enum XGINE_API ParticleBlending
{
	PARTICLEB_DEF,
	PARTICLEB_ADD,
	PARTICLEB_SUB,
	PARTICLEB_RSUB
};

enum XGINE_API BillboardRotation
{
	BILLBOARDROT_ALL,
	BILLBOARDROT_LEFT,
	BILLBOARDROT_UP,
	BILLBOARDOT_NONE
};

struct XGINE_API Particle : IEntity
{
protected:
	LPDIRECT3DVERTEXBUFFER9			m_vb;
	LPDIRECT3DINDEXBUFFER9			m_ib;
	f32								m_elapsedTime;
	BoundingBox						m_boundingBox;
	i32								m_num;
	Texture*						m_texture;
	ParticleBlending				m_blending;
	BillboardRotation				m_rot;
	
public:
	Particle(Vec3* origin, const char* textureName, ParticleBlending blending, i32 num,
			f32 lifeTime, f32 lifeTimeRandomness,
			Vec3* velocityStart, Vec3* velocityEnd, Vec3* velocityRandomness,
			Vec4* diffuseStart,  Vec4* diffuseEnd,  Vec4* diffuseRandomness,
			f32 sizeStart, f32 sizeEnd, f32 sizeRandomness,
			f32 angularVel, f32 angularVelRandomness, BillboardRotation rot = BILLBOARDROT_ALL);
	~Particle();

	void	update(ICamera* cam = 0);
	u32		renderDeferred() { return 0; }
	u32		renderForward(Light* light = 0);
	u32		renderRaw() { return 0; }
	void	setSunLight(Light *light) { }
	void	updateOrigin();	
	void	setPosition(Vec3 &pos)	{ position = (pos); }

	inline u32			rendEnabled()	{ return 1; }
	inline u32			selfRendering() { return 1; }
	inline Mesh*		getMesh()		{ return 0; }
	inline Material*	getMaterial()	{ return 0; }
	inline Shader*		getShader()		{ return shader; }
	inline Texture*		getTexture()	{ return m_texture; }
	inline u32			hasShadow()		{ return 0; }
		   REND_TYPE	getRendType();
};
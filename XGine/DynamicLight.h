#pragma once
#include "XGine.h"

struct XGINE_API DynamicLight : IEntity
{
protected:
	Shader*		m_shader;
	Mat			m_world;
	Vec3		m_position;
	BoundingBox	m_boundingBox;
	
	NxActor				*m_actor;
	Light				*m_light;
	Mesh				*m_mesh;

public:
	DynamicLight(Light *light, NxActor *actor, Mesh *mesh);
	~DynamicLight();

	void update();
	u32  render();
	u32  renderLS(Light*, RenderTexture*) { return 0; }
	u32  renderRaw() { return 0; }

	void	addLight(Light*) { }
	void	clearLights() { }
	u32		getNumLights() { return 0; }
	Light*	getLight(u32 i) { return 0; }

	inline const Vec3&	getPos()			{ return m_position; }
	inline Mat*			getWorldMat()		{ return &m_world; }
	LPD3DXMESH			getMeshPtr()		{ return 0; }
	inline BoundingBox* getBoundingBox()	{ return &m_boundingBox; }
	inline NxActor*		getActor()			{ return m_actor; }

	inline u32			isTransparent()		{ return 1; }
};
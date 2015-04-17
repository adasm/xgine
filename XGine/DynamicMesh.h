#pragma once
#include "XGine.h"

struct XGINE_API DynamicMesh : IEntity
{
protected:
	Mesh*		m_mesh;
	IMaterial*	m_mat;
	Mat			m_world;
	Mat			m_invWorld;
	Vec3		m_position;
	Vec3		m_rotation;
	BoundingBox	m_boundingBox;

	vector<Light*> m_lights;
	
	NxActor				*m_actor;
	f32					m_dynamicsMass;
	f32					m_dynamicsDensity;

public:
	DynamicMesh(Mesh *mesh, IMaterial *mat, Vec3 position, Vec3 rotation, f32 density, CollisionShapeType collisionShape, Vec3 *initialVel = 0);
	~DynamicMesh();

	void update();
	u32  render();
	u32	 renderLS(Light *light, RenderTexture* shadowMap);
	u32  renderRaw();

	void	addLight(Light*);
	void	clearLights();
	u32		getNumLights();
	Light*	getLight(u32 i);

	inline void			setPos(Vec3 &position)	{ m_position = (position); }
	inline void			setRot(Vec3 &rotation)	{ m_rotation = (rotation); }

	inline const Vec3&	getPos()			{ return m_position; }
	inline Mat*			getWorldMat()		{ return &m_world; }
	LPD3DXMESH			getMeshPtr();
	inline BoundingBox* getBoundingBox()	{ return &m_boundingBox; }
	inline NxActor*		getActor()			{ return m_actor; }

	inline u32			isTransparent()		{ return 0; }
};
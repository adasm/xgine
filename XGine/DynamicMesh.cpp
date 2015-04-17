#include "DynamicMesh.h"

DynamicMesh::DynamicMesh(Mesh *mesh, IMaterial *mat, Vec3 position, Vec3 rotation, f32 density, CollisionShapeType collisionShape, Vec3 *initialVel)
	: m_mesh(0), m_mat(0), m_actor(0)
{
	m_mesh			= mesh;
	m_mat			= mat;
	m_position		= position;
	m_rotation		= rotation;

	m_dynamicsDensity = density;

	worldMat(&m_world, m_position, m_rotation);

	if(m_mesh)
	{
		m_boundingBox = m_mesh->getBoundingBox();
		m_boundingBox.setTransform(m_position);
	}
	
	if(collisionShape == TRIANGLE)
	{
		gEngine.kernel->log->prn("Dynamic mesh can not use triangle-collision type.");
		return;
	}

	if(m_mesh)
		m_actor = gEngine.dynamics->createActor(m_mesh, &m_world, m_dynamicsDensity, collisionShape, initialVel);

	//if(m_actor)
	//	m_actor->setMass(m_dynamicsMass);
}

DynamicMesh::~DynamicMesh()
{
	gEngine.dynamics->release(m_actor);
	delete(m_mat);
	gEngine.resMgr->release(m_mesh);
}

void DynamicMesh::update()
{
	if(m_actor)
	{
		m_actor->getGlobalPose().getColumnMajor44((NxF32*)&m_world);
		m_position = *(Vec3*)&m_actor->getGlobalPosition();
		m_boundingBox.setTransform(m_position);
	}
}

u32 DynamicMesh::render()
{
	if(m_mesh && m_mat)
	{
		for(u32 lightID=0; lightID<m_lights.size(); lightID++)
		{
			if(!m_mat->apply(&m_world, m_lights[lightID]))return 0;
			for(u32 i = 0; i < m_mat->numPasses(); i++)
			{
				m_mat->beginPass(i);
				m_mesh->render();
				m_mat->endPass();
			}
		}
		return 1;
	}
	return 0;
}

u32	DynamicMesh::renderLS(Light *light, RenderTexture* shadowMap)
{
	if(m_mesh && m_mat)
	{
		if(!m_mat->apply(&m_world, light, shadowMap))return 0;
		for(u32 i = 0; i < m_mat->numPasses(); i++)
		{
			m_mat->beginPass(i);
			m_mesh->render();
			m_mat->endPass();
		}
		return 1;
	}
	return 0;
}

u32 DynamicMesh::renderRaw()
{
	if(m_mesh)
	{
		gEngine.renderer->setWorld(&m_world);
		m_mesh->render();
		return 1;
	}
	return 0;
}

void DynamicMesh::addLight(Light* light)
{
	m_lights.push_back(light);
}

void DynamicMesh::clearLights()
{
	m_lights.clear();
}

u32 DynamicMesh::getNumLights()
{ 
	return m_lights.size(); 
}
Light* DynamicMesh::getLight(u32 i) 
{ 
	return m_lights[i]; 
}

LPD3DXMESH DynamicMesh::getMeshPtr()
{
	return (m_mesh)?(m_mesh->getMeshPtr()):(0);
}
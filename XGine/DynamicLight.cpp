#include "DynamicLight.h"

DynamicLight::DynamicLight(Light *light, NxActor *actor, Mesh *mesh)
	: m_light(light), m_actor(actor), m_mesh(mesh)
{
	m_shader = gEngine.shaderMgr->load("ZFill.fx");

	if(m_mesh)
	{
		m_boundingBox = m_mesh->getBoundingBox();
		m_boundingBox.setTransform(m_position);
	}
}

DynamicLight::~DynamicLight()
{
	gEngine.resMgr->release(m_mesh);
	gEngine.dynamics->release(m_actor);
}

void DynamicLight::update()
{
	if(m_actor)
	{
		m_actor->getGlobalPose().getColumnMajor44((NxF32*)&m_world);
		m_position = *(Vec3*)&m_actor->getGlobalPosition();
		m_boundingBox.setTransform(m_position);
		m_light->setPos(&m_position);
	}
}

u32 DynamicLight::render()
{
	if(m_mesh && m_shader)
	{
		gEngine.device->getDev()->SetRenderState(D3DRS_ZENABLE,				TRUE);
		gEngine.device->getDev()->SetRenderState(D3DRS_ZWRITEENABLE,		TRUE);
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHABLENDENABLE,	FALSE);

		gEngine.renderer->setShader(m_shader->setTech(m_shader->getTechHandle("LightShape")));
		m_shader->beginPass(0);
		m_shader->setVector("lightShapeColor", m_light->getColor());
		static Mat worldViewProj;
		worldViewProj = m_world * (*gEngine.renderer->getCam()->getViewProjectionMat());
		m_shader->setMatrix(m_shader->h_matWorldViewProj, &worldViewProj);
		m_shader->commitChanges();
		m_mesh->render();
		m_shader->endPass();
		return 1;
	}
	return 0;
}
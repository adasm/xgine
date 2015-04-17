#include "SunEntity.h"

SunEntity::SunEntity(LensFlare* lens)
{
	m_render		= false;
	mesh			= gEngine.resMgr->load<Mesh>("box.x");
	shader		= gEngine.shaderMgr->load("ForwardLight.fx");
	position = Vec3(0, 0, 0);
	rotation = Vec3(0,0,0);
	m_lensFlare		= lens;

	worldMat(&world, position, rotation);
	D3DXMatrixInverse(&invWorld, 0, &world);

	if(mesh)
	{
		boundingBox = (mesh->getBoundingBox());
		boundingBox.setTransform(position);
	}
}

SunEntity::~SunEntity()
{
	gEngine.resMgr->release(mesh);
}

void SunEntity::update(ICamera* cam)
{
	m_render = false;
	D3DXMatrixInverse(&invWorld, 0, &world);
}


u32 SunEntity::renderDeferred() // TEGO NIE TRZEBA IMPLEMENTOWAC
{
	m_render = true;
	return 0;
}

u32 SunEntity::renderForward(Light* light)
{
	m_render = true;
	return 1;
	//return 0;
}

void SunEntity::renderLens()
{
	//gEngine.renderer->r2d->begin();
	////gEngine.device->getDev()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	//m_lensFlare->render(core.game->getWorld()->getScene()->getCamera(), position);
	//gEngine.renderer->r2d->end();
	//gEngine.device->getDev()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
}

u32 SunEntity::renderRaw()
{
	return 0;
}

REND_TYPE SunEntity::getRendType()
{ 
	return REND_DEFERRED; 
}
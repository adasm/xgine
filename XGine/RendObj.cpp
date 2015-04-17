#include "RendObj.h"

RendObj::RendObj(string meshName, Material* _material, Vec3 position, u32 hasShadow)
	: m_hasShadow(hasShadow)
{
	mesh			= gEngine.resMgr->load<Mesh>(meshName);
	material		= _material;
	shader			= _material->m_shader;
	position		= position;
	rotation		= Vec3(0,0,0);

	worldMat(&world, position, rotation);
	D3DXMatrixInverse(&invWorld, 0, &world);

	if(mesh)
	{
		boundingBox = (mesh->getBoundingBox());
		boundingBox.setTransform(position);
	}
}

RendObj::~RendObj()
{
	this->material->cleanup();
	gEngine.resMgr->release(mesh);
}

void RendObj::update(ICamera* cam)
{
}

u32 RendObj::renderDeferred()
{
	if(currentNode && currentNode->mesh && currentNode->material)
	{
		gEngine.renderer->setShader(currentNode->shader->setTech("RenderGBuffer"));
		gEngine.renderer->setMaterial(currentNode->material);
		gEngine.renderer->setWorld(&world);
		gEngine.renderer->commitChanges();
		currentNode->mesh->render();
		return 1;
	}
	return 0;
}

u32 RendObj::renderForward(Light* light)
{
	if(currentNode && currentNode->mesh && currentNode->material && currentNode->shader)
	{
		if(light)
		{
			if(light->getType() == LIGHT_DIR)
				gEngine.renderer->setShader(currentNode->shader->setTech("DirLighting"));
			else if(light->getType() == LIGHT_POINT)
				gEngine.renderer->setShader(currentNode->shader->setTech("PointLighting"));
			else if(light->getType() == LIGHT_SPOT)
				gEngine.renderer->setShader(currentNode->shader->setTech("SpotLighting"));

			gEngine.renderer->setMaterial(currentNode->material);
			gEngine.renderer->setWorld(&currentNode->world);
			gEngine.renderer->setupLight(light);
			gEngine.renderer->commitChanges();

			currentNode->mesh->render();

			if(g_rendObjDebugBB)
				gEngine.renderer->drawBoundingBox(&currentNode->boundingBox, Vec4(1,0,0,1));

			return 1;
		}		
	}
	return 0;
}

u32 RendObj::renderRaw()
{
	if(currentNode && currentNode->mesh)
	{	
		gEngine.renderer->setShader(currentNode->shader->setTech("RenderRaw"));
		gEngine.renderer->setWorld(&currentNode->world);
		gEngine.renderer->commitChanges();
		currentNode->mesh->render();
		return 1;
	}
	return 0;
}

void RendObj::updRotBB(Vec3 rot)
{
	if(mesh)
	{
		Mat world;
		Vec3 Min = boundingBox.Min, Max = boundingBox.Max;
		worldMat(&world, Vec3(0,0,0), rot);
		BoundingBox * newBB = mesh->getBoundingBox(world);
		boundingBox = *newBB;
		boundingBox.setTransform(position);
		delete(newBB);
	}
}

REND_TYPE RendObj::getRendType()
{ 
	return REND_DEFERRED; 
}

Shader* RendObj::getShader()
{
	return material->m_shader;
}

Texture* RendObj::getTexture() { return (currentNode && currentNode->material)?(currentNode->material->m_colorMap):(0); }
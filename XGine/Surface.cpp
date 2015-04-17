#include "Surface.h"

Surface::Surface(string meshName, Material* _material,  Vec3 _position, Vec3 _rotation, f32 _scale, u32 _allowInstancing)
	: Node(), allowInstancing(_allowInstancing)
{
	isSurface			= 1;

	mesh				= gEngine.resMgr->load<Mesh>(meshName);
	material			= _material;
	shader				= _material->m_shader;
	position			= _position;
	scale				= _scale;
	instanceData		= InstanceData(position.x, position.y, position.z, scale);
	instanceDataPacked	= InstanceDataPacked(position.x, position.y, position.z, scale);
	rotation			= _rotation;

	worldMat(&world, position, rotation, scale);
	D3DXMatrixInverse(&invWorld, 0, &world);

	if(mesh)
	{
		boundingBox = (mesh->getBoundingBox());
		boundingBox.setTransform(position, scale);
	}
	else
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Surface", "No mesh (%s) available for surface!", meshName.c_str());
	}
	
	Vec3 minbb = boundingBox.Min;
	Vec3 maxbb = boundingBox.Max;
	boundingBox.reset(minbb, maxbb);
	boundingBox.setTransform(position, scale);

	Vec3 r = (maxbb-minbb)/2;
	sphereRadius = D3DXVec3Length(&r);
	sphereCenter = minbb + r;
	sphereMaxY = maxbb.y;

	if(r_autoGenerateLODs)
		if(mesh->isBaseMesh())
			generateLODs();
}

Surface::Surface(Mesh *meshPtr, Material* _material,  Vec3 _position, Vec3 _rotation, f32 _scale, u32 _allowInstancing)
	: Node(), allowInstancing(_allowInstancing)
{
	isSurface			= 1;

	mesh				= meshPtr;
	material			= _material;
	shader				= _material->m_shader;
	position			= _position;
	scale				= _scale;
	instanceData		= InstanceData(position.x, position.y, position.z, scale);
	instanceDataPacked	= InstanceDataPacked(position.x, position.y, position.z, scale);
	rotation			= _rotation;

	worldMat(&world, position, rotation, scale);
	D3DXMatrixInverse(&invWorld, 0, &world);

	if(mesh)
	{
		boundingBox = (mesh->getBoundingBox());
		boundingBox.setTransform(position, scale);
	}
	else
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Surface", "No mesh (NULL) available for surface!");
	}
	
	Vec3 minbb = boundingBox.CurrentMin;
	Vec3 maxbb = boundingBox.CurrentMax;

	Vec3 r = (maxbb-minbb)/2;
	sphereRadius = D3DXVec3Length(&r);
	sphereCenter = minbb + r;
	sphereMaxY = maxbb.y;


	if(r_autoGenerateLODs)
		if(mesh->isBaseMesh())
			generateLODs();
}

Surface::~Surface()
{
	material->cleanup();
	gEngine.resMgr->release(mesh);
}
void Surface::updRotBB(Vec3 rot)
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

void Surface::generateLODs()
{
	f32 distances[8] = { 0, 20, 40, 60, 80, 500, 600, 2000 };

	if(!nodeLodCount)
	{
		u32 lods = mesh->getNumLODs();
		for(u32 i = 1, ld = 1; i < lods && i < nodeMaxLodCount && ld < 8; i++)
		{ 
			Mesh *m = mesh->loadLOD(i);
			if(m)this->addLod(new Surface(m, material, position, rotation, scale, allowInstancing), distances[ld++]);
		}		
	}
}
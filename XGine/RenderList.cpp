#include "RenderList.h"

/*
const u32 g_maxInstanceVBSize = 5000;

bool lessEntity::operator()(IEntity* a, IEntity* b)
{
	return (a->getShader() < b->getShader());
}

bool lessRenderablePacket::operator()(const RenderablePacket& a, const RenderablePacket& b)
{
	return (a.shader< b.shader);
}

bool sort_lessRenderablePacket(const RenderablePacket& a, const RenderablePacket& b)
{
	return (a.shader< b.shader);
}

GeoPacket::GeoPacket()
{
	ent = 0;
	matrixesMaxSize = 0;
	matrixesCount = 0;
	m_instanceVB = 0;
	m_instanceVBSize = 0;
	ptr_instances = 0;
}

EntityList::~EntityList()
{
	dataSelf.clear();
	for(dataIt it = data.begin(); it != data.end(); it++)
	{
		delete(it->second);
	}
}

void EntityList::clear()
{
	dataSelf.clear();
	for(dataIt it = data.begin(); it != data.end(); it++)
	{
		it->second->matrixesCount = 0;
		if(it->second->ptr_instances && it->second->m_instanceVB)
			it->second->m_instanceVB->Unlock();
		it->second->ptr_instances = 0;
	}
}

void EntityList::sort()
{
	std::sort(&dataSelf[0], &dataSelf[dataSelf.size()-1], sort_lessRenderablePacket); 
}

u32 EntityList::size()
{
	u32 size = dataSelf.size();
	for(dataIt it = data.begin(); it != data.end(); it++)
	{
		size += it->second->matrixesCount;
	}
	return size;
}

void EntityList::add(IEntity *ent) 
{ 
	static dataIt it;
	if(ent)
	{
		if(ent->selfRendering() || !r_geometryInstancing)
			dataSelf.push_back(RenderablePacket(ent->getShader(), ent, 0, 0));
		else
		{
			it = data.find(ent->getMesh());
			if(it != data.end())
			{
				if(!it->second->matrixesCount)
				{ 
					it->second->ent = ent;
					dataSelf.push_back(RenderablePacket(ent->getShader(), ent, 0, 0));
				}

				if(it->second->matrixesCount + 1 < it->second->matrixesMaxSize)
				{
					if(!it->second->ptr_instances)
						if(FAILED(it->second->m_instanceVB->Lock(0, 0, (void**)&it->second->ptr_instances, D3DLOCK_DISCARD)))
							{ it->second->ptr_instances = 0; return; }
				
					it->second->ptr_instances[it->second->matrixesCount++] = ent->getPose()->world;

					///memcpy(&it->second->ptr_instances[it->second->matrixesCount++], &ent->getPose()->world, sizeof(Mat));
				}
				else dataSelf.push_back(RenderablePacket(ent->getShader(), ent, 0, 0));
			}
			else
			{
				GeoPacket *packet = new GeoPacket();
				packet->ent = ent;
					
				packet->matrixesMaxSize = packet->m_instanceVBSize = g_maxInstanceVBSize;
				//MessageBox(0,"XXX","",0);
				if(FAILED( gEngine.device->getDev()->CreateVertexBuffer(packet->m_instanceVBSize * sizeof(Mat), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &packet->m_instanceVB, NULL )))
				{
					gEngine.kernel->log->prnEx("GeoPacket::m_instanceVB: Unable to create instancing vertex buffer!");
					packet->m_instanceVB = 0;
				}


				if(!packet->m_instanceVB)return;
				{
					if(FAILED(packet->m_instanceVB->Lock(0, 0, (void**)&packet->ptr_instances, D3DLOCK_DISCARD)))
						packet->ptr_instances = 0;

					if(!packet->ptr_instances)return;
				
					packet->ptr_instances[0] = ent->getPose()->world;
				}

				data.insert(make_pair(ent->getMesh(), packet));

				dataSelf.push_back(RenderablePacket(ent->getShader(), 0, ent->getMesh(), packet));
			}
		}

		for(u32 i = 0; i < ent->subEntities.size(); i++)
			add(ent->subEntities[i]);

	}
}

void EntityList::add(IEntity* ent, Mesh *mesh, vector< Mat >& matrixes)
{
	u32 matrixesSize = matrixes.size();
	if(mesh && matrixesSize && ent)
	{
		dataIt it = data.find(mesh);
		if(it != data.end())
		{
			if(!it->second->matrixesCount)it->second->ent = ent;

			if(!it->second->m_instanceVB)return;
			if(it->second->matrixesCount + matrixesSize < it->second->matrixesMaxSize)
			{
				if(!it->second->ptr_instances)
					if(FAILED(it->second->m_instanceVB->Lock(0, 0, (void**)&it->second->ptr_instances, D3DLOCK_DISCARD)))
						it->second->ptr_instances = 0;

				if(!it->second->ptr_instances)return;
			
				memcpy(&it->second->ptr_instances[it->second->matrixesCount], &matrixes[0], sizeof(Mat) * matrixesSize);
				it->second->matrixesCount += matrixesSize;
			}
			else
			{
				gEngine.kernel->log->prnEx("GeoPacket: Too small buffer for all matrixes!");
			}
		}
		else
		{
			GeoPacket *packet = new GeoPacket();
			packet->ent = ent;
				
			packet->matrixesMaxSize = packet->m_instanceVBSize = g_maxInstanceVBSize;
			//MessageBox(0,"XXX","",0);
			if(FAILED( gEngine.device->getDev()->CreateVertexBuffer(packet->m_instanceVBSize * sizeof(Mat), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &packet->m_instanceVB, NULL )))
			{
				gEngine.kernel->log->prnEx("GeoPacket::m_instanceVB: Unable to create instancing vertex buffer!");
				packet->m_instanceVB = 0;
			}


			if(!packet->m_instanceVB)return;
			{
				if(FAILED(packet->m_instanceVB->Lock(0, 0, (void**)&packet->ptr_instances, D3DLOCK_DISCARD)))
					packet->ptr_instances = 0;

				if(!packet->ptr_instances)return;
			
				memcpy(&it->second->ptr_instances[0], &matrixes[0], sizeof(Mat) * matrixesSize);
				it->second->matrixesCount = matrixesSize;
			}

			data.insert(make_pair(ent->getMesh(), packet));
		}
	}
}

//void EntityList::add(GeoPacket *geoPacket)
//{ 
//	if(geoPacket->ent->rendEnabled())
//	{
//		dataIt it = data.find(geoPacket->ent->getMesh());
//		if(it != data.end())
//		{
//			memcpy(&it->second->matrixes[it->second->matrixesCount], &geoPacket->matrixes[0], sizeof(Mat)*geoPacket->matrixesCount);
//			it->second->matrixesCount += geoPacket->matrixesCount;
//		}
//		else
//		{
//			GeoPacket *packet = new GeoPacket();
//			packet->ent = geoPacket->ent;
//			packet->matrixesMaxSize = 17000;
//			packet->matrixes = new Mat[packet->matrixesMaxSize];
//			packet->matrixesCount = geoPacket->matrixesCount;
//			memcpy(&it->second->matrixes[0], &geoPacket->matrixes[0], sizeof(Mat)*geoPacket->matrixesCount);
//			data.insert(make_pair(geoPacket->ent->getMesh(), packet));
//		}
//	}
//}


void EntityList::add(vector<IEntity*>& vec) 
{
	for(u32 i = 0; i < vec.size(); i++)
		add(vec[i]);
}
*/
RenderList::RenderList()
{
	surfaceOctree	= new Octree<Surface>();
	terrainOctree	= new Octree<Terrain>();
	entityOctree	= new Octree<IEntity>();
	hqWater			= 0;
	water			= 0;
	m_sunLight		= 0;
}

RenderList::~RenderList()
{
	for(u32 i = 0; i < surfaceVector.size(); i++)
		delete(surfaceVector[i]);
	for(u32 i = 0; i < entityVector.size(); i++)
		delete(entityVector[i]);
	delete(entityOctree);
	delete(terrainOctree);
	delete(surfaceOctree);
	delete(water);
	delete(hqWater);
	clear();
}

void RenderList::add(Surface* entity, u32 dynamic)
{
	if(dynamic == 0)
	{
		surfaceOctree->add(entity);
	}
	else
	{
		surfaceVector.push_back(entity);
	}
}

void RenderList::add(IEntity* entity, u32 dynamic)
{
	if(dynamic == 0)
	{
		entityOctree->add(entity);
	}
	else
	{
		entityVector.push_back(entity);
	}
}

void RenderList::add(Terrain* entity)
{
	terrainOctree->add(entity);
}

void RenderList::add(Light* light)
{
	lights.push_back(light);
}

void RenderList::updateLights(ICamera* camera)
{
	for(u32 i = 0; i < lights.size(); i++)
		lights[i]->update(camera);
}

bool lsort_less(Light* ent1, Light* ent2)
{
	return ent1->getType() < ent2->getType();
}

void RenderList::sortCulledLights()
{
	std::sort(culledLights.begin(), culledLights.end(), lsort_less);
}

void RenderList::clear() 
{ 
	lights.clear(); 
	clearCulled();
}

void RenderList::clearCulled()
{
	
	culledSurfaces.clear();
	culledTerrains.clear();
	culledEntities.clear();
}

void RenderList::clearCulledLights()
{
	culledLights.clear();
}

void RenderList::release()
{ 
	for(u32 i=0; i<lights.size(); i++)		delete ( lights[i] ); 
	clear(); 
}

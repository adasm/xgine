/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

//template< typename T > bool sort_lessShader(const T* a, const T* b)
//{
//	return (a->shader< b->shader);
//}
//
//template< typename T > class XGINE_API lessEntity
//{
//public:
//	bool operator()(T* a, T* b)
//	{
//		return a->shader < b->shader;
//	}
//};

template< typename T > struct XGINE_API EntityList
{
	EntityList() : dataSize(0) { data = 0; }

	inline void add(T *ent) 
	{ 
		if(!ent->rendEnabled())return;

		if(ent->currentNode)ent = (T*)ent->currentNode;

		num++; 
		u32 id = (ent->mesh)?(ent->mesh->getID()):(0);
		ent->next = data[id];
		data[id] = ent;

		if(ent->subElements.size())
		{
			for(u32 i = 0; i<ent->subElements.size(); i++)
			{
				num++; 
				ent->subElements[i]->next = 0; 
				u32 id = (ent->subElements[i]->mesh)?(ent->subElements[i]->mesh->getID()):(0);
				ent->subElements[i]->next = data[id];
				data[id] = ent->subElements[i];
			}
		}
	}

	inline void	sort() { }

	inline void	clear()
	{ 
		num = 0; 
		if(g_numMeshes > dataSize)
		{
			delete(data);
			dataSize = g_numMeshes;
			data = new T*[dataSize];
		}

		for(u32 i = 0; i < dataSize; i++)
			data[i] = 0;
	}
	u32		size() { return num; }

	T** data;
	u32 dataSize;
	u32 num;
};

struct XGINE_API RenderList
{
	RenderList();
	~RenderList();

	Octree< Surface >*		surfaceOctree;
	vector< Surface*>		surfaceVector;
	Octree< IEntity >*		entityOctree;
	vector< IEntity*>		entityVector;
	Octree< Terrain >*		terrainOctree;
	EntityList< Surface >	culledSurfaces;
	EntityList< IEntity >	culledEntities;
	EntityList< Terrain >	culledTerrains;
	HQWater*				hqWater;
	Water*					water;

	void add(Surface* entity, u32 dynamic);
	void add(IEntity* entity, u32 dynamic);
	void add(Terrain* entity);
	void add(Light* light);
	void updateLights(ICamera* camera);
	
	void clear();
	void clearCulled();
	void clearCulledLights();
	void release();


	////////////////////
	Light*				m_sunLight;
	vector<Light*>		lights;
	vector<Light*>		culledLights;

	void sortCulledLights();
};
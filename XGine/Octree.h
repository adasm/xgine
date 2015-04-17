#pragma once
#include "XGine.h"

enum CullState
{
	CS_AllInside,
	CS_AllOutside,
	CS_PartiallyIn
};

template< typename T > struct Octree
{
	Octree();
	~Octree();

	void add(T* entity);
	void remove(T* entity);
	void clear();
	void cull(const ICamera* camera, EntityList<T>* entList);

	void renderBB(Vec4 color);

	inline u32 getNumEntities() { return m_numEntities; }

private:
	typedef std::vector<T*>			tContainer;

	static const float cMinBoxSize;
	BoundingBox	 m_box;
	Octree**	 m_subtrees;
	Octree*		 m_parent;
	tContainer	 m_entities;
	unsigned int m_maxObjects;

	Octree(BoundingBox* bb, int maxObjects, Octree* parent);
	void addObj(T* entity);
	void removeObj(T* entity);
	void enlargeTree(BoundingBox* bb);
	void divideTree();
	void enqueueRenderAll(EntityList<T>* entList);
	void addLightAll(Light* light);
	inline CullState checkTreeSphere(const D3DXPLANE* frustumPlanes, int numPlanes = 6);

	u32	m_numEntities;
};

template< typename T > const float Octree<T>::cMinBoxSize = 1.0f;

template< typename T > Octree<T>::Octree()
: m_subtrees(0), m_parent(0), m_maxObjects(10), m_box(Vec3(-5,-5,-5), Vec3(5,5,5)), m_numEntities(0)
{ }

template< typename T > Octree<T>::Octree(BoundingBox* bb, int maxObjects, Octree* parent)
: m_subtrees(0), m_parent(parent), m_maxObjects(maxObjects), m_box(*bb)
{ }

template< typename T > Octree<T>::~Octree()
{
	if(m_subtrees)
	{
		for(int i = 0; i < 8; ++i)
			delete m_subtrees[i];
		delete[] m_subtrees;
	}

	for(u32 i = 0; i < m_entities.size(); i++)
	{
		delete m_entities[i];
	}

	m_entities.clear();
}

template< typename T > void Octree<T>::add(T *entity)
{
	if(!m_parent && !m_box.contain(&entity->boundingBox))
	{
		enlargeTree(&entity->boundingBox);
		add(entity);
	}
	else
	{
		addObj(entity);
	}
}

template< typename T > void Octree<T>::remove(T *entity)
{
	if(entity)
	{
		removeObj(entity);
	}
}

template< typename T > void Octree<T>::removeObj(T *entity)
{
	if(m_subtrees)
	{
		for(int i = 0; i < 8; ++i)
		{
			if(m_subtrees[i]->m_box.contain(&entity->getPose()->boundingBox))
			{
				m_subtrees[i]->removeObj<T>(entity);
				return;
			}
		}
	}

	for(std::vector<T*>::iterator it = m_entities.begin(); it != m_entities.end(); ++it)
	{
		if(*it == entity)
		{
			delete *it;
			m_entities.erase(it);
			m_numEntities--;
			return;
		}
	}
}

template< typename T > void Octree<T>::clear()
{
	if(m_subtrees)
	{
		for(int i = 0; i < 8; ++i)
			delete m_subtrees[i];
		delete[] m_subtrees;
		m_subtrees = 0;
	}

	for(u32 i = 0; i < m_entities.size(); i++)
	{
		delete m_entities[i];
	}

	m_entities.clear();
}

template< typename T > void Octree<T>::cull(const ICamera* camera, EntityList<T>* entList)
{
	//quick tests
	if(!m_subtrees && (m_entities.size() == 0))
		return;

	const BoundingSphere*	frustumSphere	= camera->getFrustumSphere();
	const Plane*			frustumPlanes	= camera->getFrustumPlanes();

	if(!frustumSphere->intersect(&m_box.BSphere))
		return;

	if(!m_box.BSphere.intersect(frustumPlanes))
		return;
	
	//slow tests
	CullState state = checkTreeSphere(camera->getFrustumPlanes());
	if(state == CS_AllInside)
		enqueueRenderAll(entList);
	else if(state == CS_PartiallyIn)
	{
		if(m_subtrees)
		{
			for(int i = 0; i < 8; ++i)
				m_subtrees[i]->cull(camera, entList);
		}

		if(m_entities.size())
		{
			for(u32 i = 0; i < m_entities.size(); i++)
			{
				T* ent = m_entities[i];
					if(ent->boundingBox.BSphere.intersect(frustumSphere))
						if(ent->boundingBox.BSphere.intersect(frustumPlanes))
							entList->add(ent);
			}
		}
	}
}


template< typename T > void Octree<T>::renderBB(Vec4 color)
{
	if(!gEngine.renderer->get().camera->getFrustumSphere()->intersect(m_box.getBoundingSphere()))
		return;

    if(!gEngine.renderer->get().camera->getFrustumCone()->intersect(m_box.getBoundingSphere()))
		return;

	if(m_subtrees)
	{
		for(int i = 0; i < 8; ++i)
			m_subtrees[i]->renderBB(color);
	}

	gEngine.renderer->drawBoundingBox(&m_box, color);
}

template< typename T > void Octree<T>::addObj(T *entity)
{
	m_numEntities++;

	if(m_subtrees)
	{
		for(int i = 0; i < 8; ++i)
		{
			if(m_subtrees[i]->m_box.contain(&entity->boundingBox))
			{
				m_subtrees[i]->addObj(entity);
				return;
			}
		}
	}

	m_entities.push_back(entity);

	if(!m_subtrees && m_entities.size() > m_maxObjects && m_box.getWidth() > cMinBoxSize)
		divideTree();
}

template< typename T > void Octree<T>::enlargeTree(BoundingBox *bb)
{
	//store old values
	Octree**	subtrees = m_subtrees;
	tContainer  entities = m_entities;
	D3DXVECTOR3 center = m_box.getCenter();

	//expand box
	D3DXVECTOR3 bbCenter(bb->getCenter());
	D3DXVECTOR3 min = m_box.Min;
	D3DXVECTOR3 max = m_box.Max;
	float expand = m_box.getWidth(); //double its size
	if(bbCenter.x < center.x) min.x -= expand;
	else max.x += expand;
	if(bbCenter.y < center.y) min.y -= expand;
	else max.y += expand;
	if(bbCenter.z < center.z) min.z -= expand;
	else max.z += expand;
	m_box.reset(min, max);

	//divide the new tree
	m_subtrees = 0;
	m_entities.clear();
	divideTree();

	//copy stored values to new subtree
	m_subtrees[m_box.getOctreeSubBoxIndex(center)]->m_subtrees	= subtrees;
	m_subtrees[m_box.getOctreeSubBoxIndex(center)]->m_entities	= entities;
}

template< typename T > void Octree<T>::divideTree()
{
	m_subtrees = new Octree*[8];
	for(int i = 0; i < 8; i++)
		m_subtrees[i] = new Octree(&m_box.getOctreeSubBox(i), m_maxObjects, this);

	if(m_entities.size())
	{
		tContainer oldEntites = m_entities;
		m_entities.clear();
		for(u32 i = 0; i < oldEntites.size(); i++)
			addObj(oldEntites[i]);
	}
}

template< typename T > void Octree<T>::enqueueRenderAll(EntityList<T>* entList)
{
	if(m_subtrees)
		for(int i = 0; i < 8; ++i)
			m_subtrees[i]->enqueueRenderAll(entList);
	if(m_entities.size())
	{
		for(u32 i = 0; i < m_entities.size(); i++)
			entList->add(m_entities[i]);
	}

}

template< typename T > CullState Octree<T>::checkTreeSphere(const D3DXPLANE* frustumPlanes, int numPlanes)
{
	static f32 distance;
	static i32 count;

			count		= 0;
	Vec3&	point		= m_box.BSphere.Center;
	f32&	radius		= m_box.BSphere.Radius;

	for(u32 i = 0; i < numPlanes; i++)
	{
		distance = frustumPlanes[i].a * point.x + frustumPlanes[i].b * point.y + frustumPlanes[i].c * point.z + frustumPlanes[i].d;
		if(distance <= -radius )
			return CS_AllOutside;
		if(distance > radius)
			count++;
	}

	if(count == numPlanes)
		return CS_AllInside;
	return CS_PartiallyIn;
}

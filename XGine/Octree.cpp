#include "Octree.h"

const float Octree::cMinBoxSize = 1.0f;
float	g_rendDistance = 500.0f;


Octree::Octree()
: m_subtrees(0), m_parent(0), m_entities(0), m_maxObjects(10), m_box(Vec3(-5,-5,-5), Vec3(5,5,5)), m_numEntities(0)
{ }

Octree::Octree(BoundingBox* bb, int maxObjects, Octree* parent)
: m_subtrees(0), m_parent(parent), m_entities(0), m_maxObjects(maxObjects), m_box(*bb)
{ }

Octree::~Octree()
{
	if(m_subtrees)
	{
		for(int i = 0; i < 8; ++i)
			delete m_subtrees[i];
		delete[] m_subtrees;
	}
	if(m_entities)
	{
		for(tIterator it = m_entities->begin(); it != m_entities->end(); ++it)
			delete *it;
		delete m_entities;
	}
}

void Octree::add(IEntity *entity)
{
	m_numEntities++;

	if(!m_parent && !m_box.contain(&entity->getPose()->boundingBox))
	{
		enlargeTree(&entity->getPose()->boundingBox);
		add(entity);
	}
	else
		addObj(entity);
}

void Octree::remove(IEntity *entity)
{
	m_numEntities--;

	if(entity)
		removeObj(entity);
}

void Octree::removeObj(IEntity *entity)
{
	if(m_subtrees)
	{
		for(int i = 0; i < 8; ++i)
			if(m_subtrees[i]->m_box.contain(&entity->getPose()->boundingBox))
			{
				m_subtrees[i]->removeObj(entity);
				return;
			}
	}
	if(m_entities)
	{
		for(tIterator it = m_entities->begin(); it != m_entities->end(); ++it)
			if(*it == entity)
			{
				delete *it;
				m_entities->erase(it);
				return;
			}
	}
}

void Octree::clear()
{
	if(m_subtrees)
	{
		for(int i = 0; i < 8; ++i)
			delete m_subtrees[i];
		delete[] m_subtrees;
		m_subtrees = 0;
	}
	if(m_entities)
	{
		for(tIterator it = m_entities->begin(); it != m_entities->end(); ++it)
			delete *it;
		delete m_entities;
		m_entities = 0;
	}
}

void Octree::cull(const ICamera* camera, EntityList* entList)
{
	//quick tests
	if(!m_subtrees && (!m_entities || m_entities->size() == 0))
		return;

	if(!camera->getFrustumSphere()->intersect(m_box.getBoundingSphere()))
		return;

	if(!camera->getFrustumCone()->intersect(m_box.getBoundingSphere()))
		return;

	if(D3DXVec3Length( &Vec3(m_box.BSphere.Center - *camera->getPosition()) ) - m_box.BSphere.Radius > g_rendDistance)
		return;

	//slow tests
	CullState state = checkTreeSphere(camera->getFrustumPlanes());
	if(state == CS_AllInside)
		enqueueRenderAll(entList);
	else if(state == CS_PartiallyIn)
	{
		if(m_subtrees)
			for(int i = 0; i < 8; ++i)
				m_subtrees[i]->cull(camera, entList);
		if(m_entities)
		{
			for(tIterator it = m_entities->begin(); it != m_entities->end(); ++it)
			{
				if(!(*it)->rendEnabled())continue;

				if(!(*it)->getPose()->boundingBox.BSphere.intersect(camera->getFrustumSphere()))
					continue;
				else if(!(*it)->getPose()->boundingBox.BSphere.intersect(camera->getFrustumPlanes()))
					continue;

				entList->add( (*it) );	
			}
		}
	}
}


void Octree::renderBB()
{
	if(m_subtrees)
	{
		for(int i = 0; i < 8; ++i)
			m_subtrees[i]->renderBB();
	}

	gEngine.renderer->drawBoundingBox(&m_box, Vec4(1,0.2,0.2,1));
}

void Octree::addObj(IEntity *entity)
{
	if(m_subtrees)
	{
		for(int i = 0; i < 8; ++i)
			if(m_subtrees[i]->m_box.contain(&entity->getPose()->boundingBox))
			{
				m_subtrees[i]->addObj(entity);
				return;
			}
	}
	if(!m_entities)
		m_entities = new tContainer();
	m_entities->push_back(entity);
	if(!m_subtrees && m_entities->size() > m_maxObjects && m_box.getWidth() > cMinBoxSize)
		divideTree();
}

void Octree::enlargeTree(BoundingBox *bb)
{
	//store old values
	Octree**	subtrees = m_subtrees;
	tContainer* entities = m_entities;
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
	m_entities = 0;
	divideTree();

	//copy stored values to new subtree
	m_subtrees[m_box.getOctreeSubBoxIndex(center)]->m_subtrees = subtrees;
	m_subtrees[m_box.getOctreeSubBoxIndex(center)]->m_entities = entities;
}

void Octree::divideTree()
{
	m_subtrees = new Octree_ptr[8];
	for(int i = 0; i < 8; i++)
		m_subtrees[i] = new Octree(&m_box.getOctreeSubBox(i), m_maxObjects, this);

	if(m_entities)
	{
		tContainer* oldEntites = m_entities;
		m_entities = 0;
		for(tIterator it = oldEntites->begin(); it != oldEntites->end(); ++it)
			addObj(*it);
	}
}

void Octree::enqueueRenderAll(EntityList* entList)
{
	if(m_subtrees)
		for(int i = 0; i < 8; ++i)
			m_subtrees[i]->enqueueRenderAll(entList);
	if(m_entities)
	{
		for(tIterator it = m_entities->begin(); it != m_entities->end(); ++it)
				if((*it)->rendEnabled())
					entList->add((*it));
	}

}

Octree::CullState Octree::checkTreeSphere(const D3DXPLANE* frustumPlanes, int numPlanes)
{
	float distance = 0;
	int count = 0;
	D3DXVECTOR3& point = m_box.getBoundingSphere()->Center;
	float& radius = m_box.getBoundingSphere()->Radius;

	for(int i = 0; i < numPlanes; i++)
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
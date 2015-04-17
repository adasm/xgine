#include "Quadtree.h"
/*
const float Quadtree::cMinBoxSize = 4.0f;

Quadtree::Quadtree()
: m_subtrees(0), m_parent(0), m_entities(0), m_maxObjects(10), m_box(Vec3(-5,-100,-5), Vec3(9000,500,9000)), m_numEntities(0)
{ }

Quadtree::Quadtree(BoundingBox* bb, int maxObjects, Quadtree* parent)
: m_subtrees(0), m_parent(parent), m_entities(0), m_maxObjects(maxObjects), m_box(*bb)
{ }

Quadtree::~Quadtree()
{
	if(m_subtrees)
	{
		for(int i = 0; i < 4; ++i)
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

void Quadtree::add(IEntity *entity)
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

void Quadtree::remove(IEntity *entity)
{
	m_numEntities--;

	if(entity)
		removeObj(entity);
}

void Quadtree::removeObj(IEntity *entity)
{
	if(m_subtrees)
	{
		for(int i = 0; i < 4; ++i)
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

void Quadtree::clear()
{
	if(m_subtrees)
	{
		for(int i = 0; i < 4; ++i)
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

void Quadtree::cull(const ICamera* camera, RenderList *in)
{
	//quick tests
	if(!m_subtrees && (!m_entities || m_entities->size() == 0))
		return;

	if(!camera->getFrustumSphere()->intersect(m_box.getBoundingSphere()))
		return;

	if(!camera->getFrustumCone()->intersect(m_box.getBoundingSphere()))
		return;

	//slow tests
	CullState state = checkTreeSphere(camera->getFrustumPlanes());
	if(state == CS_AllInside)
		enqueueRenderAll(in);
	else if(state == CS_PartiallyIn)
	{
		if(m_subtrees)
			for(int i = 0; i < 4; ++i)
				m_subtrees[i]->cull(camera, in);
		if(m_entities)
			for(tIterator it = m_entities->begin(); it != m_entities->end(); ++it)
			{
				if(!(*it)->getPose()->boundingBox.BSphere.intersect(camera->getFrustumSphere()))
					continue;
				else if(!(*it)->getPose()->boundingBox.BSphere.intersect(camera->getFrustumPlanes()))
					continue;

				in->addCulled( (*it) );	
			}
	}
}

void Quadtree::addObj(IEntity *entity)
{
	if(m_subtrees)
	{
		for(int i = 0; i < 4; ++i)
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

void Quadtree::enlargeTree(BoundingBox *bb)
{
	return;
	//store old values
	Quadtree**	subtrees = m_subtrees;
	tContainer* entities = m_entities;
	D3DXVECTOR3 center = m_box.getCenter();

	//expand box
	D3DXVECTOR3 bbCenter(bb->getCenter());
	D3DXVECTOR3 min = m_box.Min;
	D3DXVECTOR3 max = m_box.Max;
	float expand = m_box.getWidth(); //double its size
	if(bbCenter.x < center.x) min.x -= expand;
	else max.x += expand;
	if(bbCenter.z < center.z) min.z -= expand;
	else max.z += expand;
	m_box.reset(min, max);

	//divide the new tree
	m_subtrees = 0;
	m_entities = 0;
	divideTree();

	//copy stored values to new subtree
	m_subtrees[m_box.getQuadtreeSubBoxIndex(center)]->m_subtrees = subtrees;
	m_subtrees[m_box.getQuadtreeSubBoxIndex(center)]->m_entities = entities;
}

void Quadtree::divideTree()
{
	m_subtrees = new Quadtree_ptr[4];
	for(int i = 0; i < 4; i++)
		m_subtrees[i] = new Quadtree(&m_box.getOctreeSubBox(i), m_maxObjects, this);

	if(m_entities)
	{
		tContainer* oldEntites = m_entities;
		m_entities = 0;
		for(tIterator it = oldEntites->begin(); it != oldEntites->end(); ++it)
			addObj(*it);
	}
}

void Quadtree::enqueueRenderAll(RenderList *in)
{
	if(m_subtrees)
		for(int i = 0; i < 4; ++i)
			m_subtrees[i]->enqueueRenderAll(in);
	if(m_entities)
		for(tIterator it = m_entities->begin(); it != m_entities->end(); ++it)
				in->addCulled((*it));
}

Quadtree::CullState Quadtree::checkTreeSphere(const D3DXPLANE* frustumPlanes, int numPlanes)
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
*/
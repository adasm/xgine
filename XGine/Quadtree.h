#pragma once
#include "XGine.h"
/*
typedef struct XGINE_API Quadtree
{
	enum CullState
	{
		CS_AllInside,
		CS_AllOutside,
		CS_PartiallyIn
	};

	Quadtree();
	~Quadtree();

	void add(IEntity* entity);
	void remove(IEntity* entity);
	void clear();
	void cull(const ICamera* camera, RenderList *in);

	u32 getNumEntities() { return m_numEntities; }

private:
	typedef std::vector<IEntity*>	tContainer;
	typedef tContainer::iterator	tIterator;

	static const float cMinBoxSize;
	BoundingBox	 m_box;
	Quadtree**	 m_subtrees;
	Quadtree*	 m_parent;
	tContainer*	 m_entities;
	unsigned int m_maxObjects;

	Quadtree(BoundingBox* bb, int maxObjects, Quadtree* parent);
	void addObj(IEntity* entity);
	void removeObj(IEntity* entity);
	void enlargeTree(BoundingBox* bb);
	void divideTree();
	void enqueueRenderAll(RenderList *in);
	void addLightAll(Light* light);
	CullState checkTreeSphere(const D3DXPLANE* frustumPlanes, int numPlanes = 6);

	u32	m_numEntities;
} *Quadtree_ptr;
*/
#pragma once
#include "XGine.h"

struct INode
{
	INode() { isSurface = 0; }

	Mesh				*mesh;
	Shader				*shader;
	Material			*material;
	Mat					world;
	Mat					invWorld;
	Vec3				position;
	Vec3				rotation;
	BoundingBox			boundingBox;

	void				*ptr_userData;
	u32					isSurface;
};

template<typename T> struct Node : INode
{
	T	*prev, *next;
	u32 num;

	Node() : INode() { prev = next = 0; ptr_userData = 0; currentNode = this; nodeLodCount = 0; nodeMaxLodCount = 10; }

	virtual ~Node()
	{
		for(u32 i=0; i<nodeLodCount; i++)
			delete(nodeLod[i]);
		nodeLodCount = 0;
		for(u32 i = 0; i < subElements.size(); i++)
			delete(subElements[i]);
		subElements.clear();
	}

	void addSub(T* ent)
	{
		if(!ent)return;

		Vec3 Min = ent->boundingBox.CurrentMin;
		Vec3 Max = ent->boundingBox.CurrentMax;

		if(Min.x < boundingBox.CurrentMin.x) boundingBox.CurrentMin.x = boundingBox.CurrentMin.x;
		if(Min.y < boundingBox.CurrentMin.y) boundingBox.CurrentMin.y = boundingBox.CurrentMin.y;
		if(Min.z < boundingBox.CurrentMin.z) boundingBox.CurrentMin.z = boundingBox.CurrentMin.z;

		if(Max.x > boundingBox.CurrentMax.x) boundingBox.CurrentMax.x = boundingBox.CurrentMax.x;
		if(Max.y > boundingBox.CurrentMax.y) boundingBox.CurrentMax.y = boundingBox.CurrentMax.y;
		if(Max.z > boundingBox.CurrentMax.z) boundingBox.CurrentMax.z = boundingBox.CurrentMax.z;

		subElements.push_back(ent);
	}

	vector< T* >	subElements;	

	__forceinline u32 rendEnabled()
	{
		currentNode = this;
		if(!r_enableLODs)return 1;

		static ICamera* cam;
		static u32 i;
		static f32 dist;
		cam = gEngine.renderer->get().camera;
		if(!cam)return 1;
		dist = D3DXVec3Length(&Vec3(boundingBox.getCenter() - *cam->getPosition()));
		for(i=0; i<nodeLodCount; i++)
			if(dist > nodeLodStartDist[i])currentNode = nodeLod[i];
		return 1;
	}

	inline void addLod(Node* node, f32 startDist)
	{
		if(nodeLodCount + 1 < nodeMaxLodCount)
		{
			nodeLod[nodeLodCount] = node;
			nodeLodStartDist[nodeLodCount++] = startDist;
		}
	}

	Node*				currentNode;
	Node*				nodeLod[10];
	f32					nodeLodStartDist[10];
	u32					nodeLodCount;
	u32					nodeMaxLodCount;
};
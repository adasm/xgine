#pragma once
#include "Headers.h"

enum TrafficNodeType
{
	TNT_ONE_WAY,
	TNT_TWO_WAYS,
};

typedef struct TrafficNode
{
	vector<TrafficNode*> next;
	vector<TrafficNode*> previous;
	TrafficNodeType nodeType;

	Vec3 position;
	TrafficNode()
	{
		nodeType = TNT_TWO_WAYS;
		position = Vec3(0, 0, 0);
	}
}*TrafficNodePTR;

class NavigationMesh
{
public:
	NavigationMesh();

	TrafficNode*	getNearestNode(Vec3 pos);
protected:
	TrafficNode*	m_nodes;
	int				m_nodesNumber;
};
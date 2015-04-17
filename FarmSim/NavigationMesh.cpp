#include "NavigationMesh.h"

NavigationMesh::NavigationMesh()
{
	m_nodesNumber = 17;
	m_nodes = new TrafficNode[m_nodesNumber];
/*	m_nodes[0].position = Vec3(273, 0, 127);
	m_nodes[1].position = Vec3(273, 0, 137);
	m_nodes[2].position = Vec3(263, 0, 137);
	m_nodes[3].position = Vec3(263, 0, 127);*/
	m_nodes[0].position = Vec3(273, 0, 127);
	m_nodes[1].position = Vec3(279, 0, 146);
	m_nodes[2].position = Vec3(294, 0, 144);
	m_nodes[3].position = Vec3(297, 0, 130);
	m_nodes[4].position = Vec3(309, 0, 128);
	m_nodes[5].position = Vec3(310, 0, 140);
	m_nodes[6].position = Vec3(327, 0, 132);
	m_nodes[7].position = Vec3(330, 0, 120);
	m_nodes[8].position = Vec3(337, 0, 131);
	m_nodes[9].position = Vec3(347, 0, 125);
	m_nodes[10].position = Vec3(340, 0, 115);
	m_nodes[11].position = Vec3(363, 0, 114);
	m_nodes[12].position = Vec3(360, 0, 160);
	m_nodes[13].position = Vec3(300, 0, 185);
	m_nodes[14].position = Vec3(200, 0, 145);
	m_nodes[15].position = Vec3(215, 0, 100);
	m_nodes[16].position = Vec3(260, 0, 133);

	for(int i = 0; i < m_nodesNumber; i++)
	{
		m_nodes[i].position *= 16;
		m_nodes[i].position.y = 0;// core.game->getWorld()->getGrassManager()->getTerrainHeight(m_nodes[i].position.x, m_nodes[i].position.z);
	}
	m_nodes[0].next.push_back(&m_nodes[1]);
	m_nodes[0].previous.push_back(&m_nodes[m_nodesNumber-1]);

	m_nodes[m_nodesNumber-1].next.push_back(&m_nodes[0]);
	m_nodes[m_nodesNumber-1].previous.push_back(&m_nodes[m_nodesNumber-2]);
	for(int i = 1; i < m_nodesNumber-1; i++)
	{
		m_nodes[i].next.push_back(&m_nodes[i+1]);
		m_nodes[i].previous.push_back(&m_nodes[i-1]);
	}
	for(int i = 0; i < m_nodesNumber; i++)
	{
		gEngine.kernel->log->prnEx(LT_INFO, "NavigationMesh", "Node '%d' pos: %f %f %f", i, m_nodes[i].position.x, m_nodes[i].position.y, m_nodes[i].position.z);
	}
}

TrafficNode* NavigationMesh::getNearestNode(Vec3 pos)
{
	float nearest = 9999999999999999;
	float temp;
	int nearestNodeIndex = -1;
	for(int i = 0; i < m_nodesNumber; i++)
	{
		temp = D3DXVec3Length(&Vec3(m_nodes[i].position - pos));
		if(temp < nearest)
		{
			nearest = temp;
			nearestNodeIndex = i;
		}
	}
	return &m_nodes[nearestNodeIndex];
}
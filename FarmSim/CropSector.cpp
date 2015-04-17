#include "CropSector.h"

CropSector::CropSector(int cropID, Vec3 WorldPos, int posX, int posY,
					   GrassMesh* youngSector, GrassMesh* adultSector, GrassMesh* oldSector, GrassMesh* deadSector)
{
	m_toUpdate = false;

	m_posInMapX = posX;
	m_posInMapY = posY;

	m_youngPos = NULL;
	m_adultPos = NULL;
	m_oldPos = NULL;
	m_deadPos = NULL;

	m_youngNum = 0;
	m_adultNum = 0;
	m_oldNum = 0;
	m_deadNum = 0;

	m_cropID = cropID;
	m_worldPos = WorldPos;

	m_young = youngSector;
	m_adult = adultSector;
	m_old = oldSector;
	m_dead = deadSector;
	m_cropTypeInfo = NULL;
	if(cropID != -1)
		m_cropTypeInfo = core.game->getWorld()->getGrassManager()->getCropTypeInfo(m_cropID);

	generatePositions();
	//update();
	m_toUpdate = true;
}

void CropSector::generatePositions()
{
	m_youngNum = 0;
	m_adultNum = 0;
	m_oldNum = 0;
	m_deadNum = 0;

	vector<Field*> *map = core.game->getWorld()->getGrassManager()->getFieldMap();
	int mapSize = core.game->getWorld()->getGrassManager()->getMapSize();
	for(int y = m_posInMapY; y < m_posInMapY + g_grassSectorSize; y++)
	{
		for(int x = m_posInMapX; x < m_posInMapX + g_grassSectorSize; x++)
		{
			if((*map)[y * mapSize + x]->cropId == m_cropID)
			{
				if((*map)[y * mapSize + x]->stadium == 1)
					m_youngNum++;
				if((*map)[y * mapSize + x]->stadium == 2)
					m_adultNum++;
				if((*map)[y * mapSize + x]->stadium == 3)
					m_oldNum++;
				if((*map)[y * mapSize + x]->stadium == 4)
					m_deadNum++;
			}
		}
	}


	//gEngine.kernel->mem->alloc(
	/*if(m_youngNum)
	{
		if(!m_youngPos)
			m_youngPos = new Vec3[m_youngNum];
		else
			m_youngPos = (Vec3*)realloc((void*)m_youngPos, sizeof(Vec3) * m_youngNum);
	}

	if(m_adultNum)
	{
		if(!m_adultPos)
			m_adultPos = new Vec3[m_adultNum];
		else
			m_adultPos = (Vec3*)realloc((void*)m_adultPos, sizeof(Vec3) * m_adultNum);
	}

	if(m_oldNum)
	{
		if(!m_oldPos)
			m_oldPos = new Vec3[m_oldNum];
		else
			m_oldPos = (Vec3*)realloc((void*)m_oldPos, sizeof(Vec3) * m_oldNum);
	}

	if(m_deadNum)
	{
		if(!m_deadPos)
			m_deadPos = new Vec3[m_deadNum];
		else
			m_deadPos = (Vec3*)realloc((void*)m_deadPos, sizeof(Vec3) * m_deadNum);
	}*/

	if(m_youngNum)
	{
		if(!m_youngPos)
			m_youngPos = (Vec3*)gEngine.kernel->mem->alloc(sizeof(Vec3) * m_youngNum, "CropSectora.cpp", 101);
		else
		{
			gEngine.kernel->mem->free((void*)m_youngPos);
			m_youngPos = (Vec3*)gEngine.kernel->mem->alloc(sizeof(Vec3) * m_youngNum, "CropSectora.cpp", 105);
		}
	}

	if(m_adultNum)
	{
		if(!m_adultPos)
			m_adultPos = (Vec3*)gEngine.kernel->mem->alloc(sizeof(Vec3) * m_adultNum, "CropSectora.cpp", 112);
		else
		{
			gEngine.kernel->mem->free((void*)m_adultPos);
			m_adultPos = (Vec3*)gEngine.kernel->mem->alloc(sizeof(Vec3) * m_adultNum, "CropSectora.cpp", 116);
		}
	}

	if(m_oldNum)
	{
		if(!m_oldPos)
			m_oldPos = (Vec3*)gEngine.kernel->mem->alloc(sizeof(Vec3) * m_oldNum, "CropSectora.cpp", 123);
		else
		{
			gEngine.kernel->mem->free((void*)m_oldPos);
			m_oldPos = (Vec3*)gEngine.kernel->mem->alloc(sizeof(Vec3) * m_oldNum, "CropSectora.cpp", 127);
		}
	}

	if(m_deadNum)
	{
		if(!m_deadPos)
			m_deadPos = (Vec3*)gEngine.kernel->mem->alloc(sizeof(Vec3) * m_deadNum, "CropSectora.cpp", 134);
		else
		{
			gEngine.kernel->mem->free((void*)m_deadPos);
			m_deadPos = (Vec3*)gEngine.kernel->mem->alloc(sizeof(Vec3) * m_deadNum, "CropSectora.cpp", 138);
		}
	}

	m_youngNum = 0;
	m_adultNum = 0;
	m_oldNum = 0;
	m_deadNum = 0;

	Vec3 *terrainVertices = core.game->getWorld()->getTerrainManager()->m_vertices;
	int terrainWidth = core.game->getWorld()->getTerrainManager()->m_size;
	time_t t;
	srand((unsigned) time(&t));

	for(int y = m_posInMapY; y < m_posInMapY + g_grassSectorSize; y++)
	{
		for(int x = m_posInMapX; x < m_posInMapX + g_grassSectorSize; x++)
		{
			if((*map)[y * mapSize + x]->cropId == m_cropID)
			{

				Vec3 resultPos;
				int modulo = g_grassTileSize*50;
				if((*map)[y * mapSize + x]->stadium > 0)
				{
					D3DXPLANE plane;

					Vec3 p1 = terrainVertices[(int)(y*g_grassTileSize*0.5f)*terrainWidth+(int)(x*g_grassTileSize*0.5f)];
					Vec3 p2 = terrainVertices[(int)((y*g_grassTileSize*0.5f)+1)*terrainWidth+(int)(x*g_grassTileSize*0.5f)];
					Vec3 p3 = terrainVertices[(int)(y*g_grassTileSize*0.5f)*terrainWidth+(int)(x*g_grassTileSize*0.5f)+1];

					D3DXPlaneFromPoints(&plane, &p1, &p2, &p3);
					Vec3 upPoint = Vec3((x)*g_grassTileSize, 1000, (y)*g_grassTileSize);
					Vec3 downPoint = Vec3((x)*g_grassTileSize, -1000, (y)*g_grassTileSize);

					D3DXPlaneIntersectLine(&resultPos, &plane, &upPoint, &downPoint);

					resultPos.x = x*g_grassTileSize + (rand()%modulo)/100.0f;
					resultPos.z = y*g_grassTileSize + (rand()%modulo)/100.0f;

					resultPos -= m_worldPos;
				}

				if((*map)[y * mapSize + x]->stadium == 1)
				{
					m_youngPos[m_youngNum] = resultPos;
					m_youngNum++;
				}
				else
				if((*map)[y * mapSize + x]->stadium == 2)
				{
					m_adultPos[m_adultNum] = resultPos;
					m_adultNum++;
				}
				else
				if((*map)[y * mapSize + x]->stadium == 3)
				{
					m_oldPos[m_oldNum] = resultPos;
					m_oldNum++;
				}
				else
				if((*map)[y * mapSize + x]->stadium == 4)
				{
					m_deadPos[m_deadNum] = resultPos;
					m_deadNum++;
				}
			}
		}
	}
}


void CropSector::harvest(ActionBox *abox)
{
	if(!abox)
		return;
	int deletedY = 0;
	int deletedA = 0;
	int deletedO = 0;
	int deletedD = 0;
	bool young = false;
	bool adult = false;
	bool old = false;
	bool dead = false;
	int biggestSize = -1;
	if(m_youngNum > biggestSize)
		biggestSize = m_youngNum;
	if(m_adultNum > biggestSize)
		biggestSize = m_adultNum;
	if(m_oldNum > biggestSize)
		biggestSize = m_oldNum;
	if(m_deadNum > biggestSize)
		biggestSize = m_deadNum;

	for(int i = 0; i < biggestSize; i++)
	{
		if(i <= m_youngNum)
		if(abox->isPointInside(&Vec3(m_youngPos[i] + m_worldPos)))
		{
			m_youngPos[i] = m_youngPos[m_youngNum - deletedY];
			deletedY++;
			young = true;
		}

		if(i <= m_adultNum)
		if(abox->isPointInside(&Vec3(m_adultPos[i] + m_worldPos)))
		{
			m_adultPos[i] = m_adultPos[m_adultNum - deletedA];
			deletedA++;
			adult = true;
		}

		if(i <= m_oldNum)
		if(abox->isPointInside(&Vec3(m_oldPos[i] + m_worldPos)))
		{
			m_oldPos[i] = m_oldPos[m_oldNum - deletedO];
			deletedO++;
			old = true;
		}

		if(i <= m_deadNum)
		if(abox->isPointInside(&Vec3(m_deadPos[i] + m_worldPos)))
		{
			m_deadPos[i] = m_deadPos[m_deadNum - deletedD];
			deletedD++;
			dead = true;
		}
	}

	if(young)
	{
		m_youngNum -= deletedY;
		m_youngPos = (Vec3*)realloc((void*)m_youngPos, sizeof(Vec3) * m_youngNum);
	}

	if(adult)
	{
		m_adultNum -= deletedA;
		m_adultPos = (Vec3*)realloc((void*)m_adultPos, sizeof(Vec3) * m_adultNum);
	}

	if(old)
	{
		m_oldNum -= deletedO;
		m_oldPos = (Vec3*)realloc((void*)m_oldPos, sizeof(Vec3) * m_oldNum);
	}

	if(dead)
	{
		m_deadNum -= deletedD;
		m_deadPos = (Vec3*)realloc((void*)m_deadPos, sizeof(Vec3) * m_deadNum);
	}
	update(young, adult, old, dead);
}

void CropSector::update(bool young, bool adult, bool old, bool dead)
{
	if(young)
		m_young->reassign(m_youngPos, m_cropTypeInfo->grassBladeDimmYoung, 2, m_youngNum);
	if(adult)
		m_adult->reassign(m_adultPos, m_cropTypeInfo->grassBladeDimmAdult, 2, m_adultNum);
	if(old)
		m_old->reassign(m_oldPos, m_cropTypeInfo->grassBladeDimmOld, 2, m_oldNum);
	if(dead)
		m_dead->reassign(m_deadPos, m_cropTypeInfo->grassBladeDimmDead, 2, m_deadNum);
	m_toUpdate = false;

	/*if(young)
		m_young->reassign(m_youngPos, Vec2(5, 1), 1, m_youngNum);
	if(adult)
		m_adult->reassign(m_adultPos, Vec2(1, 5), 2, m_adultNum);
	if(old)
		m_old->reassign(m_oldPos, Vec2(1, 1), 3, m_oldNum);
	if(dead)
		m_dead->reassign(m_deadPos, Vec2(5, 5), 4, m_deadNum);*/
}

void CropSector::render()
{
	if(!m_youngNum && !m_adultNum && !m_oldNum && !m_deadNum)
		return;
	if(m_toUpdate)
		update();
	Vec3 sectorCenter = m_worldPos;
	sectorCenter.x += g_grassSectorSize * g_grassTileSize * 0.5f;
	sectorCenter.y = core.game->getCamera()->getPosition()->y;
	sectorCenter.z += g_grassSectorSize * g_grassTileSize * 0.5f;
	if(D3DXVec3Length(&Vec3(*core.game->getCamera()->getPosition() - sectorCenter)) >= g_grassSectorRenderDistance)
		return;
	if(D3DXVec3Length(&Vec3(*core.game->getCamera()->getPosition() - sectorCenter)) <= g_grassAlphaRenderDistance)
	{
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); 
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000066);
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}
	else
	{
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE); 
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); 
	}
	Mat temp;
	D3DXMatrixTranslation(&temp, m_worldPos.x, m_worldPos.y, m_worldPos.z);
	
	gEngine.renderer->setWorld(&temp);
	if(m_youngNum)
	{
		core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("ColorMap", m_cropTypeInfo->textureYoung);
		core.game->getWorld()->getGrassManager()->getGrassShader()->beginPass(0);
		m_young->render();
		core.game->getWorld()->getGrassManager()->getGrassShader()->endPass();
	}
	if(m_adultNum)
	{
		core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("ColorMap", m_cropTypeInfo->textureAdult);
		core.game->getWorld()->getGrassManager()->getGrassShader()->beginPass(0);
		m_adult->render();
		core.game->getWorld()->getGrassManager()->getGrassShader()->endPass();
	}
	if(m_oldNum)
	{
		core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("ColorMap", m_cropTypeInfo->textureOld);
		core.game->getWorld()->getGrassManager()->getGrassShader()->beginPass(0);
		m_old->render();
		core.game->getWorld()->getGrassManager()->getGrassShader()->endPass();
	}
	if(m_deadNum)
	{
		core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("ColorMap", m_cropTypeInfo->textureDead);
		core.game->getWorld()->getGrassManager()->getGrassShader()->beginPass(0);
		m_dead->render();
		core.game->getWorld()->getGrassManager()->getGrassShader()->endPass();
	}
}

void CropSector::newTurn()
{
	generatePositions();
	m_toUpdate = true;
}

void CropSector::deleteMesh()
{
	m_young->deleteMesh();
	m_adult->deleteMesh();
	m_old->deleteMesh();
	m_dead->deleteMesh();
}

void CropSector::recreateMesh()
{
	m_young->recreate();
	m_adult->recreate();
	m_old->recreate();
	m_dead->recreate();
}
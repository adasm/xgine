#include "FieldSector.h"

FieldSector::FieldSector(Vec3 WorldPos, int posX, int posY)
{
	m_heights = NULL;
	m_dumpedGrass = true;
	m_dumpedHM = true;
	m_timeCounter = 0;
	m_posX = posX;
	m_posY = posY;
	m_heightMap = NULL;
	m_grassTypeMap = NULL;
	m_posX = posX;
	m_posY = posY;
	m_worldPos = WorldPos;
	//generateHeights();
	m_fieldMap = NULL;
	m_isEmpty = checkIsEmpty();
	m_fieldEntity = NULL;
	if(m_isEmpty)
		return;

	//generateGrassMap();
}

void FieldSector::generateGrassMap()
{
	if(m_isEmpty)
		return;
	if(!m_grassTypeMap)
	{
		if(gEngine.device->getDev()->CreateTexture(g_grassSectorSize, g_grassSectorSize, 0, /*D3DUSAGE_DYNAMIC*/0, D3DFMT_G16R16F, /*D3DPOOL_DEFAULT*/D3DPOOL_MANAGED, &m_grassTypeMap, NULL) != D3D_OK)
		{
			gEngine.kernel->log->prnEx(LT_FATAL, "FieldSector", "Couldn't create grass type map in %d %d sector", m_posX, m_posY);
			return;
		}
	}
	//vector<Field*> *map = core.game->getWorld()->getGrassManager()->getFieldMap();
	//Field* map = core.game->getWorld()->getGrassManager()->getFieldMap();
	//int mapSize = core.game->getWorld()->getGrassManager()->getMapSize();
	//m_isEmpty = true;
	D3DLOCKED_RECT pLockedRectGM;
	m_grassTypeMap->LockRect(0, &pLockedRectGM, NULL, D3DLOCK_DISCARD);
	for(int y = 0; y < g_grassSectorSize; y++)
	{
		for(int x = 0; x < g_grassSectorSize; x++)
		{
			float r = 0;
			float g = 0;
			if(m_fieldMap[y * g_grassSectorSize + x].cropId != -1)
			{
				if(m_fieldMap[y * g_grassSectorSize + x].stadium == 1)
					g = (core.game->getWorld()->getGrassManager()->getCropTypeInfo(m_fieldMap[y * g_grassSectorSize + x].cropId)->grassBladeHeightYoung);

				if(m_fieldMap[y * g_grassSectorSize + x].stadium == 2)
					g = (core.game->getWorld()->getGrassManager()->getCropTypeInfo(m_fieldMap[y * g_grassSectorSize + x].cropId)->grassBladeHeightAdult);

				if(m_fieldMap[y * g_grassSectorSize + x].stadium == 3)
					g = (core.game->getWorld()->getGrassManager()->getCropTypeInfo(m_fieldMap[y * g_grassSectorSize + x].cropId)->grassBladeHeightOld);

				if(m_fieldMap[y * g_grassSectorSize + x].stadium == 4)
					g = (core.game->getWorld()->getGrassManager()->getCropTypeInfo(m_fieldMap[y * g_grassSectorSize + x].cropId)->grassBladeHeightDead);

				r = m_fieldMap[y * g_grassSectorSize + x].cropId * 4 + (m_fieldMap[y * g_grassSectorSize + x].stadium-1);
				//m_isEmpty = false;
			}
			((ColorG16R16*)(pLockedRectGM.pBits))[y * g_grassSectorSize + x] = ColorG16R16(g, r);
			//gEngine.kernel->log->prnEx("%.3f %.3f", (float)((ColorG16R16*)(pLockedRectGM.pBits))[y * g_grassSectorSize + x].g, (float)((ColorG16R16*)(pLockedRectGM.pBits))[y * g_grassSectorSize + x].r);
		}
	}
	m_grassTypeMap->UnlockRect(0);
}

void FieldSector::newTurn()
{
	m_isEmpty = checkIsEmpty();
	if(m_isEmpty)
		return;
	for(int i = 0; i < g_grassSectorSize*g_grassSectorSize; i++)
	{
		if(m_fieldMap[i].stadium >= 0 && m_fieldMap[i].stadium < 4)
			m_fieldMap[i].stadium++;
	}
	dumpData();
}

bool FieldSector::checkIsEmpty()
{
	/*//vector<Field*> *map = core.game->getWorld()->getGrassManager()->getFieldMap();
	Field* map = core.game->getWorld()->getGrassManager()->getFieldMap();
	int mapSize = core.game->getWorld()->getGrassManager()->getMapSize();
	m_isEmpty = true;
	for(int y = 0; y < g_grassSectorSize; y++)
	{
		for(int x = 0; x < g_grassSectorSize; x++)
		{
			int X = x + m_posX;
			int Y = y + m_posY;
			if(map[Y * mapSize + X].cropId != -1)
				return false;
		}
	}
	return true;*/
	if(!m_fieldMap)
		return true;
	for(int y = 0; y < g_grassSectorSize; y++)
	{
		for(int x = 0; x < g_grassSectorSize; x++)
		{
			if(m_fieldMap[y * g_grassSectorSize + x].cropId != -1)
				return false;
		}
	}
	return true;
}

void FieldSector::generateHMap()
{
	if(!m_heightMap)
	{
		if(gEngine.device->getDev()->CreateTexture(g_grassSectorSize, g_grassSectorSize, 0, /*D3DUSAGE_DYNAMIC*/0, D3DFMT_R16F, /*D3DPOOL_DEFAULT*/D3DPOOL_MANAGED, &m_heightMap, NULL) != D3D_OK)
		{
			gEngine.kernel->log->prnEx(LT_FATAL, "FieldSector", "Couldn't create height map in %d %d sector", m_posX, m_posY);
			return;
		}
	}

	D3DLOCKED_RECT pLockedRectHM;// = new D3DLOCKED_RECT;
	m_heightMap->LockRect(0, &pLockedRectHM, NULL, /*D3DLOCK_DISCARD*/D3DLOCK_NO_DIRTY_UPDATE);

	D3DXFLOAT16 r;

	for(int y = 0; y < g_grassSectorSize; y++)
	{
		for(int x = 0; x < g_grassSectorSize; x++)
		{
			//D3DXFloat32To16Array(&r, &m_heights[y * g_grassSectorSize + x], 1);
			((D3DXFLOAT16*)(pLockedRectHM.pBits))[y * g_grassSectorSize + x] = m_heights[y * g_grassSectorSize + x];
		}
	}
	m_heightMap->UnlockRect(0);
}

void FieldSector::generateHeights()
{
	if(m_heights)
		return;
	m_heights = new D3DXFLOAT16[g_grassSectorSize * g_grassSectorSize];
	TerrainVertex *terrainVertices = core.game->getWorld()->getTerrainManager()->m_vertices;
	int terrainWidth = core.game->getWorld()->getTerrainManager()->m_size;
	float coordinatesModifier = g_grassTileSize / core.game->getWorld()->getTerrainManager()->m_hmGeoStep;
	for(int y = 0; y < g_grassSectorSize; y++)
	{
		for(int x = 0; x < g_grassSectorSize; x++)
		{
			float terrainGrassProportion = g_grassTileSize / core.game->getWorld()->getTerrainManager()->m_hmGeoStep;
			//terrainGrassProportion = 0.5f;
			/// filling heightmap with data up
			D3DXPLANE plane;

			int X = x + m_posX;
			int Y = y + m_posY;
			Vec3 p1 = terrainVertices[(int)(Y*terrainGrassProportion)*terrainWidth+(int)(X*terrainGrassProportion)].pos;
			Vec3 p2 = terrainVertices[(int)((Y*terrainGrassProportion)+1)*terrainWidth+(int)(X*terrainGrassProportion)].pos;
			Vec3 p3 = terrainVertices[(int)(Y*terrainGrassProportion)*terrainWidth+(int)(X*terrainGrassProportion)+1].pos;
			Vec3 p4 = terrainVertices[(int)(Y*terrainGrassProportion+1)*terrainWidth+(int)(X*terrainGrassProportion)+1].pos;

			Vec3 resultPos;
			Vec3 upPoint = Vec3((X)*g_grassTileSize, 2000, (Y)*g_grassTileSize);
			Vec3 downPoint = Vec3((X)*g_grassTileSize, -2000, (Y)*g_grassTileSize);

			f32 pU, pV, pDist;

			if(D3DXIntersectTri(&p1, &p2, &p3, &upPoint, &Vec3(0,-1,0), &pU, &pV, &pDist))
				D3DXPlaneFromPoints(&plane, &p1, &p2, &p3);
			else if(D3DXIntersectTri(&p2, &p3, &p4, &upPoint, &Vec3(0,-1,0), &pU, &pV, &pDist))
				D3DXPlaneFromPoints(&plane, &p2, &p3, &p4);	

			D3DXPlaneIntersectLine(&resultPos, &plane, &upPoint, &downPoint);
			D3DXFloat32To16Array(&m_heights[y * g_grassSectorSize + x], &resultPos.y, 1);
		}
	}
}

void FieldSector::dumpData()
{
	if(!m_dumpedGrass)
	{
		m_grassTypeMap->Release();
		m_grassTypeMap = NULL;
		m_dumpedGrass = true;
	}
	if(m_heightMap)
	{
		m_heightMap->Release();
		m_heightMap = NULL;
	}
	if(m_heights)
	{
		delete m_heights;
		m_heights = NULL;
	}
	m_dumpedHM = true;
}

void FieldSector::generateFieldMap()
{
	if(m_fieldMap)
		return;
	m_fieldMap = new Field[g_grassSectorSize * g_grassSectorSize];
	m_isEmpty = false;
}

void FieldSector::cleanupFieldMap()
{
	if(!m_fieldMap)
		return;
	delete m_fieldMap;
	m_fieldMap = NULL;
}

void FieldSector::setField(int x, int y, Field field)
{
	if(isEmpty())
		generateFieldMap();
	m_fieldMap[y * g_grassSectorSize + x] = field;
	if(m_fieldEntity)
		m_fieldEntity->sectorChangeNotification();
}

void FieldSector::getField(int x, int y, Field *field)
{
	if(isEmpty())
	{
		*field = Field();
		return;
	}
	else
		*field = m_fieldMap[y * g_grassSectorSize + x];
}

float FieldSector::getHeight(int x, int y)
{
	float r = 0.0f;
	if(!m_heights)
	{
		generateHeights();
		m_dumpedHM = true;
	}
	D3DXFloat16To32Array(&r, &m_heights[y * g_grassSectorSize + x], 1);
	return r;
}
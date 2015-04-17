#include "GrassManager.h"


int round(float a)
{
	if((a - floor(a)) >= 0.5f)
		return ceil(a);
	return (int)a;
}

CropType::CropType(string fname)
{
	typeName = "";
	cropName = "";

	grassBladeHeightYoung = 0;
	grassBladeHeightAdult = 0;
	grassBladeHeightOld = 0;
	grassBladeHeightDead = 0;

	float GrassBladeHeightYoung;
	float GrassBladeHeightAdult;
	float GrassBladeHeightOld;
	float GrassBladeHeightDead;
	int ColorId;

	vector<CfgVariable*> vars;
	vars.push_back(new CfgVariable(V_FLOAT, &GrassBladeHeightYoung, getVarName(GrassBladeHeightYoung)));
	vars.push_back(new CfgVariable(V_FLOAT, &GrassBladeHeightAdult, getVarName(GrassBladeHeightAdult)));
	vars.push_back(new CfgVariable(V_FLOAT, &GrassBladeHeightOld, getVarName(GrassBladeHeightOld)));
	vars.push_back(new CfgVariable(V_FLOAT, &GrassBladeHeightDead, getVarName(GrassBladeHeightDead)));

	vars.push_back(new CfgVariable(V_STRING, &typeName, getVarName(typeName)));
	vars.push_back(new CfgVariable(V_STRING, &cropName, getVarName(cropName)));

	vars.push_back(new CfgVariable(V_INT, &ColorId, getVarName(ColorId)));
	CfgLoader(fname, &vars);

	colorId = ColorId;//D3DCOLOR_XRGB((int)ColorId.x, (int)ColorId.y, (int)ColorId.z);
	grassBladeHeightYoung = GrassBladeHeightYoung;
	grassBladeHeightAdult = GrassBladeHeightAdult;
	grassBladeHeightOld = GrassBladeHeightOld;
	grassBladeHeightDead = GrassBladeHeightDead;
}


GrassManager::GrassManager(string cropTextureName)
{
	m_tileSize = g_grassTileSize;
	m_sectorSize = g_grassSectorSize;
	/*for(int i = 0; i < m_fieldMap.size(); i++)
		m_fieldMap[i] = new Field();*/
	//m_sectorsMap = new FieldSectorPTR[m_widthInSectors*m_heightInSectors];
	m_grassShader = gEngine.shaderMgr->load("Grass.fx");


	m_cropsTexture = gEngine.resMgr->load<Texture>(cropTextureName, 0);
	Buffer buff;
	gEngine.kernel->fs->loadCached("CropTypes\\crop.txt", buff, g_appCache);
	Tokenizer tokenizer(buff.data, buff.size);
	gEngine.kernel->mem->freeBuff(buff);

	string token;

	Vec3 *pos = new Vec3[g_grassSectorSize*g_grassSectorSize];
	Vec4 uvs;
	Vec2 bladeDimms;
	for(int y = 0; y < g_grassSectorSize; y++)
		for(int x = 0; x < g_grassSectorSize; x++)
			pos[y * g_grassSectorSize + x] = Vec3(x * m_tileSize, 0, y * m_tileSize);

	m_grassMesh = new GrassMesh(pos, Vec4(0, 0, (g_grassTextureCellWidth / m_cropsTexture->width), 1), Vec2(1, 1), 2, g_grassSectorSize*g_grassSectorSize, 1);
	m_grassMeshLow = new GrassMesh(pos, Vec4(0, 0, (g_grassTextureCellWidth / m_cropsTexture->width), 1), Vec2(1, 1), 1, g_grassSectorSize*g_grassSectorSize, 1);
	while(tokenizer.nextToken(&token))
	{
		string fname = token;
		tokenizer.nextToken(&token);
		fname[atoi(token.c_str())] = 0;
		m_cropTypes.push_back(new CropType(fname));
	}
}

void GrassManager::loadMap(Texture* indexMap)
{

	Texture* texture = indexMap;

	D3DLOCKED_RECT lockedRect;

	if(!texture)
		return;
	if(FAILED(texture->get()->LockRect(0, &lockedRect, 0, D3DLOCK_READONLY)))return;

	m_size = texture->width;
	m_sizeInSectors = ceil((float)m_size/(float)m_sectorSize);
	m_sectorsMap.resize(m_sizeInSectors*m_sizeInSectors);
	//m_fieldMap.resize(m_size*m_size);
	//m_fieldMap = new Field[m_size*m_size];





	int sectorCounter = 0;
	for(int y = 0; y < m_sizeInSectors; y++)
	{
		for(int x = 0; x < m_sizeInSectors; x++)
		{
			m_sectorsMap[y * m_sizeInSectors + x] = new FieldSector(Vec3(x * m_sectorSize * m_tileSize, 0, y * m_sectorSize * m_tileSize), x * m_sectorSize, y * m_sectorSize);
		}
	}
	gEngine.kernel->log->prnEx(LT_SUCCESS, "GrassManager", "Sectors computed properly. Sectors number: %d", m_sizeInSectors*m_sizeInSectors);






	//Generate groups of Entities
	unsigned int color;
	int pointCounter = 0;
	int textureSize = m_size;
	m_size = m_sizeInSectors * m_sectorSize;

	int bytesPerPoint = lockedRect.Pitch / m_size;
	Field temp;
	for(int y = 0; y < m_size; y++)
	{
		for(int x = 0; x < m_size; x++)
		{
			if(!(x > textureSize || y > textureSize))
			{
				color = ((unsigned int*)lockedRect.pBits)[y*m_size+x];
				if(color)
				{
					//gEngine.kernel->log->prnEx("old method: %d %d %d, new method: %d %d %d", GetRValue(color), GetGValue(color), GetBValue(color), _GetRValue(color), _GetGValue(color), _GetBValue(color));
					for(int i = 0; i < m_cropTypes.size(); i++)
					{
						if(GetBValue(color) == /*GetBValue(*/m_cropTypes[i]->colorId/*)*/)
						{
							temp.stadium = (char)GetGValue(color);
							temp.cropId = (char)i;
							temp.valueModifier = (char)(GetRValue(color) / 255 * 10);
							setField(x, y, temp);
							pointCounter++;
						}
					}
				}
			}
		}
	}
	texture->get()->UnlockRect(0);
	//gEngine.resMgr->release(texture);
	gEngine.kernel->log->prnEx(LT_INFO, "GrassManager", "IndexMap prepared properly. Points number: %d. Crop types number: %d", pointCounter, m_cropTypes.size());

	//int sizeInEntities = g_grassEntitiesNum;
	int sectorsPerEntity = m_sizeInSectors / g_grassEntitiesNum;
	Vec3 diagonal = Vec3(sectorsPerEntity * m_sectorSize * m_tileSize, 0, sectorsPerEntity * m_sectorSize * m_tileSize) * 0.5f;
	for(int y = 0; y < g_grassEntitiesNum; y++)
	{
		for(int x = 0; x < g_grassEntitiesNum; x++)
		{
			FieldSectorPTR* sectors = new FieldSectorPTR[sectorsPerEntity*sectorsPerEntity];
			int secNum = 0;
			for(int secY = 0; secY < sectorsPerEntity; secY++)
			{
				for(int secX = 0; secX < sectorsPerEntity; secX++)
				{
					sectors[secNum] = m_sectorsMap[(secY + (y * sectorsPerEntity))*m_sizeInSectors+(secX + (x * sectorsPerEntity))];
					secNum++;
				}
			}
			//gEngine.kernel->log->prnEx("SecNum: %d", secNum);
			Vec3 pos = Vec3(x * sectorsPerEntity * m_sectorSize * m_tileSize, 0, y * sectorsPerEntity * m_sectorSize * m_tileSize) + diagonal;
			core.game->getWorld()->addToWorld(new FieldEntity(pos, m_sectorSize, m_sectorSize, sectors, secNum), NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
		}
	}
	gEngine.kernel->log->prnEx(LT_SUCCESS, "GrassManager", "Grass loaded properly. Entities number: %d", g_grassEntitiesNum*g_grassEntitiesNum);
}

bool GrassManager::loadMapFromBin(string fname)
{
	FileStream *fstream = new FileStream(fname.c_str(), 1);
	if(!fstream->fp)
		return false;
	m_size = fstream->readDword();
	m_sizeInSectors = fstream->readDword();
	int fieldsNumber = fstream->readDword();
	BinEntry *buff = new BinEntry[fieldsNumber];
	m_sectorsMap.resize(m_sizeInSectors*m_sizeInSectors);
	//m_fieldMap = new Field[m_size*m_size];
	fstream->readBuffer((void*)buff, sizeof(BinEntry) * fieldsNumber);
	delete fstream;


	int sectorCounter = 0;
	for(int y = 0; y < m_sizeInSectors; y++)
	{
		for(int x = 0; x < m_sizeInSectors; x++)
		{
			m_sectorsMap[y * m_sizeInSectors + x] = new FieldSector(Vec3(x * m_sectorSize * m_tileSize, 0, y * m_sectorSize * m_tileSize), x * m_sectorSize, y * m_sectorSize);
		}
	}
	gEngine.kernel->log->prnEx(LT_SUCCESS, "GrassManager", "Sectors computed properly. Sectors number: %d", m_sizeInSectors*m_sizeInSectors);



	Field temp;
	for(int i = 0; i < fieldsNumber; i++)
	{
		setField(buff[i].pos.x, buff[i].pos.y, buff[i].field);
		//m_fieldMap[(int)(buff[i].pos.y * m_size + buff[i].pos.x)] = buff[i].field;
	}
	delete buff;


	gEngine.kernel->log->prnEx(LT_SUCCESS, "GrassManager", "IndexMap prepared properly. Points number: %d. Crop types number: %d", fieldsNumber, m_cropTypes.size());

	//int sizeInEntities = g_grassEntitiesNum;
	int sectorsPerEntity = m_sizeInSectors / g_grassEntitiesNum;
	Vec3 diagonal = Vec3(sectorsPerEntity * m_sectorSize * m_tileSize, 0, sectorsPerEntity * m_sectorSize * m_tileSize) * 0.5f;
	for(int y = 0; y < g_grassEntitiesNum; y++)
	{
		for(int x = 0; x < g_grassEntitiesNum; x++)
		{
			FieldSectorPTR* sectors = new FieldSectorPTR[sectorsPerEntity*sectorsPerEntity];
			int secNum = 0;
			for(int secY = 0; secY < sectorsPerEntity; secY++)
			{
				for(int secX = 0; secX < sectorsPerEntity; secX++)
				{
					sectors[secNum] = m_sectorsMap[(secY + (y * sectorsPerEntity))*m_sizeInSectors+(secX + (x * sectorsPerEntity))];
					secNum++;
				}
			}
			//gEngine.kernel->log->prnEx("SecNum: %d", secNum);
			Vec3 pos = Vec3(x * sectorsPerEntity * m_sectorSize * m_tileSize, 0, y * sectorsPerEntity * m_sectorSize * m_tileSize) + diagonal;
			core.game->getWorld()->addToWorld(new FieldEntity(pos, m_sectorSize, m_sectorSize, sectors, secNum), NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
		}
	}
	gEngine.kernel->log->prnEx(LT_SUCCESS, "GrassManager", "Grass loaded properly. Entities number: %d", g_grassEntitiesNum*g_grassEntitiesNum);

	return true;
}

void GrassManager::saveMapToBin(string fname)
{
	FileStream *fstream = new FileStream(fname.c_str(), 0);
	if(!fstream->fp)
		return;
	fstream->storeDword(m_size);
	fstream->storeDword(m_sizeInSectors);
	int fieldsNumber = 0;
	Field temp;
	for(int y = 0; y < m_size; y++)
	{
		for(int x = 0; x < m_size; x++)
		{
			getField(x, y, &temp);
			if(temp.stadium >= 0)
				fieldsNumber++;
		}
	}
	fstream->storeDword(fieldsNumber);

	for(int y = 0; y < m_size; y++)
	{
		for(int x = 0; x < m_size; x++)
		{
			getField(x, y, &temp);
			if(temp.stadium >= 0)
			{
				BinEntry btemp;
				btemp.field = temp;
				btemp.pos = Vec2(x, y);
				fstream->storeBuffer((void*)&btemp, sizeof(BinEntry));
			}
		}
	}
	delete fstream;
}


void GrassManager::destroyGrass(ActionBox* abox)
{
	if(!abox)
		return;
	Vec3 *min = new Vec3(0, 0, 0);
	Vec3 *max = new Vec3(0, 0, 0);
	*min = abox->getBoundingBox()->Min;
	*max = abox->getBoundingBox()->Max;

	int xtop = min->x;
	int ytop = min->z;

	int xend = max->x;
	int yend = max->z;


	xtop /= m_tileSize;
	ytop /= m_tileSize;

	xend /= m_tileSize;
	yend /= m_tileSize;

	if((xtop < 0 || ytop < 0 || xend < 0 || yend < 0) || (xtop > m_size || ytop > m_size || xend > m_size || yend > m_size))
		return;

	int vecDeleted = 0;
	int sectorIndex = 0;
	int oldSectorIndex = 0;

	bool edited = false;
	Field actualField;
	for(int y = ytop; y < yend; y++)
	{
		for(int x = xtop; x < xend; x++)
		{
			sectorIndex = (y / m_sectorSize) * m_sizeInSectors + (x / m_sectorSize);
			getField(x, y, &actualField);
			if(actualField.stadium >= 0)
			{
				if(abox->isPointInside(&Vec3(x*m_tileSize, getTerrainHeight(x * m_tileSize, y * m_tileSize) + 0.5f, y*m_tileSize)))
				{
					if(actualField.stadium >= 1)
						edited = true;
					actualField.cropId = -1;
					actualField.stadium = -1;
					setField(x, y, actualField);
				}
			}
			if(sectorIndex != oldSectorIndex)
			{
				if(edited)
				{
					m_sectorsMap[oldSectorIndex]->dumpData();
					edited = false;
				}
				oldSectorIndex = sectorIndex;
			}
		}
	}
	if(edited)
		m_sectorsMap[sectorIndex]->dumpData();
	return;
}

int GrassManager::harvestGrass(ActionBox* abox, CropType *grassFilter)
{
	if(!grassFilter)
		return 0;
	if(!abox)
		return 0;
	float retVal = 0.0f;
	Vec3 *min = new Vec3(0, 0, 0);
	Vec3 *max = new Vec3(0, 0, 0);
	*min = abox->getBoundingBox()->Min;
	*max = abox->getBoundingBox()->Max;

	int xtop = min->x;
	int ytop = min->z;

	int xend = max->x;
	int yend = max->z;


	xtop /= m_tileSize;
	ytop /= m_tileSize;

	xend /= m_tileSize;
	yend /= m_tileSize;

	if((xtop < 0 || ytop < 0 || xend < 0 || yend < 0) || (xtop > m_size || ytop > m_size || xend > m_size || yend > m_size))
		return 0;

	int vecDeleted = 0;
	int sectorIndex = 0;
	int oldSectorIndex = 0;

	bool edited = false;
	Field actualField;
	for(int y = ytop; y < yend; y++)
	{
		for(int x = xtop; x < xend; x++)
		{
			sectorIndex = (y / m_sectorSize) * m_sizeInSectors + (x / m_sectorSize);
			getField(x, y, &actualField);
			if(actualField.stadium > 0)
			{
				if(getCropTypeInfo(actualField.cropId) == grassFilter)
				{
					if(abox->isPointInside(&Vec3(x*m_tileSize, getTerrainHeight(x * m_tileSize, y * m_tileSize) + 1, y*m_tileSize)))
					{
						if(actualField.stadium == 1)
						{
							actualField.cropId = -1;
							actualField.stadium = -1;
							retVal += 0.5f + 0.5f * (actualField.valueModifier * 0.1f);
							//edited = true;
						}
						else
						if(actualField.stadium == 2)
						{
							actualField.cropId = -1;
							actualField.stadium = -1;
							retVal += 1 + (actualField.valueModifier * 0.1f);
							//edited = true;
						}
						else
						if(actualField.stadium == 3)
						{
							actualField.cropId = -1;
							actualField.stadium = -1;
							retVal += 0.5f + 0.5f * (actualField.valueModifier * 0.1f);
							//edited = true;
						}
						else
						if(actualField.stadium == 4)
						{
							actualField.cropId = -1;
							actualField.stadium = -1;
							//edited = true;
						}
						setField(x, y, actualField);
						edited = true;
					}
				}
			}
			if(sectorIndex != oldSectorIndex)
			{
				oldSectorIndex = sectorIndex;
				if(edited)
				{
					m_sectorsMap[oldSectorIndex]->dumpData();
					edited = false;
				}
			}
		}
	}
	if(edited)
		m_sectorsMap[sectorIndex]->dumpData();
	return retVal;
}

int GrassManager::harvestGrass(ActionBox* abox, string typeName, int *pOutCropID)
{
	if(!abox)
		return 0;
	float retVal = 0.0f;
	Vec3 *min = new Vec3(0, 0, 0);
	Vec3 *max = new Vec3(0, 0, 0);
	*min = abox->getBoundingBox()->Min;
	*max = abox->getBoundingBox()->Max;

	int xtop = min->x;
	int ytop = min->z;

	int xend = max->x;
	int yend = max->z;


	xtop /= m_tileSize;
	ytop /= m_tileSize;

	xend /= m_tileSize;
	yend /= m_tileSize;

	if((xtop < 0 || ytop < 0 || xend < 0 || yend < 0) || (xtop > m_size || ytop > m_size || xend > m_size || yend > m_size))
		return 0;

	int vecDeleted = 0;
	int sectorIndex = 0;
	int oldSectorIndex = (ytop / m_sectorSize) * m_sizeInSectors + (xtop / m_sectorSize);

	bool edited = false;
	bool cropTypeSet = false;
	Field actualField;
	for(int y = ytop; y < yend; y++)
	{
		for(int x = xtop; x < xend; x++)
		{
			sectorIndex = (y / m_sectorSize) * m_sizeInSectors + (x / m_sectorSize);
			getField(x, y, &actualField);
			if(actualField.stadium > 0)
			{
				if(getCropTypeInfo(actualField.cropId)->typeName == typeName)
				{
					if(abox->isPointInside(&Vec3(x*m_tileSize, getTerrainHeight(x * m_tileSize, y * m_tileSize) + 1, y*m_tileSize)))
					{
						if(!cropTypeSet)
						{
							cropTypeSet = true;
							*pOutCropID = actualField.cropId;
						}
						if(actualField.stadium == 1)
						{
							actualField.cropId = -1;
							actualField.stadium = -1;
							retVal += 0.5f + 0.5f * (actualField.valueModifier * 0.1f);
							//edited = true;
						}
						else
						if(actualField.stadium == 2)
						{
							actualField.cropId = -1;
							actualField.stadium = -1;
							retVal += 1 + (actualField.valueModifier * 0.1f);
							//edited = true;
						}
						else
						if(actualField.stadium == 3)
						{
							actualField.cropId = -1;
							actualField.stadium = -1;
							retVal += 0.5f + 0.5f * (actualField.valueModifier * 0.1f);
							//edited = true;
						}
						else
						if(actualField.stadium == 4)
						{
							actualField.cropId = -1;
							actualField.stadium = -1;
							//edited = true;
						}
						setField(x, y, actualField);
						edited = true;
					}
				}
			}
			if(sectorIndex != oldSectorIndex)
			{
				if(edited)
				{
					m_sectorsMap[oldSectorIndex]->dumpData();
					edited = false;
				}
				oldSectorIndex = sectorIndex;
			}
		}
	}
	if(edited)
		m_sectorsMap[sectorIndex]->dumpData();
	return retVal;
}

int GrassManager::seedGrass(ActionBox* abox, char cropID)
{
	if(!abox)
		return 0;
	int retVal = 0;
	Vec3 *min = new Vec3(0, 0, 0);
	Vec3 *max = new Vec3(0, 0, 0);
	*min = abox->getBoundingBox()->Min;
	*max = abox->getBoundingBox()->Max;

	int xtop = min->x;
	int ytop = min->z;

	int xend = max->x;
	int yend = max->z;


	xtop /= m_tileSize;
	ytop /= m_tileSize;

	xend /= m_tileSize;
	yend /= m_tileSize;

	if((xtop < 0 || ytop < 0 || xend < 0 || yend < 0) || (xtop > m_size || ytop > m_size || xend > m_size || yend > m_size))
		return 0;
	Field actualField;
	for(int y = ytop; y < yend; y++)
	{
		for(int x = xtop; x < xend; x++)
		{
			getField(x, y, &actualField);
			if(actualField.stadium <= 0)
			{
				if(abox->isPointInside(&Vec3(x*m_tileSize, getTerrainHeight(x * m_tileSize, y * m_tileSize) + 1, y*m_tileSize)))
				{
					actualField.cropId = cropID;
					actualField.stadium = 0;
					actualField.valueModifier = 0;
					setField(x, y, actualField);
					retVal++;
				}
			}
		}
	}
	return retVal;
}

void GrassManager::changeTerrain(ActionBox *abox, Vec4 terrainColor)
{
	if(!abox)
		return;
	Vec3 *min = new Vec3(0, 0, 0);
	Vec3 *max = new Vec3(0, 0, 0);
	*min = abox->getBoundingBox()->Min;
	*max = abox->getBoundingBox()->Max;

	int xtop = min->x;
	int ytop = min->z;

	int xend = max->x;
	int yend = max->z;


	xtop /= m_tileSize;
	ytop /= m_tileSize;

	xend /= m_tileSize;
	yend /= m_tileSize;

	if((xtop < 0 || ytop < 0 || xend < 0 || yend < 0) || (xtop > m_size || ytop > m_size || xend > m_size || yend > m_size))
		return;

	//core.game->getWorld()->getTerrainManager()->lockIndexTexture();
	int indTexExp = core.game->getWorld()->getTerrainManager()->m_indTexExp;
	int geoStep = core.game->getWorld()->getTerrainManager()->m_hmGeoStep;
	ytop--;
	xtop--;
	yend--;
	xend--;
	core.game->getWorld()->getTerrainManager()->lockIndexTexture(Vec4(xtop * m_tileSize, ytop * m_tileSize, xend * m_tileSize, yend * m_tileSize));
	for(int y = ytop; y < yend; y++)
	{
		for(int x = xtop; x < xend; x++)
		{
			Vec3 point = Vec3(x*m_tileSize, getTerrainHeight(x * m_tileSize, y * m_tileSize) + 1, y*m_tileSize);
			if(abox->isPointInside(&point))
			{
				core.game->getWorld()->getTerrainManager()->setIndexTextureTexel(x * m_tileSize, y * m_tileSize, &terrainColor);
			}
		}
	}
	core.game->getWorld()->getTerrainManager()->unlockIndexTexture(Vec4(xtop * m_tileSize, ytop * m_tileSize, xend * m_tileSize, yend * m_tileSize));
}

void GrassManager::newTurn()
{
	for(int i = 0; i < m_sectorsMap.size(); i++)
	{
		m_sectorsMap[i]->newTurn();
	}
}

CropType* GrassManager::getCropTypeInfo(int ID)
{
	if(ID >= 0 || ID < m_cropTypes.size())
		return m_cropTypes[ID];
	return NULL;
}

float GrassManager::getTerrainHeight(float x, float y)
{
	int convX = x / m_tileSize;
	int convY = y / m_tileSize;
	int secX = convX / g_grassSectorSize;
	int secY = convY / g_grassSectorSize;
	return m_sectorsMap[secY * m_sizeInSectors + secX]->getHeight(convX - (secX * g_grassSectorSize), convY - (secY * g_grassSectorSize));
}

void GrassManager::setField(int x, int y, Field field)
{
	int secX = x / g_grassSectorSize;
	int secY = y / g_grassSectorSize;
	m_sectorsMap[secY * m_sizeInSectors + secX]->setField(x - (secX * g_grassSectorSize), y - (secY * g_grassSectorSize), field);
}

void GrassManager::getField(int x, int y, Field *field)
{
	int secX = x / g_grassSectorSize;
	int secY = y / g_grassSectorSize;
	m_sectorsMap[secY * m_sizeInSectors + secX]->getField(x - (secX * g_grassSectorSize), y - (secY * g_grassSectorSize), field);
}
#pragma once
#include "Headers.h"

struct CropType
{
	string typeName;
	string cropName;

	float grassBladeHeightYoung;
	float grassBladeHeightAdult;
	float grassBladeHeightOld;
	float grassBladeHeightDead;

	unsigned int colorId;
	CropType()
	{
		typeName = "";
		cropName = "";

		grassBladeHeightYoung = 0;
		grassBladeHeightAdult = 0;
		grassBladeHeightOld = 0;
		grassBladeHeightDead = 0;

		colorId = 0;
	}
	CropType(string fname);
};

struct Field
{
	char stadium;			//-1 - empty; 0 - just sowed; 1 - young; 2 - adult; 3 - old; 4 - dead
	char valueModifier;		//in percent, divide by 10 to get value;
	char cropId;			//crop ID
	Field()
	{
		cropId = -1;
		stadium = -1;
		valueModifier = 0;
	}

	void operator=(Field f)
	{
		cropId = f.cropId;
		stadium = f.stadium;
		valueModifier = f.valueModifier;
	}
};

struct BinEntry
{
	Field field;
	Vec2 pos;
	BinEntry(Field fld, Vec2 ps)
	{
		field = fld;
		pos = ps;
	}
	BinEntry()
	{
		pos = Vec2(0, 0);
	}
};


class GrassManager
{
public:
	GrassManager(string cropTextureName); //Every size-parameter is in TileSize not in Units!!!

	void					loadMap(Texture* indexMap);
	bool					loadMapFromBin(string fname);
	void					saveMapToBin(string fname);


	void					destroyGrass(ActionBox* abox);
	int						harvestGrass(ActionBox* abox, string typeName, int *pOutCropID);
	int						harvestGrass(ActionBox* abox, CropType *grassFilter);
	int						seedGrass(ActionBox* abox, char cropID);
	void					changeTerrain(ActionBox *abox, Vec4 terrainColor);

	void					newTurn();


	void					setField(int x, int y, Field field);
	void					getField(int x, int y, Field *field);



	//FieldEntity*			createField(POINT* points, int pointNum, Vec2 grassBladeDimm, int grassBladesNum);
	int						getMapSize()				{ return m_size; } //returns width of map in tiles
	float					getTileSize()				{ return m_tileSize; } //returns one square size (one tile)
	int						getSectorSize()				{ return m_sectorSize; } //returns one sector of grass size in tiles(needed to LoD)
	vector<CropType*>*		getCropTypesList()			{ return &m_cropTypes; }
	CropType*				getCropTypeInfo(int ID);
	//Field*					getFieldMap()				{ return m_fieldMap; }

	Texture*				getCropTexture()			{ return m_cropsTexture; }

	Shader*					getGrassShader()			{ return m_grassShader; }
	GrassMesh*				getGrassMesh()				{ return m_grassMesh; }
	GrassMesh*				getGrassMeshLow()			{ return m_grassMeshLow; }
	float					getTerrainHeight(float x, float y);	///Returns height of terrain in given point(in and out are in units, not in tileSizes)
protected:
	float					m_tileSize;
	int						m_sectorSize;

	int						m_size;
	int						m_sizeInSectors;
	
	Shader*					m_grassShader;

	Texture*				m_cropsTexture;
	GrassMesh*				m_grassMesh;
	GrassMesh*				m_grassMeshLow;

	vector<CropType*>		m_cropTypes;
	//vector<Field*>			m_fieldMap; // y*width+x
	//Field*					m_fieldMap;
	vector<FieldSector*>	m_sectorsMap;
};
#pragma once
#include "Headers.h"

struct TerrainParams
{
	int geoStep;
	int indTexExp;
	int heightMapSize;
	int burnBin;
	
	string heightMapFname;
	string indexMapFname;

	string heightBinFname;
	string indexBinFname;

	bool burnMapToBin(TerrainManager *tm);
	bool loadTerrain(string cfgFname);
	TerrainParams()
	{
		geoStep = 0;
		indTexExp = 0;
		heightMapSize = 0;
		burnBin = 0;
		
		heightMapFname = "";
		indexMapFname = "";

		heightBinFname = "";
		indexBinFname = "";
	}
};
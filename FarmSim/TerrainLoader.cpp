#include "TerrainLoader.h"

bool TerrainParams::loadTerrain(string cfgFname)
{
	vector<CfgVariable*> vars;
	vars.push_back(new CfgVariable(V_INT, &geoStep, getVarName(geoStep)));
	vars.push_back(new CfgVariable(V_INT, &indTexExp, getVarName(indTexExp)));
	vars.push_back(new CfgVariable(V_INT, &heightMapSize, getVarName(heightMapSize)));
	vars.push_back(new CfgVariable(V_INT, &burnBin, getVarName(burnBin)));

	vars.push_back(new CfgVariable(V_STRING, &heightMapFname, getVarName(heightMapFname)));
	vars.push_back(new CfgVariable(V_STRING, &indexMapFname, getVarName(indexMapFname)));

	vars.push_back(new CfgVariable(V_STRING, &heightBinFname, getVarName(heightBinFname)));
	vars.push_back(new CfgVariable(V_STRING, &indexBinFname, getVarName(indexBinFname)));

	CfgLoader cfg(cfgFname, &vars);
	return true;
}

bool TerrainParams::burnMapToBin(TerrainManager *tm)
{
	tm->updateBinLoad(heightMapFname, indexMapFname, heightBinFname, indexBinFname, heightMapSize);
	return true;
}
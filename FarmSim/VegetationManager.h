#pragma once
#include "Headers.h"


struct TreeParams
{
	string branchMeshName;
	string bunchLowMeshName;
	string bunchHiMeshName;
	string treeLowMeshName;

	string branchTextureName;
	string branchNormalMapName;

	string bunchHiTextureName;
	string bunchHiNormalMapName;

	string bunchLowTextureName;
	string bunchLowNormalMapName;

	string treeLowTextureName;
	string treeLowNormalMapName;

	float interval;

	int treeTypeID;
	vector<CfgVariable*> *getVariableList();

	TreeParams()
	{
		interval = 0.0f;
		treeTypeID = 0;
		varList = NULL;
		branchMeshName = bunchLowMeshName = bunchHiMeshName = treeLowMeshName = branchTextureName = branchNormalMapName = bunchHiTextureName = bunchHiNormalMapName = bunchLowTextureName = bunchLowNormalMapName = treeLowTextureName = treeLowNormalMapName = "";
	}
protected:
	vector<CfgVariable*> *varList;
};



struct TreeType
{
	Mesh* branchMesh;
	Mesh* bunchLowMesh;
	Mesh* bunchHiMesh;
	Mesh* treeLowMesh;

	Texture* branchTexture;
	Texture* branchNormalMap;

	Texture* bunchHiTexture;
	Texture* bunchHiNormalMap;

	Texture* bunchLowTexture;
	Texture* bunchLowNormalMap;

	Texture* treeLowTexture;
	Texture* treeLowNormalMap;

	int treeTypeID;
	float interval;
	void loadFromTreeParams(TreeParams* treeParams);
	TreeType()
	{
		interval = 0.0f;
		treeTypeID = 0;
		branchMesh = NULL;
		bunchLowMesh = NULL;
		bunchHiMesh = NULL;
		treeLowMesh = NULL;

		branchTexture = NULL;
		branchNormalMap = NULL;

		bunchHiTexture = NULL;
		bunchHiNormalMap = NULL;

		bunchLowTexture = NULL;
		bunchLowNormalMap = NULL;

		treeLowTexture = NULL;
		treeLowNormalMap = NULL;
	}
};

struct TreePosition
{
	Vec3 pos;
	int treeType;
	TreePosition(Vec3 position, int tt)
	{
		pos = position;
		treeType = tt;
	}
	TreePosition()
	{
		pos = Vec3(0, 0, 0);
		treeType = 0;
	}
};


class VegetationManager
{
public:
	VegetationManager();
	bool					loadFromImage(Texture* indexMap);
	bool					loadFromBin(string fname);
	void					saveToBin(string foutname);

	void					cleanupMap();
	void					cleanupTypes();
protected:
	bool					ableToAdd(int pos, int textSize, int treeID);
	map<int, int>			m_treeMap;
	vector<TreeType*>		m_treeTypes;
	TreePosition*			m_trees;
	int						m_treesNumber;
};
#pragma once
#include "Headers.h"

struct ObjectParams
{
	string meshName;
	string collisionMeshName;
	string textureName;
	string normalMapName;
	string albedoMapName;

	float textureRepeat;
	float albedoMapRepeat;
	float heightMapScale;
	float specularExponent;
	Vec4 diffuseColor;
	int useSpecular;
	int useParallax;
	int receiveShadow;

	int collShape; //0 - NO_COLLISION; 1 - BOX; 2 - SPHERE; 3 - TRIANGLE
	float density;
	int collGroup; //0 - NON_COLLIDABLE; 1 - COLLIDABLE_NON_PUSHABLE; 2 - COLLIDABLE_PUSHABLE

	ObjectParams()
	{
		resetValues();
		m_varList = NULL;
	}
	bool					loadFromFile(string fname);
	CollisionShapeType		getCollisionShape();
	CollisionGroup			getCollisionGroup();
	Material*				generateMaterial();
	vector<CfgVariable*>	*getVariableList();
protected:
	void					resetValues();
	Material*				m_material;
	vector<CfgVariable*>*	m_varList;
};

struct SceneEntry
{
	string fname;
	string typeName;
	Vec3 pos;
	Vec3 rotate;
	SceneEntry()
	{
		fname = typeName = "";
		pos = rotate = Vec3(0, 0, 0);
	}
};

struct SceneParams
{
	string terrainCfg;
	string GrassMap;
	string treeBin;
	string grassBin;
	float waterLevel;
	Vec3 playerStartPoint;
	vector<SceneEntry*> vehicles;
	vector<SceneEntry*> agriDevices;
	vector<SceneEntry*> objects;
	vector<SceneEntry*> harvestShops;
	vector<SceneEntry*> deviceShops;
	vector<SceneEntry*>	lights;
	bool loadFromFile(string fname);
};
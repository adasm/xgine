#include "SceneLoader.h"

vector<CfgVariable*> *ObjectParams::getVariableList()
{

	if(m_varList)
		return m_varList;
	m_varList = new vector<CfgVariable*>;

	m_varList->push_back(new CfgVariable(V_STRING, &meshName, getVarName(meshName)));
	m_varList->push_back(new CfgVariable(V_STRING, &collisionMeshName, getVarName(collisionMeshName)));
	m_varList->push_back(new CfgVariable(V_STRING, &textureName, getVarName(textureName)));
	m_varList->push_back(new CfgVariable(V_STRING, &normalMapName, getVarName(normalMapName)));
	m_varList->push_back(new CfgVariable(V_STRING, &albedoMapName, getVarName(albedoMapName)));

	m_varList->push_back(new CfgVariable(V_FLOAT, &textureRepeat, getVarName(textureRepeat)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &albedoMapRepeat, getVarName(albedoMapRepeat)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &heightMapScale, getVarName(heightMapScale)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &specularExponent, getVarName(specularExponent)));

	m_varList->push_back(new CfgVariable(V_VEC4, &diffuseColor, getVarName(diffuseColor)));
	m_varList->push_back(new CfgVariable(V_INT, &useSpecular, getVarName(useSpecular)));
	m_varList->push_back(new CfgVariable(V_INT, &useParallax, getVarName(useParallax)));
	m_varList->push_back(new CfgVariable(V_INT, &receiveShadow, getVarName(receiveShadow)));

	m_varList->push_back(new CfgVariable(V_INT, &collShape, getVarName(collShape)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &density, getVarName(density)));
	m_varList->push_back(new CfgVariable(V_INT, &collGroup, getVarName(collGroup)));

	//varList->push_back(new CfgVariable(V_VEC3, &dimm, getVarName(dimm)));
	return m_varList;
}

void ObjectParams::resetValues()
{
	meshName = collisionMeshName = textureName = normalMapName = albedoMapName = "";
	textureRepeat = albedoMapRepeat = 1.0f;
	heightMapScale = 0.1f;
	specularExponent = 40.0f;
	diffuseColor = Vec4(1, 1, 1, 1);
	useSpecular = 1;
	useParallax = 0;
	receiveShadow = 1;

	collShape = 0;
	density = 0;
	collGroup = 0;
	m_material = NULL;
}

bool ObjectParams::loadFromFile(std::string fname)
{
	resetValues();
	CfgLoader(fname, getVariableList());
	return true;
}

CollisionShapeType ObjectParams::getCollisionShape()
{
	switch(collShape)
	{
		case 0:
			return NO_COLLISION;
			break;
		case 1:
			return BOX;
			break;
		case 2:
			return SPHERE;
			break;
		case 3:
			return TRIANGLE;
			break;
	}
	return NO_COLLISION;
}

CollisionGroup ObjectParams::getCollisionGroup()
{
	switch(collGroup)
	{
		case 0:
			return GROUP_NON_COLLIDABLE;
			break;
		case 1:
			return GROUP_COLLIDABLE_NON_PUSHABLE;
			break;
		case 2:
			return GROUP_COLLIDABLE_PUSHABLE;
			break;
	}
	return GROUP_NON_COLLIDABLE;
}

Material* ObjectParams::generateMaterial()
{
	if(!m_material)
		m_material = new Material(MT_DEFAULT, textureName, getWithoutExt(textureName)+"_n.png", "", 1, 1, 50, 1, DT_PARALLAX_OCCLUSION_MAPPING); //albedoMapName, textureRepeat, albedoMapRepeat, , heightMapScale, specularExponent,  diffuseColor, useSpecular, useParallax, receiveShadow);
	return m_material;
}

bool SceneParams::loadFromFile(string fname)
{
	Buffer buff;
	gEngine.kernel->fs->loadCached(fname, buff, g_appCache);
	Tokenizer tokenizer(buff.data, buff.size);
	gEngine.kernel->mem->freeBuff(buff);

	string token;
	treeBin = "";

	while(tokenizer.nextToken(&token))
	{
		SceneEntry* temp;
		if(token == "playerStartPoint")
		{
			tokenizer.nextToken(&token);
			playerStartPoint = getVec3FromString(token);
		}
		else
		if(token == "waterLevel")
		{
			tokenizer.nextToken(&token);
			waterLevel = getVec3FromString(token).x;
		}
		else
		if(token == "terrainCfg")
		{
			tokenizer.nextToken(&token);
			terrainCfg = token;
		}
		else
		if(token == "grassMap")
		{
			tokenizer.nextToken(&token);
			GrassMap = token;
		}
		else
		if(token == "grassBin")
		{
			tokenizer.nextToken(&token);
			grassBin = token;
		}
		else
		if(token == "treeBin")
		{
			tokenizer.nextToken(&token);
			treeBin = token;
		}
		else
		if(token == "agriDevice")
		{
			temp = new SceneEntry;
			tokenizer.nextToken(&token);
			temp->fname = token;
			tokenizer.nextToken(&token);
			temp->typeName = token;
			tokenizer.nextToken(&token);
			temp->pos = getVec3FromString(token);
			tokenizer.nextToken(&token);
			temp->rotate = getVec3FromString(token);
			agriDevices.push_back(temp);
		}
		else
		if(token == "vehicle")
		{
			temp = new SceneEntry;
			tokenizer.nextToken(&token);
			temp->fname = token;
			tokenizer.nextToken(&token);
			temp->typeName = token;
			tokenizer.nextToken(&token);
			temp->pos = getVec3FromString(token);
			tokenizer.nextToken(&token);
			temp->rotate = getVec3FromString(token);
			vehicles.push_back(temp);
		}
		else
		if(token == "object")
		{
			temp = new SceneEntry;
			tokenizer.nextToken(&token);
			temp->fname = token;
			tokenizer.nextToken(&token);
			temp->pos = getVec3FromString(token);
			tokenizer.nextToken(&token);
			temp->rotate = getVec3FromString(token);
			objects.push_back(temp);
		}
		else
		if(token == "harvestShop")
		{
			temp = new SceneEntry;
			tokenizer.nextToken(&token);
			temp->fname = token;
			tokenizer.nextToken(&token);
			temp->pos = getVec3FromString(token);
			tokenizer.nextToken(&token);
			harvestShops.push_back(temp);
		}
		else
		if(token == "deviceShop")
		{
			temp = new SceneEntry;
			tokenizer.nextToken(&token);
			temp->fname = token;
			tokenizer.nextToken(&token);
			temp->typeName = token;
			tokenizer.nextToken(&token);
			temp->pos = getVec3FromString(token);
			tokenizer.nextToken(&token);
			deviceShops.push_back(temp);
		}
		else
		if(token == "light")
		{
			temp = new SceneEntry;
			tokenizer.nextToken(&token);
			temp->fname = token;
			tokenizer.nextToken(&token);
			temp->pos = getVec3FromString(token);
			tokenizer.nextToken(&token);
			temp->rotate = getVec3FromString(token);
			D3DXVec3Normalize(&temp->rotate, &temp->rotate);
			lights.push_back(temp);
		}
	}
	return true;
}
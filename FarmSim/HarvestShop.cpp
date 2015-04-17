#include "HarvestShop.h"

HarvestShopParams::HarvestShopParams(string fname)
{
	vector<CfgVariable*> vars;

	vars.push_back(new CfgVariable(V_STRING, &harvestDumpPointCfgFileName, getVarName(harvestDumpPointCfgFileName)));
	vars.push_back(new CfgVariable(V_INT, &pricePerUnitHigh, getVarName(pricePerUnitHigh)));
	vars.push_back(new CfgVariable(V_INT, &pricePerUnitLow, getVarName(pricePerUnitLow)));
	vars.push_back(new CfgVariable(V_VEC3, &harvestDumpPointPos, getVarName(harvestDumpPointPos)));
	vars.push_back(new CfgVariable(V_VEC3, &harvestDumpPointRot, getVarName(harvestDumpPointRot)));

	CfgLoader cfg(fname, &vars);

	Buffer buff;
	gEngine.kernel->fs->loadCached(fname, buff, g_appCache);
	Tokenizer tokenizer(buff.data, buff.size);
	gEngine.kernel->mem->freeBuff(buff);

	string token;

	while(tokenizer.nextToken(&token))
	{
		SceneEntry* temp;
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
	}
}

HarvestShop::HarvestShop(Vec3 pos, HarvestShopParams *params)
{
	HarvestDumpPointParams* hdpp = new HarvestDumpPointParams;
	CfgLoader cfg(params->harvestDumpPointCfgFileName, hdpp->getVariableList());
	m_dumpPoint = new HarvestDumpPoint(params->harvestDumpPointPos, params->harvestDumpPointRot, hdpp);
	Mat pose;
	if(params->harvestDumpPointPos.y == 0)
		params->harvestDumpPointPos.y = core.game->getWorld()->getGrassManager()->getTerrainHeight(params->harvestDumpPointPos.x, params->harvestDumpPointPos.z);
	Vec3 harvestPos = pos + params->harvestDumpPointPos;
	worldMat(&pose, harvestPos, Vec3(0, 0, 0));
	m_dumpPoint->setPose(&pose);
	m_minPrice = params->pricePerUnitLow;
	m_maxPrice = params->pricePerUnitHigh;
	ObjectParams temp;
	for(int i = 0; i < params->objects.size(); i++)
	{
		if(params->objects[i]->pos.y == 0.0f)
		{
			params->objects[i]->pos += pos;
			params->objects[i]->pos.y = core.game->getWorld()->getGrassManager()->getTerrainHeight(params->objects[i]->pos.x, params->objects[i]->pos.z);
		}
		else
			params->objects[i]->pos += pos;
		temp.loadFromFile((string)("Objects\\") + params->objects[i]->fname);
		Surface *rend = new Surface(temp.meshName, temp.generateMaterial(), params->objects[i]->pos);
		setSurfaceTransform(rend, params->objects[i]->pos, params->objects[i]->rotate, 1);
		core.game->getWorld()->addToWorld(rend, temp.getCollisionShape(), temp.density, temp.getCollisionGroup());
	}
}

int HarvestShop::getActualPayload(CropType *cropType)
{
	return m_dumpPoint->getActualPayload(cropType);
}

int HarvestShop::getCapacity()
{
	return m_dumpPoint->getCapacity();
}

void HarvestShop::nextTurn()
{
}

int HarvestShop::sellHarvest(int number, CropType *cropType)
{
	float retVal = 0.0f;
	int actualPayload = getActualPayload(cropType);
	if(actualPayload == -1)
		return -1;
	if(number > getCapacity() - actualPayload)
		return -2;
	for(int i = 0; i < number; i++)
	{
		int priceDiff = m_maxPrice - m_minPrice;
		float wareNumberRate = actualPayload / getCapacity();
		retVal += m_maxPrice - (priceDiff * wareNumberRate);
		m_dumpPoint->addPayload(1, cropType);
	}
	return (int)retVal;
}
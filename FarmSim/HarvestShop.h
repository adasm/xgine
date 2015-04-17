#pragma once
#include "Headers.h"

struct HarvestShopParams
{
	HarvestShopParams(string fname);
	vector<SceneEntry*> objects;
	string harvestDumpPointCfgFileName;
	Vec3 harvestDumpPointPos;
	Vec3 harvestDumpPointRot;

	int pricePerUnitHigh;
	int pricePerUnitLow;
};

class HarvestShop
{
public:
	friend Player;
	HarvestShop(Vec3 pos, HarvestShopParams *params);
	HarvestDumpPoint*		getHarvestDumpPoint() { return m_dumpPoint; }
	int						getCapacity();
	int						getActualPayload(CropType* cropType);		//-1: uncompatible CropType

	void					nextTurn();
protected:
	int						sellHarvest(int number, CropType* cropType); //return values:
																		 //-1: uncompatible CropType
																		 //-2: too high ware number to sell
	HarvestDumpPoint*		m_dumpPoint;

	int						m_minPrice;
	int						m_maxPrice;
};
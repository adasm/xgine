#pragma once
#include "Headers.h"


struct ShopParams
{
	ShopParams(string fname);
	vector<SceneEntry*> objectsOutside;
	vector<SceneEntry*> objectsInside;
	vector<SceneEntry*> lights;
	vector<ShopItem*> shopItems;
	float entryPointRadius;
	Vec3 entryPoint;
	Vec3 cameraPos;
	Vec3 cameraLookAt;
	Vec3 itemDisplayPos;
	Vec3 parkingPos;
	int parkingColumns;
	int parkingRows;
};
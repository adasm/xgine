#include "IDeviceShop.h"

ShopParams::ShopParams(string fname)
{
	vector<CfgVariable*> vars;

	vars.push_back(new CfgVariable(V_FLOAT, &entryPointRadius, getVarName(entryPointRadius)));
	vars.push_back(new CfgVariable(V_VEC3, &cameraPos, getVarName(cameraPos)));
	vars.push_back(new CfgVariable(V_VEC3, &cameraLookAt, getVarName(cameraLookAt)));
	vars.push_back(new CfgVariable(V_VEC3, &entryPoint, getVarName(entryPoint)));
	vars.push_back(new CfgVariable(V_VEC3, &itemDisplayPos, getVarName(itemDisplayPos)));

	vars.push_back(new CfgVariable(V_VEC3, &parkingPos, getVarName(parkingPos)));
	vars.push_back(new CfgVariable(V_INT, &parkingColumns, getVarName(parkingColumns)));
	vars.push_back(new CfgVariable(V_INT, &parkingRows, getVarName(parkingRows)));

	CfgLoader cfg(fname, &vars, 1);

	Buffer buff;
	gEngine.kernel->fs->load(fname, buff);
	Tokenizer tokenizer(buff.data, buff.size);

	string token;

	while(tokenizer.nextToken(&token))
	{
		SceneEntry* temp;
		ShopItem *shopItemTemp;
		if(token == "item")
		{
			shopItemTemp = new ShopItem;
			tokenizer.nextToken(&token);
			shopItemTemp->typeName = token;
			tokenizer.nextToken(&token);
			shopItemTemp->fname = token;
			tokenizer.nextToken(&token);
			shopItemTemp->price = atoi(token.c_str());
			shopItems.push_back(shopItemTemp);
		}
		else
		if(token == "object")
		{
			string inside;
			temp = new SceneEntry;
			tokenizer.nextToken(&token);
			temp->fname = token;
			tokenizer.nextToken(&inside);
			tokenizer.nextToken(&token);
			temp->pos = getVec3FromString(token);
			tokenizer.nextToken(&token);
			temp->rotate = getVec3FromString(token);
			if(inside == "inside")
				objectsInside.push_back(temp);
			if(inside == "outside")
				objectsOutside.push_back(temp);
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
}
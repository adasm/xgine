#include "VegetationManager.h"

#define GetAValue(rgba) ((BYTE) ((rgba) >> 24))

vector<CfgVariable*> *TreeParams::getVariableList()
{
	if(varList)
		return varList;
	varList = new vector<CfgVariable*>;

	varList->push_back(new CfgVariable(V_STRING, &branchMeshName, getVarName(branchMeshName)));
	varList->push_back(new CfgVariable(V_STRING, &bunchLowMeshName, getVarName(bunchLowMeshName)));
	varList->push_back(new CfgVariable(V_STRING, &bunchHiMeshName, getVarName(bunchHiMeshName)));
	varList->push_back(new CfgVariable(V_STRING, &treeLowMeshName, getVarName(treeLowMeshName)));

	varList->push_back(new CfgVariable(V_STRING, &branchTextureName, getVarName(branchTextureName)));
	varList->push_back(new CfgVariable(V_STRING, &branchNormalMapName, getVarName(branchNormalMapName)));

	varList->push_back(new CfgVariable(V_STRING, &bunchHiTextureName, getVarName(bunchHiTextureName)));
	varList->push_back(new CfgVariable(V_STRING, &bunchHiNormalMapName, getVarName(bunchHiNormalMapName)));

	varList->push_back(new CfgVariable(V_STRING, &bunchLowTextureName, getVarName(bunchLowTextureName)));
	varList->push_back(new CfgVariable(V_STRING, &bunchLowNormalMapName, getVarName(bunchLowNormalMapName)));

	varList->push_back(new CfgVariable(V_STRING, &treeLowTextureName, getVarName(treeLowTextureName)));
	varList->push_back(new CfgVariable(V_STRING, &treeLowNormalMapName, getVarName(treeLowNormalMapName)));
	varList->push_back(new CfgVariable(V_INT, &treeTypeID, getVarName(treeTypeID)));
	varList->push_back(new CfgVariable(V_FLOAT, &interval, getVarName(interval)));

	return varList;
}


void TreeType::loadFromTreeParams(TreeParams *treeParams)
{
	treeTypeID = treeParams->treeTypeID;
	interval = treeParams->interval;
	if(treeParams->branchMeshName != "")
		branchMesh = gEngine.resMgr->load<Mesh>(treeParams->branchMeshName);
	if(treeParams->bunchLowMeshName != "")
		bunchLowMesh = gEngine.resMgr->load<Mesh>(treeParams->bunchLowMeshName);
	if(treeParams->bunchHiMeshName != "")
		bunchHiMesh = gEngine.resMgr->load<Mesh>(treeParams->bunchHiMeshName);
	if(treeParams->treeLowMeshName != "")
		treeLowMesh = gEngine.resMgr->load<Mesh>(treeParams->treeLowMeshName);

	if(treeParams->branchTextureName != "")
		branchTexture = gEngine.resMgr->load<Texture>(treeParams->branchTextureName);
	if(treeParams->branchNormalMapName != "")
		branchNormalMap = gEngine.resMgr->load<Texture>(treeParams->branchNormalMapName);

	if(treeParams->bunchHiTextureName != "")
		bunchHiTexture = gEngine.resMgr->load<Texture>(treeParams->bunchHiTextureName);
	if(treeParams->bunchHiNormalMapName != "")
		bunchHiNormalMap = gEngine.resMgr->load<Texture>(treeParams->bunchHiNormalMapName);

	if(treeParams->bunchLowTextureName != "")
		bunchLowTexture = gEngine.resMgr->load<Texture>(treeParams->bunchLowTextureName);
	if(treeParams->bunchLowNormalMapName != "")
		bunchLowNormalMap = gEngine.resMgr->load<Texture>(treeParams->bunchLowNormalMapName);

	if(treeParams->treeLowTextureName != "")
		treeLowTexture = gEngine.resMgr->load<Texture>(treeParams->treeLowTextureName);
	if(treeParams->treeLowNormalMapName != "")
		treeLowNormalMap = gEngine.resMgr->load<Texture>(treeParams->treeLowNormalMapName);
}

VegetationManager::VegetationManager()
{
	m_trees = NULL;
	cleanupMap();
	Buffer buff;
	gEngine.kernel->fs->loadCached("treeTypes\\trees.txt", buff, g_appCache);
	Tokenizer tokenizer(buff.data, buff.size);
	gEngine.kernel->mem->freeBuff(buff);

	string token;
	TreeParams treeParams;
	while(tokenizer.nextToken(&token))
	{
		TreeType *temp = new TreeType();
		string fname = token;
		tokenizer.nextToken(&token);
		fname[atoi(token.c_str())] = 0;
		CfgLoader(fname, treeParams.getVariableList());
		temp->loadFromTreeParams(&treeParams);
		m_treeTypes.push_back(temp);
	}
}

bool VegetationManager::loadFromImage(Texture* indexMap)
{
	cleanupMap();
	Texture* texture = indexMap;

	D3DLOCKED_RECT lockedRect;

	if(!texture)
	{
		return false;
	}
	if(FAILED(texture->get()->LockRect(0, &lockedRect, 0, D3DLOCK_READONLY)))return false;


	//unsigned int color;
	int pointCounter = 0;
	int textureSize = texture->width;
	
	int bytesPerPoint = lockedRect.Pitch / textureSize;
	struct TreeEntry
	{
		Vec3 pos;
		int treeType;
		bool accepted;
		u32 lodType;
		TreeEntry(Vec3 position, int treeID)
		{
			accepted = true;
			pos = position;
			treeType = treeID;
		}
	};
	vector<TreeEntry*> trees;
	DWORD loadTime = GetTickCount();
	gEngine.kernel->log->prnEx(LT_INFO, "VegetationManager", "Started texture loading.");
	int denied = 0;
	vector<Vec2> lastPos;
	for(int i = 0; i < m_treeTypes.size(); i++)
	{
		lastPos.push_back(Vec2(-1, -1));
	}
	for(int y = 0; y < 8000; y+=(rand()%56+35))
	{
		for(int x = 0; x < 8000; x+=(rand()%56+35))
		{
			/*color = ((unsigned int*)lockedRect.pBits)[y*textureSize+x];
			if(GetAValue(color) != 255)
			{
				for(int i = 0; i < m_treeTypes.size(); i++)
				{
					if(GetAValue(color) == m_treeTypes[i]->treeTypeID)
					{
						if(D3DXVec2Length(&Vec2(lastPos[i] - Vec2(x, y))) >= m_treeTypes[i]->interval || lastPos[i].x == -1)
							if(ableToAdd(y * textureSize + x, textureSize, i))
							{*/
								Vec3 pos = Vec3(x * g_grassTileSize, core.game->getWorld()->getGrassManager()->getTerrainHeight(x * g_grassTileSize, y * g_grassTileSize), y * g_grassTileSize);
								trees.push_back(new TreeEntry(pos, 0));
								pointCounter++;
								m_treeMap.insert(pair<int, int>(y * textureSize + x, 0));
						/*		lastPos[i] = Vec2(x, y);
							}
							else
								denied++;
					}
				}
			}*/
		}
	}
	texture->get()->UnlockRect(0);
	//gEngine.resMgr->release(texture);
	gEngine.kernel->log->prnEx(LT_INFO, "VegetationManager", "Tree texture loaded in time: %d", GetTickCount() - loadTime);
	gEngine.kernel->log->prnEx(LT_SUCCESS, "VegetationManager", "Trees loaded properly.\nTrees number loaded: %d.", pointCounter);
	gEngine.kernel->log->prnEx(LT_INFO, "VegetationManager", "denied trees: %d.", denied);
	gEngine.kernel->log->prnEx(LT_INFO, "VegetationManager", "Started trees adding to world");
	loadTime = GetTickCount();
	m_trees = new TreePosition[trees.size()];
	for(int i = 0; i < trees.size(); i++)
	{
		if(trees[i]->accepted)
		{
			string name[2];
			name[0] = "tree2_bunch.x";
			name[1] = "tree2_branch.x";

			f32 scale = 1;
			Surface *bunch = new Surface(name[0], new Material(MT_LEAF, "bunch_ex.png", "bunch_n.png"), m_trees[i].pos, Vec3(0,0,0), scale);
			Surface *branch = new Surface(name[1], new Material(MT_TREE, "branch.png", "branch_n.png"), m_trees[i].pos, Vec3(0,0,0), scale);
			branch->material->m_textureRepeat = 20.0f;
			branch->addSub(bunch);
			core.game->getWorld()->addToWorld(branch, NO_COLLISION, 0, GROUP_COLLIDABLE_NON_PUSHABLE);

			m_trees[i].pos = trees[i]->pos;
			m_trees[i].treeType = trees[i]->treeType;
		}
	}
	m_treesNumber = trees.size();
	gEngine.kernel->log->prnEx(LT_INFO, "VegetationManager", "Trees number in world: %d", m_treesNumber);
	gEngine.kernel->log->prnEx(LT_INFO, "VegetationManager", "Trees added in time: %d", GetTickCount() - loadTime);
	trees.clear();
	return true;
}

bool VegetationManager::loadFromBin(string fname)
{
	cleanupMap();
	FileStream *fstream = new FileStream(fname.c_str(), 1);
	if(!fstream->fp)
		return false;
	m_treesNumber = fstream->readDword();
	m_trees = new TreePosition[m_treesNumber];
	fstream->readBuffer((void*)m_trees, sizeof(TreePosition) * m_treesNumber);
	int lodType = 0;
	for(unsigned int i = 0; i < m_treesNumber; i++)
	{
		string name[2];
		name[0] = "tree2_bunch.x";
		name[1] = "tree2_branch.x";

		u32 r = rand()%5;
			
		Vec3 pos = m_trees[i].pos;

		Surface *surface;

		if(r == 0)
		{
			f32 scale = 2+(f32)(rand()%100)/40.0f;
			Surface *bunch = new Surface(name[0], new Material(MT_LEAF, "bunch_ex.png", "bunch_n.png"), pos, Vec3(0,0,0), scale);
			surface = new Surface(name[1], new Material(MT_TREE, "branch.png", "branch_n.png"), pos, Vec3(0,0,0), scale);
			surface->material->m_textureRepeat = 20.0f;
			surface->addSub(bunch);
			core.game->getWorld()->addToWorld(surface, NO_COLLISION, 0, GROUP_COLLIDABLE_NON_PUSHABLE);
		}
		else if(r == 1)
		{
			f32 scale = 0.1f + rand()%2;
			surface= new Surface("veg/shrub.x", new Material(MT_LEAF, "veg/shrub.png", "veg/shrub_n.png"), pos, Vec3(0,0,0), scale);
			surface->material->m_animationSpeed = 1.5f;
			core.game->getWorld()->addToWorld(surface, NO_COLLISION, 0, GROUP_COLLIDABLE_NON_PUSHABLE);
		}
		else if(r == 2)
		{
			f32 scale = rand()%2+(f32)(rand()%100)/120.0f;
			surface = new Surface("veg/fern.x", new Material(MT_LEAF, "veg/fern.png", "veg/fern_n.png"), pos, Vec3(0,0,0), scale);
			surface->material->m_animationSpeed = 2.0f;
			core.game->getWorld()->addToWorld(surface, NO_COLLISION, 0, GROUP_COLLIDABLE_NON_PUSHABLE);
		}
		else if(r == 3)
		{
			f32 scale = 0.3f + (f32)(rand()%100)/120.0f;
			surface = new Surface("veg/shrub.x", new Material(MT_LEAF, "veg/shrub.png", "veg/shrub_n.png"), pos, Vec3(0,0,0), scale);
			surface->material->m_animationSpeed = 2.0f;
			core.game->getWorld()->addToWorld(surface, NO_COLLISION, 0, GROUP_COLLIDABLE_NON_PUSHABLE);
		}
		else if(r == 4)
		{
			f32 scale = 0.2f + (f32)(rand()%100)/120.0f;
			surface= new Surface("veg/fern.x", new Material(MT_LEAF, "veg/fern.png", "veg/fern_n.png"), pos, Vec3(0,0,0), scale);
			surface->material->m_animationSpeed = 1;
			surface->material->m_animationSpeed = 2.0f;
			core.game->getWorld()->addToWorld(surface, NO_COLLISION, 0, GROUP_COLLIDABLE_NON_PUSHABLE);
		}

	}
	gEngine.kernel->log->prnEx(LT_SUCCESS, "VegetationManager", "Trees loaded properly from '%s'.\nTrees number: %d", fname.c_str(), m_treesNumber);
	delete fstream;
	return true;
}

void VegetationManager::saveToBin(string foutname)
{
	FileStream *fstream = new FileStream(foutname.c_str(), 0);
	if(!fstream->fp)
		return;
	fstream->storeDword(m_treesNumber);
	fstream->storeBuffer((void*)m_trees, sizeof(TreePosition) * m_treesNumber);
	m_treeMap.clear();
	delete fstream;
	return;
}

bool VegetationManager::ableToAdd(int position, int textSize, int treeID)
{
	Vec2 pos = getCoordinatesFromIndex(position, textSize);
	for(int y = pos.y - m_treeTypes[treeID]->interval; y < pos.y + m_treeTypes[treeID]->interval; y++)
		for(int x = pos.x - m_treeTypes[treeID]->interval; x < pos.x + m_treeTypes[treeID]->interval; x++)
		{
			if(m_treeMap.find(y * textSize + x) != m_treeMap.end())
			{
				if(D3DXVec2Length(&Vec2(pos - Vec2(x, y))) < m_treeTypes[treeID]->interval)
					return false;
			}
		}
	return true;
}

void VegetationManager::cleanupMap()
{
	if(m_trees)
		delete m_trees;
	m_trees = NULL;
	m_treeMap.clear();
	m_treesNumber = 0;
}

void VegetationManager::cleanupTypes()
{
	m_treeTypes.clear();
}
#include "World.h"

void World::init(SceneParams *sceneParams)
{
	DWORD waterTime;
	DWORD dayNightManagerTime;
	DWORD terrainManagerTime;
	DWORD grassTime;
	DWORD treesTime;
	DWORD sceneTime;
	waterTime = GetTickCount();
	m_scene = new Scene(core.game->getCamera());
	m_scene->setSun(Vec3(0, -1, 0), Vec4(1, 1, 1, 1));
	m_waterLevel = sceneParams->waterLevel;
	m_scene->addWater(m_waterLevel);
	waterTime = GetTickCount() - waterTime;

	dayNightManagerTime = GetTickCount();
	m_dayNightManager = new DayNightManager(m_scene, 1000, 12.0f, "lens.txt");
	m_playerInfo = new Player(sceneParams->playerStartPoint);
	dayNightManagerTime = GetTickCount() - dayNightManagerTime;

	terrainManagerTime = GetTickCount();
	TerrainParams terrainLoader;
	terrainLoader.loadTerrain(sceneParams->terrainCfg);
	m_terrainManager = new TerrainManager(m_scene, terrainLoader.geoStep, terrainLoader.indTexExp);
	if(terrainLoader.burnBin)
	{
		terrainLoader.burnMapToBin(m_terrainManager);
	}

	addTerrainToWorld(terrainLoader.heightBinFname, terrainLoader.indexBinFname);
	terrainManagerTime = GetTickCount() - terrainManagerTime;

	grassTime = GetTickCount();
	m_grassManager = new GrassManager("crops.tga");
	
	Texture* indexMap = NULL;
	if(!m_grassManager->loadMapFromBin(sceneParams->grassBin))
	{
		indexMap = gEngine.resMgr->load<Texture>(sceneParams->GrassMap, 0);
		m_grassManager->loadMap(indexMap);
		m_grassManager->saveMapToBin("data\\grass.bin");
	}
	
	grassTime = GetTickCount() - grassTime;
	//delete(m_terrainManager->m_vertices); m_terrainManager->m_vertices = 0;
	delete(m_terrainManager->m_indices); m_terrainManager->m_indices = 0;

	treesTime = GetTickCount();
	m_vegetationManager = new VegetationManager();
	if(!m_vegetationManager->loadFromBin(sceneParams->treeBin))
	{
		if(!indexMap)
			indexMap = gEngine.resMgr->load<Texture>(sceneParams->GrassMap, 0);
		m_vegetationManager->loadFromImage(indexMap);
		m_vegetationManager->saveToBin("data\\trees.bin");
	}
	treesTime = GetTickCount() - treesTime;
	if(indexMap)
		gEngine.resMgr->release(indexMap);

	sceneTime = GetTickCount();

	m_trafficManager = new TrafficManager;
	//for(int i = 
	const f32 count = 0.0f;
	int modifier = 7;
	for(u32 i = 0; i < count; i++)
		m_trafficManager->addActor(new Tractor(Vec3(4368 + sin(2 * D3DX_PI * i/count)*count*modifier, 650, 2032 + cos(2 * D3DX_PI * i/count)*count*modifier), "mustang.txt"));

	for(int i = 0; i < sceneParams->vehicles.size(); i++)
	{
		if(sceneParams->vehicles[i]->typeName == "Combine")
			addToWorld(new Combine(sceneParams->vehicles[i]->pos, sceneParams->vehicles[i]->fname));
		if(sceneParams->vehicles[i]->typeName == "Tractor")
			addToWorld(new Tractor(sceneParams->vehicles[i]->pos, sceneParams->vehicles[i]->fname));
		if(sceneParams->vehicles[i]->typeName == "Trailer")
			addToWorld(new Trailer(sceneParams->vehicles[i]->pos, sceneParams->vehicles[i]->fname));
	}
	for(int i = 0; i < sceneParams->agriDevices.size(); i++)
	{
		if(sceneParams->agriDevices[i]->typeName == "Plow")
			addToWorld(new Plow(NxVec3(sceneParams->agriDevices[i]->pos), sceneParams->agriDevices[i]->fname));
		if(sceneParams->agriDevices[i]->typeName == "Cutter")
			addToWorld(new Cutter(NxVec3(sceneParams->agriDevices[i]->pos), sceneParams->agriDevices[i]->fname));
		if(sceneParams->agriDevices[i]->typeName == "Seeder")
			addToWorld(new Seeder(NxVec3(sceneParams->agriDevices[i]->pos), sceneParams->agriDevices[i]->fname));
		/*if(sceneParams->agriDevices[i]->typeName == "Cultivator")
			addToWorld(new Cultivator(NxVec3(sceneParams->agriDevices[i]->pos), sceneParams->agriDevices[i]->fname));*/
	}
	ObjectParams temp;
	for(int i = 0; i < sceneParams->objects.size(); i++)
	{
		temp.loadFromFile((string)("Objects\\") + sceneParams->objects[i]->fname);
		if(sceneParams->objects[i]->pos.y == 0.0f)
			sceneParams->objects[i]->pos.y = m_grassManager->getTerrainHeight(sceneParams->objects[i]->pos.x, sceneParams->objects[i]->pos.z);

		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////// < ----- TUTAJ COS NIE TAK
		//////////////////////////////////////////////////////////////////// < ----- TUTAJ COS NIE TAK
		if(temp.meshName == "")continue;         /////////////////////////// < ----- TUTAJ COS NIE TAK, nawet jak meshName = "" i tak ladujesz Surface i dodajesz do sceny, a potem jest blad przy renderingu
		//////////////////////////////////////////////////////////////////// < ----- TUTAJ COS NIE TAK
		//////////////////////////////////////////////////////////////////// < ----- TUTAJ COS NIE TAK
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////

		Surface *rend = new Surface(temp.meshName, temp.generateMaterial(), sceneParams->objects[i]->pos);
		setSurfaceTransform(rend, sceneParams->objects[i]->pos, sceneParams->objects[i]->rotate, 1);
		addToWorld(rend, temp.getCollisionShape(), temp.density, temp.getCollisionGroup(), gEngine.resMgr->load<Mesh>(temp.collisionMeshName));
	}
	for(int i = 0; i < sceneParams->harvestShops.size(); i++)
	{
		if(sceneParams->harvestShops[i]->pos.y == 0.0f)
			sceneParams->harvestShops[i]->pos.y = m_grassManager->getTerrainHeight(sceneParams->harvestShops[i]->pos.x, sceneParams->harvestShops[i]->pos.z);
		addToWorld(new HarvestShop(sceneParams->harvestShops[i]->pos, new HarvestShopParams(sceneParams->harvestShops[i]->fname)));
	}

	for(int i = 0; i < sceneParams->deviceShops.size(); i++)
	{
		if(sceneParams->deviceShops[i]->pos.y == 0.0f)
			sceneParams->deviceShops[i]->pos.y = m_grassManager->getTerrainHeight(sceneParams->deviceShops[i]->pos.x, sceneParams->deviceShops[i]->pos.z);
		if(sceneParams->deviceShops[i]->typeName == "Tractor")
			addToWorld(new TractorShop(sceneParams->deviceShops[i]->pos, new ShopParams(sceneParams->deviceShops[i]->fname)));
	}
	for(int i = 0; i < sceneParams->lights.size(); i++)
	{
		if(sceneParams->lights[i]->pos.y <= 0)
			sceneParams->lights[i]->pos.y = m_grassManager->getTerrainHeight(sceneParams->lights[i]->pos.x, sceneParams->lights[i]->pos.z) - sceneParams->lights[i]->pos.y;
		m_dayNightManager->addLight(sceneParams->lights[i]->fname, sceneParams->lights[i]->pos, sceneParams->lights[i]->rotate);
	}
	sceneTime = GetTickCount() - sceneTime;

	m_torch = new Light(LIGHT_SPOT, 1, Vec4(239.0f/255.0f,228.0f/255.0f,176.0f/255.0f, 1), *core.game->getCamera()->getPosition(), 200, *core.game->getCamera()->getViewDirection(), D3DX_PI/4);
	m_torch->setColor(&Vec4(239.0f/255.0f,228.0f/255.0f,176.0f/255.0f, 1));
	m_scene->add(m_torch);
	m_torchEnabled = false;

	m_scene->update();
	DWORD total = waterTime + dayNightManagerTime + terrainManagerTime + grassTime + treesTime + sceneTime;

	gEngine.kernel->log->prnEx(LT_SUCCESS, "World", "Scene loaded properly.\nWater setting time: %d\nDayNight mgr time: %d\nTerrainManager time: %d\nGrass time: %d\nTrees time: %d\nScene time: %d\nTotal: %d",
	waterTime,
	dayNightManagerTime,
	terrainManagerTime,
	grassTime,
	treesTime,
	sceneTime,
	total);
}


void World::close()
{
	delete(m_scene);
}

void World::render()
{
	bool klawisz = false;
	if(gEngine.input->isKeyDown(DIK_P))
		klawisz = true;
	m_deltaTime = gEngine.kernel->tmr->get() / 500.0f;
	{
		PROFILER("Game logic");
		update();
	}
	if(!klawisz)
		core.dynamics->startSimulation(m_deltaTime);
	Scene* sceneToRender = m_scene;
	if(m_playerInfo->getVisitingShop())
		sceneToRender = m_playerInfo->getVisitingShop()->getScene();
	sceneToRender->preRender();
	if(gEngine.renderer->begin()){
		gEngine.renderer->clear(1,1,1,0x000000);
			sceneToRender->render();
				//gEngine.renderer->drawBoundingBox(m_agriDevices[2]->getActionBox()->getBoundingBox(), Vec4(1, 1, 1, 1));
				//gEngine.renderer->drawBoundingBox(m_harvestShops[0]->getHarvestDumpPoint()->getActionBox()->getBoundingBox(), Vec4(1, 1, 1, 1));
			m_dayNightManager->renderLensFlare();
			/*gEngine.renderer->addTxt("CamPos:%f,%f,%f", camera->getPosition()->x, camera->getPosition()->y, camera->getPosition()->z);
			gEngine.renderer->addTxt("CamDir:%f,%f,%f", camera->getViewDirection()->x, camera->getViewDirection()->y, camera->getViewDirection()->z);
			gEngine.renderer->addTxt("Terrain size: %u", terrainManager->m_size);
			gEngine.renderer->addTxt("Terrain sector size: %u", terrainManager->m_sectorSize);*/
		gEngine.renderer->end();
	}
	if(!klawisz)
		core.dynamics->endSimulation();
}

struct LightBox
{
	Surface* ent;
	Light* light;
};

vector<LightBox> lightBoxes;

void World::update()
{
	m_dayNightManager->update();
	core.dynamics->updateCars(m_deltaTime);
	if(gEngine.desktop->isFocused())
		updateInput();
	m_playerInfo->update();
	for(int i = 0; i < m_vehicles.size(); i++)
	{
		m_vehicles[i]->update();
	}

	for(int i = 0; i < m_trailers.size(); i++)
	{
		m_trailers[i]->update();
	}

	for(int i = 0; i < m_agriDevices.size(); i++)
	{
		m_agriDevices[i]->update();
	}


	NxActor* temp;
	{
		for(int i = 0; i < m_scene->getRendList()->surfaceVector.size(); i++)
		{
			if(m_scene->getRendList()->surfaceVector[i]->ptr_userData != NULL)
			if(((EntityData*)(m_scene->getRendList()->surfaceVector[i]->ptr_userData))->m_actor != NULL)
			{
				temp = ((EntityData*)(m_scene->getRendList()->surfaceVector[i]->ptr_userData))->m_actor;
				if(temp->isSleeping())
					continue;

				temp->getGlobalPose().getColumnMajor44((NxF32*)&m_scene->getRendList()->surfaceVector[i]->world);
				D3DXMatrixInverse(&m_scene->getRendList()->surfaceVector[i]->invWorld, 0, &m_scene->getRendList()->surfaceVector[i]->world);
				Vec3 pos = Vec3((*(NxVec3*)&temp->getGlobalPosition()).x,(*(NxVec3*)&temp->getGlobalPosition()).y, (*(NxVec3*)&temp->getGlobalPosition()).z);
				m_scene->getRendList()->surfaceVector[i]->position = pos;
				m_scene->getRendList()->surfaceVector[i]->boundingBox.setTransform(m_scene->getRendList()->surfaceVector[i]->position);
			}
		}
	}

	for(u32 i = 0; i < lightBoxes.size(); i++)
	{
		lightBoxes[i].light->setPos(&lightBoxes[i].ent->boundingBox.getCenter());
		lightBoxes[i].light->update(core.game->getCamera());
	}

	m_scene->update();
	m_terrainManager->update(core.game->getCamera());
	m_trafficManager->update(core.game->getCamera());
}

void World::updateInput()
{
	{
		/*if(gEngine.input->isKeyDown(DIK_C))
		{
			m_scene->add(new Light(LIGHT_POINT, 1, Vec4(rand()%1000/1000.0f,rand()%1000/1000.0f,rand()%1000/1000.0f, 1), *core.game->getCamera()->getPosition(), 100));
			Sleep(100);
		}
		if(gEngine.input->isKeyDown(DIK_V))
		{
			m_scene->add(new Light(LIGHT_SPOT, 1, Vec4(rand()%1000/1000.0f,rand()%1000/1000.0f,rand()%1000/1000.0f, 1), *core.game->getCamera()->getPosition(), 100, *core.game->getCamera()->getViewDirection(), D3DX_PI/4));
			Sleep(100);
		}*/

		if(gEngine.input->isKeyDown(DIK_B))
		{
			m_scene->add(new Light(LIGHT_DIR, 1, Vec4(rand()%1000/1000.0f,rand()%1000/1000.0f,rand()%1000/1000.0f, 1), *core.game->getCamera()->getPosition(), 100, *core.game->getCamera()->getViewDirection(), D3DX_PI/3));
			Sleep(100);
		}
	}

	/*if(gEngine.input->isKeyDown(DIK_P))
	{
		m_scene->add(new Particle(&Vec3(*core.game->getCamera()->getPosition()), "particle.png", PARTICLEB_DEF, 100, 10, 5, &Vec3(5,30,5), &Vec3(1,50,4), &Vec3(10,50,10), &Vec4(0,0,0,0.9), &Vec4(0,0,0,0), &Vec4(0,0,0,0), 4, 14, 0, 2, 4));
		Sleep(100);
	}*/

	if(gEngine.input->isKeyDown(DIK_X))
	{
		/*const u32 bs = 100;
		Vec3 *positions = new Vec3[bs*bs];
		for(u32 X = 0; X < 4; X++)
		{
			for(u32 Y = 0; Y < 4; Y++)
			{
				for(u32 x = 0; x < bs; x++)
				{
						for(u32 y = 0; y < bs; y++)
						{
							positions[y*bs+x] = Vec3(2000*X+1000.0f,0,2000*Y+1000.0f) + Vec3((-1000.0f + (f32)(rand()%2000))* g_grassTileSize, 0, (-1000.0f + (f32)(rand()%2000))* g_grassTileSize);
							positions[y*bs+x].y = core.game->getWorld()->getGrassManager()->getTerrainHeight(positions[y*bs+x].x, positions[y*bs+x].z) + 0.5f;
						}
				}

				Vegetation* veg = new Vegetation("grassWalpha.png", positions, bs*bs, 2.0f, BILLBOARDROT_LEFT);
				veg->ptr_userData = 0;
				m_scene->add(veg, 1);
			}
		}
		
		delete(positions);
		*/
	
		for(u32 X = 0; X < 8192; X+= rand()%40+20)
		{
			for(u32 Y = 0; Y < 8192; Y+= rand()%40+30)
			{
				Vec3 pos = Vec3(X, 0, Y);
				pos.y = core.game->getWorld()->getGrassManager()->getTerrainHeight(pos.x, pos.z);
				f32 scale = 0.1f + ((f32)(rand()%1000))/9000.0f;
				Surface* surface= new Surface("veg/shrub.x", new Material(MT_LEAF, "veg/shrub.png"), pos, Vec3(0,0,0), scale);
				surface->material->m_animationSpeed = 5.0f;
				core.game->getWorld()->addToWorld(surface, NO_COLLISION, 0, GROUP_COLLIDABLE_NON_PUSHABLE);
			}
		}


		//m_scene->add(new Particle(&Vec3(*core.game->getCamera()->getPosition()), "particle2.png", PARTICLEB_DEF, 1000, 1000, 0, &Vec3(20,0,20), &Vec3(90000,0,90000), &Vec3(20000,10,20000), &Vec4(0.6,0.6,0.6,0.3), &Vec4(0.6,0.6,0.6,0.3), &Vec4(0,0,0,0.1),700, 700, 300, 1.5, 2, PARTICLEROT_LEFT), 1);
		//m_trafficManager->addActor(new Tractor(*core.game->getCamera()->getPosition(), "mustang.txt"));
		Sleep(300);
	}

	if(gEngine.input->isKeyDown(DIK_Z))
	{
		m_dayNightManager->setTime(m_dayNightManager->getHour(), m_dayNightManager->getMinutes()+30);
	}

	if(gEngine.input->isKeyDown(DIK_L))
	{
		gEngine.renderer->takeScreenshot();
		Sleep(200);
	}

	m_playerInfo->updateInput();

	{ // TORCH
		if(m_torchEnabled)
		{
			Vec3 left = Vec3(core.game->getCamera()->getViewLeft()->x, core.game->getCamera()->getViewLeft()->y, core.game->getCamera()->getViewLeft()->z);
			Vec3 up = Vec3(core.game->getCamera()->getViewUp()->x, core.game->getCamera()->getViewUp()->y, core.game->getCamera()->getViewUp()->z);
			m_torch->setPos(&(*core.game->getCamera()->getPosition()+left));
			m_torch->setDir(core.game->getCamera()->getViewDirection());
			m_torch->setColor(&Vec4(1,1,1,1));
		}
		
		m_torch->setEnabled(m_torchEnabled);

		if(gEngine.input->isKeyDown(DIK_F))
		{
			m_torchEnabled = !m_torchEnabled;
			if(!m_torchEnabled)m_torch->setColor(&Vec4(0,0,0,0));
			Sleep(300);
			if(m_torchEnabled)gEngine.renderer->addTxtF("Turned torch on...");
		}
	}
}

void World::addToWorld(INode *entity, CollisionShapeType collShape, f32 density, CollisionGroup collGroup, Mesh* collisionMesh, bool dynamic)
{
	if(!entity)
		return;
	EntityData *EDtemp = new EntityData;
	Mesh* meshToCheck = entity->mesh;
	if(collisionMesh)
		meshToCheck = collisionMesh;
	//temp->m_actor = dynamics->createBox(NxVec3(entity->position), NxVec3(entity->boundingBox.getWidth()/2, entity->boundingBox.getHeight()/2, entity->boundingBox.getDepth()/2), 1);
	//u32 dynamic = 1;
	if(!meshToCheck)
		collShape = NO_COLLISION;
	switch(collShape)
	{
		case BOX:
			EDtemp->m_actor = core.dynamics->createBox(NxVec3(entity->position), NxVec3(meshToCheck->getBoundingBox().getWidth()/2, meshToCheck->getBoundingBox().getHeight()/2, meshToCheck->getBoundingBox().getDepth()/2), density);
			break;
		case SPHERE:
			EDtemp->m_actor = core.dynamics->createSphere(NxVec3(entity->position), meshToCheck->getBoundingBox().BSphere.Radius, density);
			break;
		case TRIANGLE:
			{
				//dynamic = 0;
				if(!density)
				{
					EDtemp->m_actor = core.dynamics->createActor(meshToCheck, &entity->world, 0, TRIANGLE, 0);
				}
				else
				{
					gEngine.kernel->log->prnEx(LT_WARNING, "World", "Triangle collision detection is reserved for static objects. Setting to BOX.");
					EDtemp->m_actor = core.dynamics->createBox(NxVec3(entity->position), NxVec3(meshToCheck->getBoundingBox().getWidth()/2, meshToCheck->getBoundingBox().getHeight()/2, meshToCheck->getBoundingBox().getDepth()/2), density);
				}
			}
			break;
		case NO_COLLISION:
			//dynamic = 0;
			EDtemp->m_actor = NULL;
			break;
	}
	entity->ptr_userData = NULL;
	if(EDtemp->m_actor)
	{
		dynamic = true;
		SetActorCollisionGroup(EDtemp->m_actor, collGroup);
		entity->ptr_userData = (void*)EDtemp;
		EDtemp->m_actor->userData = (void*)new AdditionalData(dynamic ? DT_DYNAMIC : DT_STATIC, NULL);
	}
	//m_scene->add(entity, EDtemp->m_actor ? true : false);
	if(entity->isSurface)
		m_scene->add((Surface*)entity, dynamic);
	else
		m_scene->add((IEntity*)entity, dynamic);
}

void World::addToWorld(Light* light)
{
	if(light)
		m_scene->add(light);
}

void World::addToWorld(IAgriDevice *device)
{
	if(device)
		m_agriDevices.push_back(device);
}

void World::addToWorld(ITrailer *trailer)
{
	if(trailer)
		m_trailers.push_back(trailer);
}

void World::addToWorld(HarvestShop *shop)
{
	if(shop)
		m_harvestShops.push_back(shop);
}

void World::addToWorld(IDeviceShop *shop)
{
	if(shop)
		m_deviceShops.push_back(shop);
}

void World::addToWorld(IVehicle *vehicle)
{
	if(vehicle)
		m_vehicles.push_back(vehicle);
}

void World::addTerrainToWorld(const string& texHmName, const string& texIndName)
{
	if(!m_terrainManager->load(texHmName, texIndName))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "World", "Adding terrain to world failed...");
		return;
	}
	NxActor* gTerrain;
	NxTriangleMeshDesc terrainDesc;
	terrainDesc.numVertices					= core.game->getWorld()->getTerrainManager()->m_numVertices;
	terrainDesc.numTriangles				= core.game->getWorld()->getTerrainManager()->m_numIndices/3;
	terrainDesc.pointStrideBytes			= sizeof(NxVec3);
	terrainDesc.triangleStrideBytes			= 3*sizeof(NxU32);
		Vec3 *points = new Vec3 [ terrainDesc.numVertices ];
		for(u32 i = 0 ; i < terrainDesc.numVertices; i++)points[i] = core.game->getWorld()->getTerrainManager()->m_vertices[i].pos;
	terrainDesc.points						= points;
	terrainDesc.triangles					= core.game->getWorld()->getTerrainManager()->m_indices;
	terrainDesc.flags						= 0;
	terrainDesc.heightFieldVerticalAxis		= NX_Y;
	terrainDesc.heightFieldVerticalExtent	= -1000.0f;
	NxTriangleMesh *terrainMesh;

	if(!gEngine.kernel->fs->exists("PhysXdata\\terr_cooked.bin"))
	{
		gEngine.kernel->log->prnEx(LT_INFO, "World", "Cooking terrain...");
		if(!core.dynamics->getCooking()->NxCookTriangleMesh(terrainDesc, FileStream("PhysXdata\\terr_cooked.bin", false)))
		{
			gEngine.kernel->log->prnEx(LT_ERROR, "World", "Cooking failed. Adding terrain to world failed...");
			return;
		}
	}
	terrainMesh = core.dynamics->getSDK()->createTriangleMesh(FileStream("PhysXdata\\terr_cooked.bin", true));


	NxTriangleMeshShapeDesc terrainShapeDesc;
	terrainShapeDesc.meshData		= terrainMesh;
	terrainShapeDesc.shapeFlags		= NX_SF_FEATURE_INDICES;
	terrainShapeDesc.materialIndex	= 0;

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&terrainShapeDesc);
	gTerrain = core.dynamics->getScene()->createActor(ActorDesc);
	if(gTerrain)
		SetActorCollisionGroup(gTerrain, GROUP_COLLIDABLE_NON_PUSHABLE);
	gTerrain->userData = (void*)new AdditionalData(DT_STATIC, NULL);
	delete(points);
}

void World::newTurn()
{
	m_dayNightManager->setTime(4, 0);
	m_grassManager->newTurn();
}
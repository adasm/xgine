/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */

#include "../XGine/XGine.h"
#pragma comment(lib, "../Debug/XGine.lib")

struct XGineFramework
{
	Scene*		scene;
	FreeCamera*	camera;
	TerrainManager* terrainManager;
	
	void	init(HINSTANCE hInstance);
	u32		load();
	void	close();
	u32		loop();
	void	update();
	void	render();
};

XGineFramework xgine;
GUIMenu menu;
Vec4 colorB[2];
Vec4 colorF[2];
Vec4 cb, cf;
f32 fade = 0.0f;
u32 c = 0;
vector< RendObj* > obj;
Light* torch;
bool torchEnabled = false;
HWND g_hwnd;
u32 printedTEST = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	xgine.init(hInstance);
	if(!xgine.load())return -1;
	xgine.update();
	while(true)
	{
		if(!(xgine.loop() && !gEngine.input->isKeyDown(DIK_ESCAPE)))break;
		if(!printedTEST) { gEngine.kernel->log->prnEx(LT_DEBUG, "TEST", "Engine loaded in %fms", gEngine.kernel->tmr->getAppTime()); printedTEST = 1; }
	}
	xgine.close();
	return 0;
}

u32 XGineFramework::load()
{
	//ALLOCATION TEST

	/*
	gEngine.kernel->log->prnEx(LT_INFO, "AllocTEST", "***BEG ALLOCATION TEST***");
	__int64 *mem[5000];
	for(int i = 0; i < 5000; i++)
	{	mem[i] = 0; }

	f32 s, d1, d2;
	s = gEngine.kernel->tmr->getAppTime();
	mem[0] = new __int64 [ 5000 * 5000 ];
	d1 = gEngine.kernel->tmr->getAppTime() - s;
	gEngine.kernel->log->prnEx(LT_INFO, "AllocTEST", "  -> block of 5000 * 5000 * sizeof(__int64) memory -> Time: %fms", d1);
	delete mem[0]; mem[0] = 0;

	

	for(int j = 5000; j > 1; j/=2)
	{
		s = gEngine.kernel->tmr->getAppTime();
		for(int i = 0; i < 5000 * 5000 / j; i++)
			mem[i] = new __int64 [ j ];
		d2 = gEngine.kernel->tmr->getAppTime() - s;	
		gEngine.kernel->log->prnEx(LT_INFO, "AllocTEST", "  -> %i blocks of  %i * sizeof(__int64) memory -> Time: %fms", 5000 * 5000 /j, j, d2);

		for(int i = 0; i < 5000; i++)
		{	delete mem[i]; mem[i] = 0; }
	}

	gEngine.kernel->log->prnEx(LT_INFO, "AllocTEST", "***END ALLOCATION TEST***");
	*/


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//i32 id = gEngine.audio->addSound("audio/drumloop.wav");
	//gEngine.audio->addSoundSource(id, LOOPING);
	//gEngine.audio->setSourcePosition(id, Vec3(0,500,0));
	////gEngine.audio->setSourceAttenuation(id, 20, 50);
	//gEngine.audio->play(id);
	//scene->add(new Surface("sponza.x", new Material(MT_DEFAULT, "brick2.jpg"), Vec3(0,500,0), Vec3(0,0,0), 100), 0);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	torch = new Light(LIGHT_SPOT, 1, Vec4(1.5,1.5,1.5, 1), *camera->getPosition(), 2000, *camera->getViewDirection(), D3DX_PI/4);
	torch->setColor(&Vec4(0,0,0,0));
	scene->add(torch);

	terrainManager  = new TerrainManager(scene);

	terrainManager->updateBinLoad("terrain/heightmap.bmp", "terrain/indexMap4096.bmp", "data/hm.bin", "data/im.bin", 512);

	scene->addWater(14);



	/*string cacheName = "data/cache.dat";
	gEngine.kernel->fs->addSrc(cacheName, "XPAK");

	FSPakFileInfo file;
	file.compType = COMP_ZLIB;
	file.packedFileName = file.fileName = "data/im.bin";
	gEngine.kernel->fs->addToArchive(cacheName, file);
	file.packedFileName = file.fileName = "data/hm.bin";
	gEngine.kernel->fs->addToArchive(cacheName, file);

	gEngine.kernel->fs->updateArchive(cacheName);*/

	colorF[0] = Vec4(0.8,0.7,0.2,1);
	colorB[0] = Vec4(0.6,0.6,0.8,1);

	colorF[1] = Vec4(1,1,1,0.6);
	colorB[1] = Vec4(0.8,0.5,0.1,1);


	cf = colorF[0];
	cb = colorB[0];

	string name[6];
	name[0] = "box.x";
	name[1] = "box1.x";
	name[2] = "box2.x";
	name[3] = "box3.x";
	name[4] = "box4.x";
	name[5] = "box5.x";

	string colorMaps[6];
	colorMaps[0] = "asfalt.jpg";
	colorMaps[1] = "branch.png";
	colorMaps[2] = "brick2.jpg";
	colorMaps[3] = "brickbump.jpg";
	colorMaps[4] = "rockbump.jpg";
	colorMaps[5] = "rockwall.jpg";

	string normalMaps[6];
	normalMaps[0] = "asfalt_n.png";
	normalMaps[1] = "branch_n.png";
	normalMaps[2] = "brick2_n.tga";
	normalMaps[3] = "brickbump.tga";
	normalMaps[4] = "rockbump.tga";
	normalMaps[5] = "rockwall.tga";
	
	srand(timeGetTime());

	////////////////////////////////////////////////////////////////////////////////////

	//scene->load();
	
	////////////////////////////////////////////////////////////////////////////////////////

	//Surface* ent = new Surface("waterPlane4.x", new Material(MT_DEFAULT, "rockbump.jpg", "rockbump.tga", "", , 1, 50, 1, 3), Vec3(0,100,0), Vec3(0,0,0), 13);
	//scene->add(ent, 0);

	DISPLACEMENT_TYPE dispType = DT_BUMP_MAPPING;

	/*Surface* ent;

	ent = new Surface("%gm plane 300 1", new Material(MT_DEFAULT, colorMaps[0], normalMaps[0], "", 3, 1, 60, 1, dispType), Vec3(-300.0f*0-50.0f,300,-0), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 300 2", new Material(MT_DEFAULT, colorMaps[1], normalMaps[1], "", 3, 1, 60, 1, dispType), Vec3(-300.0f*1-50.0f,300,-0), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 300 3", new Material(MT_DEFAULT, colorMaps[2], normalMaps[2], "", 3, 1, 60, 1, dispType), Vec3(-300.0f*2-50.0f,300,-0), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 300 4", new Material(MT_DEFAULT, colorMaps[3], normalMaps[3], "", 3, 1, 60, 1, dispType), Vec3(-300.0f*3-50.0f,300,-0), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 300 5", new Material(MT_DEFAULT, colorMaps[4], normalMaps[4], "", 3, 1, 60, 1, dispType), Vec3(-300.0f*4-50.0f,300,-0), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 300 6", new Material(MT_DEFAULT, colorMaps[5], normalMaps[5], "", 3, 1, 60, 1, dispType), Vec3(-300.0f*5-50.0f,300,-0), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);

	dispType = DT_PARALLAX_MAPPING;
	ent = new Surface("%gm plane 350 1", new Material(MT_DEFAULT, colorMaps[0], normalMaps[0], "", 3, 1, 60, 1, dispType), Vec3(-350.0f*0-50.0f,300,-1000), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 350 2", new Material(MT_DEFAULT, colorMaps[1], normalMaps[1], "", 3, 1, 60, 1, dispType), Vec3(-350.0f*1-50.0f,300,-1000), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 350 3", new Material(MT_DEFAULT, colorMaps[2], normalMaps[2], "", 3, 1, 60, 1, dispType), Vec3(-350.0f*2-50.0f,300,-1000), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 350 4", new Material(MT_DEFAULT, colorMaps[3], normalMaps[3], "", 3, 1, 60, 1, dispType), Vec3(-350.0f*3-50.0f,300,-1000), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 350 5", new Material(MT_DEFAULT, colorMaps[4], normalMaps[4], "", 3, 1, 60, 1, dispType), Vec3(-350.0f*4-50.0f,300,-1000), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 350 6", new Material(MT_DEFAULT, colorMaps[5], normalMaps[5], "", 3, 1, 60, 1, dispType), Vec3(-350.0f*5-50.0f,300,-1000), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);

	dispType = DT_PARALLAX_OCCLUSION_MAPPING;
	ent = new Surface("%gm plane 400 1", new Material(MT_DEFAULT, colorMaps[0], normalMaps[0], "", 3, 1, 60, 1, dispType), Vec3(-400.0f*0-50.0f,300,-2000), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 400 2", new Material(MT_DEFAULT, colorMaps[1], normalMaps[1], "", 3, 1, 60, 1, dispType), Vec3(-400.0f*1-50.0f,300,-2000), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 400 3", new Material(MT_DEFAULT, colorMaps[2], normalMaps[2], "", 3, 1, 60, 1, dispType), Vec3(-400.0f*2-50.0f,300,-2000), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 400 4", new Material(MT_DEFAULT, colorMaps[3], normalMaps[3], "", 3, 1, 60, 1, dispType), Vec3(-400.0f*3-50.0f,300,-2000), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 400 5", new Material(MT_DEFAULT, colorMaps[4], normalMaps[4], "", 3, 1, 60, 1, dispType), Vec3(-400.0f*4-50.0f,300,-2000), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);
	ent = new Surface("%gm plane 400 6", new Material(MT_DEFAULT, colorMaps[5], normalMaps[5], "", 3, 1, 60, 1, dispType), Vec3(-400.0f*5-50.0f,300,-2000), Vec3(1.2,0,0), 1);
	scene->add(ent, 0);*/


	/*ent = new Surface("sponza.x", new Material(MT_DEFAULT, colorMaps[5], normalMaps[5], "", 3, 1, 60, 1, dispType), Vec3(-500.0f*10-50.0f,300,-50), Vec3(0,0,0), 10);
	scene->add(ent, 0);*/

	u32 stepY = 1000, stepX = 1000;
	for(u32 y = 0; y < 8000; y+=stepY)
	{
		for(u32 x = 0; x < 8000; x+=stepX)
		{
			if( ( y >= 1500 && y <= 3500 ) && ( x >= 1500 && x <= 3500 ) )stepY = (u32)100 + (u32)rand()%100;
			else stepY = (u32)1000 + (u32)rand()%1000;

			if( ( y >= 1500 && y <= 3500 ) && ( x >= 1500 && x <= 3500 ) )stepX = (u32)100 + (u32)rand()%100;
			else stepX = (u32)1000 + (u32)rand()%1000;

			u32 r;
			/*if( ( y >= 1500 && y <= 3500 ) && ( x >= 1500 && x <= 3500 ) )r = (u32)rand()%2+1;
			else */r = 0;

			
			

			//RendObj* ent1 = new RendObj(name[1], new Material("brick2.jpg", "", "", 1, 1, 0.015, 50, Vec4(1,1,1,1), 1, 1), Vec3(x,500,y));
			//RendObj* ent2 = new RendObj(name[2], new Material("brick2.jpg", "", "", 1, 1, 0.015, 50, Vec4(1,1,1,1), 1, 1), Vec3(x,500,y));
			//RendObj* ent3 = new RendObj(name[3], new Material("brick2.jpg", "", "", 1, 1, 0.015, 50, Vec4(1,1,1,1), 1, 1), Vec3(x,500,y));

			//ent->addLOD(ent1, 200);
			//ent->addLOD(ent2, 400);
			//ent->addLOD(ent3, 600);

			
			
			Vec3 pos = Vec3(x,60,y);

			if(r == 0)
			{
				int height = 150 + rand()%750;
				for(int z = 60; z < height; z+=65)
				{
					u32	materialID = rand()%6;
					Surface* ent = new Surface(name[materialID], new Material(MT_DEFAULT, colorMaps[materialID], normalMaps[materialID], "", 1, 1, 50, 1, dispType), Vec3(x,z,y), Vec3(0,0,0), 13);
					scene->add(ent, 1);
				}
			}
			else if(r == 1)
			{
				f32 scale = rand()%2+2;
				Surface *shrub = new Surface("veg/shrub.x", new Material(MT_LEAF, "veg/shrub.png", "veg/shrub_n.png"), pos, Vec3(0,0,0), scale);
				scene->add(shrub, 0);
			}
			else if(r == 2)
			{
				f32 scale = rand()%2+1;
				Surface *fern = new Surface("veg/fern.x", new Material(MT_LEAF, "veg/fern.png", "veg/fern_n.png"), pos, Vec3(0,0,0), scale);
				scene->add(fern, 0);
			}
		}
	}
	return 1;
}

u32 XGineFramework::loop()
{
	if(gEngine.device->loop() && gEngine.device->afterLoop())
	{
		update();
		scene->preRender();
		if(gEngine.renderer->begin()){
				render();
			gEngine.renderer->end();
		}
		return 1;
	}
	return 0;
}

void XGineFramework::render()
{
	scene->render();
}

void XGineFramework::update()
{
	gEngine.kernel->tmr->update();
	gEngine.input->update();
	Vec3 move(0,0,0), rotate(0,0,0);
	if(gEngine.desktop->isFocused())
	{
		float speed = 1.0f;

		if(gEngine.input->isKeyDown(DIK_LSHIFT))
			speed = 0.05f;

		if(gEngine.input->isKeyDown(DIK_W))
			move.z = speed*(float)gEngine.kernel->tmr->get();
		else if(gEngine.input->isKeyDown(DIK_S))
			move.z = -speed*(float)gEngine.kernel->tmr->get();
		if(gEngine.input->isKeyDown(DIK_A))
			move.x = speed*(float)gEngine.kernel->tmr->get();
		else if(gEngine.input->isKeyDown(DIK_D))
			move.x = -speed*(float)gEngine.kernel->tmr->get();
		if(gEngine.input->isMousePressed(0))
			{ rotate.x = gEngine.input->getMouseRel()->y*0.003f; rotate.y = -gEngine.input->getMouseRel()->x*0.003f; }	
		camera->moveByRel(move);
		camera->rotateBy(rotate);

		if(torchEnabled)
		{
			Vec3 left = Vec3(camera->getViewLeft()->x, camera->getViewLeft()->y, camera->getViewLeft()->z);
			Vec3 up = Vec3(camera->getViewUp()->x, camera->getViewUp()->y, camera->getViewUp()->z);
			torch->setPos(&(*camera->getPosition()+left));
			torch->setDir(camera->getViewDirection());
			torch->setColor(&Vec4(1,1,1,1));
		}
		torch->setEnabled(torchEnabled);
		
		if(gEngine.input->isKeyPressed(DIK_F))
		{
			torchEnabled = !torchEnabled;
			if(!torchEnabled)torch->setColor(&Vec4(0,0,0,0));
			//Sleep(300);
		}

		if(gEngine.input->isKeyPressed(DIK_C))
		{
			scene->add(new Light(LIGHT_POINT, 1, Vec4(1,1,1, 1), *camera->getPosition(), 100));
			//Sleep(100);
		}
		if(gEngine.input->isKeyPressed(DIK_V))
		{
			scene->add(new Light(LIGHT_SPOT, 1, Vec4(1,1,1, 1), *camera->getPosition(), 2000, *camera->getViewDirection(), D3DX_PI/4));
			//Sleep(100);
		}

		if(gEngine.input->isKeyPressed(DIK_B))
		{
			scene->add(new Light(LIGHT_DIR, 1, Vec4(1,1,1, 1), *camera->getPosition(), 100, *camera->getViewDirection(), D3DX_PI/3));
			//Sleep(100);
		}

		if(gEngine.input->isKeyPressed(DIK_X))
		{
		//	const u32 bs = 100;
		//	Vec3 positions[bs*bs];
		//	for(u32 x = 0; x < bs; x++)
		//	{
		//		for(u32 y = 0; y < bs; y++)
		//		{
		//			positions[y*bs+x] = *camera->getPosition() + Vec3(x * 7 + rand()%5, -2, y*7 + rand()%5);
		//		}
		//	}
		//	scene->add(new Vegetation("grassWalpha.png", positions, bs*bs, 10, BILLBOARDROT_LEFT), 1);
		//	scene->add(new Particle(&Vec3(*camera->getPosition()), "particle2.png", PARTICLEB_DEF, 1000, 1000, 0, &Vec3(20,0,20), &Vec3(90000,0,90000), &Vec3(20000,10,20000), &Vec4(0.6,0.6,0.6,0.3), &Vec4(0.6,0.6,0.6,0.3), &Vec4(0,0,0,0.1),700, 700, 300, 1.5, 2, BILLBOARDROT_ALL), 1);
		//	//Sleep(100);
		}

		//if(gEngine.input->isKeyDown(DIK_Q))
		//{
		//	Vec3 p = *scene->getCamera()->getPosition();
		//	u32 x = (u32)p.x;
		//	u32 y = (u32)p.z;
		//	if(x < 6 || y < 6 || x > terrainManager->m_size*terrainManager->m_hmGeoStep-6 || y > terrainManager->m_size*terrainManager->m_hmGeoStep-6);
		//	else
		//	{
		//		Vec4 rect = Vec4(x-5,y-5,x+5,y+5);
		//		terrainManager->lockIndexTexture(rect);
		//		//for(i32 n = -5; n < 6; n++)
		//		//  for(i32 m = -5; m < 6; m++)
		//				terrainManager->setIndexTextureTexel(x, y, &Vec4(1, 1, 1, 0));
		//		terrainManager->unlockIndexTexture(rect);
		//	}
		//}

		
		/*if(gEngine.input->isKeyDown(DIK_E))
		{
			Vec3 p = *scene->getCamera()->getPosition();
			u32 x = (u32)p.x;
			u32 y = (u32)p.z;
			if(x < 0 || y < 0 || x > terrainManager->m_size-1 || y > terrainManager->m_size-1);
			else
			{
				terrainManager->lockIndexTexture();
				terrainManager->setIndexTextureTexel(x, y, &Vec4(1, 1, 1, 0));
				terrainManager->unlockIndexTexture();
			}
		}*/

		/*if(gEngine.input->isKeyDown(DIK_U))
		{
			terrainManager->create("terrain/heightmap.bmp", "terrain/indexMap4096.bmp", 512);
		}*/
		

		/*if(gEngine.input->isKeyDown(DIK_T))
		{
			terrainManager->save("data/hm.bin");
			terrainManager->saveIndTex("data/im.bin");
			Sleep(100);
		}

		if(gEngine.input->isKeyDown(DIK_Y))
		{
			terrainManager->load("data/hm.bin", "data/im.bin");
			Sleep(100);
		}

		if(gEngine.input->isKeyDown(DIK_U))
		{
			terrainManager->saveHmAsXMesh("hmXMesh.x");
			Sleep(100);
		}*/


	}

	menu.update();

	camera->update();

	terrainManager->update(camera);

	for(u32 i = 0; i < obj.size(); i ++)
	{
		Vec3 old = obj[i]->position;
		obj[i]->position = Vec3(old.x, 30.0f + sin(timeGetTime()*0.0001f + 4*old.x + 2*old.z) * 20.0f, old.z);
		obj[i]->rotation = Vec3(timeGetTime()*0.0005f+old.x, timeGetTime()*0.0001f+old.x, timeGetTime()*0.0003f+old.z);
		worldMat(&obj[i]->world, obj[i]->position, obj[i]->rotation);
		obj[i]->boundingBox.setTransform(obj[i]->position);

		//obj[i]->getMaterial()->m_diffuseColor = Vec4( old.y  / 50.0f, 1 - old.y  / 50.0f, old.x / 1024, 1);
	}

	scene->update();
}

void XGineFramework::init(HINSTANCE hInstance)
{
	gEngine.init();
	gEngine.kernel->init();
	gEngine.loadPluginCfg("plugins.txt");
	gEngine.shaderMgr->init();
	//gEngine.kernel->con->initWnd(hInstance);
	RenderWindowInfo rendInfo;
	gEngine.device->createRendWnd(hInstance, &rendInfo);
	g_hwnd = rendInfo.hMainWnd;
	gEngine.device->init(&rendInfo);
	gEngine.input->init();
	gEngine.renderer->init();

	camera = new FreeCamera(Vec3(-399, 1313, -595), Vec3(-399, 1313, -595)+Vec3(0.619488, -0.397217, 0.677102));
	scene = new Scene(camera);

	menu.init();
	
	gEngine.desktop->m_Visible = true;
	gEngine.desktop->setFocus(gEngine.desktop);
}

void XGineFramework::close()
{
	delete(terrainManager);
	delete(scene);
	gEngine.renderer->close();
	gEngine.input->close();
	gEngine.resMgr->close();
	gEngine.shaderMgr->close();
	gEngine.device->close();
	gEngine.kernel->close();
	gEngine.close();
}

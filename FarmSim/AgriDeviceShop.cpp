//#include "AgriDeviceShop.h"
//
//AgriDeviceShop::AgriDeviceShop(Vec3 pos, ShopParams *shopParams)
//{
//	m_parkingPos = shopParams->parkingPos + pos;
//	m_parkingColumns = shopParams->parkingColumns;
//	m_parkingRows = shopParams->parkingRows;
//	m_actualItem = 0;
//	m_camera = new FreeCamera(shopParams->cameraPos, shopParams->cameraLookAt);
//	m_camera->lookAt(shopParams->cameraLookAt);
//	m_camera->update();
//	m_scene = new Scene(m_camera, 0);
//	m_entryPointRadius = shopParams->entryPointRadius;
//	m_entryPoint = shopParams->entryPoint;
//	m_itemDisplayPos = shopParams->itemDisplayPos;
//	/*if(m_entryPoint.y == 0.0f)
//		m_entryPoint.y = core.game->getWorld()->getGrassManager()->getTerrainHeight(m_entryPoint.x, m_entryPoint.z);*/
//	m_entryPoint += pos;
//	ObjectParams temp;
//	m_scene->add(new Light(LIGHT_POINT));
//	for(int i = 0; i < shopParams->objectsOutside.size(); i++)
//	{
//		temp.loadFromFile("Objects\\" + shopParams->objectsOutside[i]->fname);
//		shopParams->objectsOutside[i]->pos += pos;
//		/*if(shopParams->objectsOutside[i]->pos.y == 0.0f)
//			shopParams->objectsOutside[i]->pos.y = core.game->getWorld()->getGrassManager()->getTerrainHeight(shopParams->objectsOutside[i]->pos.x, shopParams->objectsOutside[i]->pos.z);*/
//		Surface* rend = new Surface(temp.meshName, temp.generateMaterial(), shopParams->objectsOutside[i]->pos);
//		//core.game->getWorld()->addToWorld(rend, temp.getCollisionShape(), temp.density, temp.getCollisionGroup());
//		core.game->getWorld()->addToWorld(rend, TRIANGLE, 0, GROUP_COLLIDABLE_NON_PUSHABLE);
//	}
//	for(int i = 0; i < shopParams->objectsInside.size(); i++)
//	{
//		temp.loadFromFile("Objects\\" + shopParams->objectsInside[i]->fname);
//		Surface* rend = new Surface(temp.meshName, temp.generateMaterial(), shopParams->objectsInside[i]->pos);
//		m_scene->add(rend, 0);
//	}
//	for(int i = 0; i < shopParams->lights.size(); i++)
//	{
//		shopParams->lights[i]->pos += pos;
//		//if(sceneParams->lights[i]->pos.y <= 0)
//			//sceneParams->lights[i]->pos.y = m_grassManager->getTerrainHeight(sceneParams->lights[i]->pos.x, sceneParams->lights[i]->pos.z) - sceneParams->lights[i]->pos.y;
//		core.game->getWorld()->getDayNightManager()->addLight(shopParams->lights[i]->fname, shopParams->lights[i]->pos, shopParams->lights[i]->rotate);
//	}
//	for(int i = 0; i < shopParams->shopItems.size(); i++)
//	{
//		m_items.push_back(shopParams->shopItems[i]);
//		VehicleParams *vehTemp = new VehicleParams();
//
//		string vehicleEngine;
//		string wheelFrontLeftModel;
//		string wheelFrontRightModel;
//		string wheelRearLeftModel;
//		string wheelRearRightModel;
//		string chassisModel;
//
//		vehTemp->getVariableList()->push_back(new CfgVariable(V_STRING, &vehicleEngine, getVarName(vehicleEngine)));
//		vehTemp->getVariableList()->push_back(new CfgVariable(V_STRING, &wheelFrontLeftModel, getVarName(wheelFrontLeftModel)));
//		vehTemp->getVariableList()->push_back(new CfgVariable(V_STRING, &wheelFrontRightModel, getVarName(wheelFrontRightModel)));
//		vehTemp->getVariableList()->push_back(new CfgVariable(V_STRING, &wheelRearLeftModel, getVarName(wheelRearLeftModel)));
//		vehTemp->getVariableList()->push_back(new CfgVariable(V_STRING, &wheelRearRightModel, getVarName(wheelRearRightModel)));
//		vehTemp->getVariableList()->push_back(new CfgVariable(V_STRING, &chassisModel, getVarName(chassisModel)));
//		CfgLoader cfg(m_items[i]->fname, vehTemp->getVariableList());
//		VehicleEngine vehEng(vehicleEngine);
//
//		m_items[i]->fuelCapacity = vehTemp->fuelCapacity;
//		m_items[i]->fuelConsumptionPerMth = vehTemp->fuelConsumptionPerMth;
//		m_items[i]->gearNum = vehEng.getGearNum();
//		switch(vehTemp->wheelDriveType)
//		{
//			case 0:
//				m_items[i]->wdt = WDT_2WD;
//				break;
//			case 1:
//				m_items[i]->wdt = WDT_4WD;
//				break;
//			case 2:
//				m_items[i]->wdt = WDT_FWD;
//				break;
//		}
//		Vec3 chassisPos = Vec3(0, vehTemp->frontWheelsRadius - vehTemp->posFrontLeftWheel.y, 0);
//		//Vec3 chassisPos = Vec3(0, 0, 0);
//		ObjectParams temp;
//		Surface* obj = NULL;
//		temp.loadFromFile("Objects\\" + chassisModel);
//		obj = new Surface(temp.meshName, temp.generateMaterial(), chassisPos + Vec3(0, 100000, 0));
//		if(obj)
//			m_items[i]->SurfaceSet.push_back(obj);
//
//
//		temp.loadFromFile("Objects\\" + wheelFrontLeftModel);
//		obj = new Surface(temp.meshName, temp.generateMaterial(), vehTemp->posFrontLeftWheel + chassisPos/* + Vec3(0, vehTemp->frontWheelsRadius * 0.5f, 0)*/ + Vec3(0, 100000, 0));
//		if(obj)
//			m_items[i]->SurfaceSet.push_back(obj);
//		temp.loadFromFile("Objects\\" + wheelFrontRightModel);
//		obj = new Surface(temp.meshName, temp.generateMaterial(), vehTemp->posFrontRightWheel + chassisPos/* + Vec3(0, vehTemp->frontWheelsRadius * 0.5f, 0)*/ + Vec3(0, 100000, 0));
//		if(obj)
//			m_items[i]->SurfaceSet.push_back(obj);
//		temp.loadFromFile("Objects\\" + wheelRearLeftModel);
//		obj = new Surface(temp.meshName, temp.generateMaterial(), vehTemp->posRearLeftWheel + chassisPos/* + Vec3(0, vehTemp->rearWheelsRadius * 0.5f, 0)*/ + Vec3(0, 100000, 0));
//		if(obj)
//			m_items[i]->SurfaceSet.push_back(obj);
//		temp.loadFromFile("Objects\\" + wheelRearRightModel);
//		obj = new Surface(temp.meshName, temp.generateMaterial(), vehTemp->posRearRightWheel + chassisPos/* + Vec3(0, vehTemp->rearWheelsRadius * 0.5f, 0)*/ + Vec3(0, 100000, 0));
//		if(obj)
//			m_items[i]->SurfaceSet.push_back(obj);
//		for(int z = 0; z < m_items[i]->SurfaceSet.size(); z++)
//		{
//			m_scene->add(m_items[i]->SurfaceSet[z]);
//		}
//	}
//	for(int i = 0; i < m_items[m_actualItem]->SurfaceSet.size(); i++)
//	{
//		setSurfaceTransform(m_items[m_actualItem]->SurfaceSet[i], m_items[m_actualItem]->SurfaceSet[i]->position - Vec3(0, 100000, 0), Vec3(0, 0, 0), 0);
//	}
//}
//
//void AgriDeviceShop::nextItem()
//{
//	for(int i = 0; i < m_items[m_actualItem]->SurfaceSet.size(); i++)
//	{
//		setSurfaceTransform(m_items[m_actualItem]->SurfaceSet[i], m_items[m_actualItem]->SurfaceSet[i]->position + Vec3(0, 100000, 0), Vec3(0, 0, 0), 0);
//	}
//	if(m_actualItem < m_items.size() - 1)
//		m_actualItem++;
//	else
//		m_actualItem = 0;
//	for(int i = 0; i < m_items[m_actualItem]->SurfaceSet.size(); i++)
//	{
//		setSurfaceTransform(m_items[m_actualItem]->SurfaceSet[i], m_items[m_actualItem]->SurfaceSet[i]->position - Vec3(0, 100000, 0), Vec3(0, 0, 0), 0);
//	}
//}
//
//void AgriDeviceShop::previousItem()
//{
//	for(int i = 0; i < m_items[m_actualItem]->SurfaceSet.size(); i++)
//	{
//		setSurfaceTransform(m_items[m_actualItem]->SurfaceSet[i], m_items[m_actualItem]->SurfaceSet[i]->position + Vec3(0, 100000, 0), Vec3(0, 0, 0), 0);
//	}
//	if(m_actualItem > 0)
//		m_actualItem--;
//	else
//		m_actualItem = m_items.size()-1;
//	for(int i = 0; i < m_items[m_actualItem]->SurfaceSet.size(); i++)
//	{
//		setSurfaceTransform(m_items[m_actualItem]->SurfaceSet[i], m_items[m_actualItem]->SurfaceSet[i]->position - Vec3(0, 100000, 0), Vec3(0, 0, 0), 0);
//		//setSurfaceTransform(m_items[m_actualItem]->SurfaceSet[i], m_items[m_actualItem]->SurfaceSet[i]->position - Vec3(0, 100000, 0), Vec3(0, 0, 0), 0);
//	}
//}
//
//void AgriDeviceShop::update()
//{
//	static float angle = 0;
//	if(angle >= (3.14f) * 2)
//		angle -= angle;
//	angle += gEngine.kernel->tmr->get() * 0.001f;
//	Mat rot, trans;
//	D3DXMatrixRotationY(&rot, angle);
//	D3DXMatrixTranslation(&trans, m_itemDisplayPos.x, m_itemDisplayPos.y, m_itemDisplayPos.z);
//	gEngine.renderer->addTxt("Angle: %.3f", angle);
//	for(int i = 0; i < m_items[m_actualItem]->SurfaceSet.size(); i++)
//	{
//		setSurfaceTransform(m_items[m_actualItem]->SurfaceSet[i], m_items[m_actualItem]->SurfaceSet[i]->position, Vec3(0, 0, 0), 0);
//		m_items[m_actualItem]->SurfaceSet[i]->rotation = Vec3(0, angle, 0);
//		m_items[m_actualItem]->SurfaceSet[i]->world = m_items[m_actualItem]->SurfaceSet[i]->world * rot * trans;
//	}
//	m_scene->update();
//}
//
//void AgriDeviceShop::updateInput()
//{
//	if(core.game->getKeyboardManager()->isAction1())
//		nextItem();
//	if(core.game->getKeyboardManager()->isAction2())
//		previousItem();
//	if(core.game->getKeyboardManager()->isAttachToVehicle())
//		buyItem();
//}
//
//void AgriDeviceShop::buyItem()
//{
//	Vec3 parkingPos = Vec3(0, 0, 0);
//	float interval = 12.0f;
//	bool alreadyUsed = false;
//	int x = 0, y = 0;
//	while(!checkIfParkingIsFree(x, y, interval) && y < m_parkingRows)
//	{
//		if(x < m_parkingColumns)
//			x++;
//		else
//		{
//			x = 0;
//			y++;
//		}
//	}
//	if(y < m_parkingRows)
//	{
//		parkingPos = Vec3(x * interval, 10, y * interval) + m_parkingPos;
//		if(core.game->getWorld()->getPlayerInfo()->getMoney() > m_items[m_actualItem]->price)
//		{
//			core.game->getWorld()->getPlayerInfo()->addMoney(-m_items[m_actualItem]->price);
//			if(m_items[m_actualItem]->typeName == "Tractor")
//				core.game->getWorld()->addToWorld(new Tractor(parkingPos, m_items[m_actualItem]->fname));
//			if(m_items[m_actualItem]->typeName == "Combine")
//				core.game->getWorld()->addToWorld(new Combine(parkingPos, m_items[m_actualItem]->fname));
//		}
//	}
//}
//
//bool AgriDeviceShop::checkIfParkingIsFree(int x, int y, float interval)
//{
//	Vec3 pos = Vec3(x * interval, 0, y * interval) + m_parkingPos;
//	for(int i = 0; i < core.game->getWorld()->getVehicles()->size(); i++)
//	{
//		Vec3 vehPos = Vec3((*core.game->getWorld()->getVehicles())[i]->getVehicleController()->getActor()->getGlobalPosition().x, (*core.game->getWorld()->getVehicles())[i]->getVehicleController()->getActor()->getGlobalPosition().y, (*core.game->getWorld()->getVehicles())[i]->getVehicleController()->getActor()->getGlobalPosition().z);
//		if(D3DXVec3Length(&Vec3(vehPos - pos)) < interval)
//			return false;
//	}
//	return true;
//}
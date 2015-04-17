#include "Game.h"

void Game::init()
{
	m_keyboardManager = new KeyboardManager();
	core.dynamics->init();


	m_camera = new FreeCamera(Vec3(0, 5, 0), Vec3(10, 10, 10));
	m_world = new World;
	SceneParams temp;
	temp.loadFromFile("scene.txt");
	m_world->init(&temp);


	m_menu.init();
	//m_world->getScene()->getRendList()->sort();
	gEngine.desktop->m_Visible = true;
}

void Game::close()
{
	delete m_keyboardManager;
	m_world->close();
}

void Game::update()
{
	gEngine.renderer->addTxt("CamPos: %f, %f, %f", m_camera->getPosition()->x, m_camera->getPosition()->y, m_camera->getPosition()->z);
	gEngine.kernel->tmr->update();
	gEngine.input->update();
	Vec3 move(0,0,0), rotate(0,0,0);
	/*if(gEngine.input->isKeyDown(DIK_W))
		move.z = 0.05f*(float)gEngine.kernel->tmr->get();
	else if(gEngine.input->isKeyDown(DIK_S))
		move.z = -0.05f*(float)gEngine.kernel->tmr->get();
	if(gEngine.input->isKeyDown(DIK_A))
		move.x = 0.05f*(float)gEngine.kernel->tmr->get();
	else if(gEngine.input->isKeyDown(DIK_D))
		move.x = -0.05f*(float)gEngine.kernel->tmr->get();*/
	if(gEngine.input->isMousePressed(0))
		{ rotate.x = gEngine.input->getMouseRel()->y*0.003f; rotate.y = -gEngine.input->getMouseRel()->x*0.003f; }	
	//m_camera->moveByRel(move);
	if(m_world->getPlayerInfo()->getDrivingVehicle())
	{
		static float zoom = 4;
		static Vec3 mouse_rot = Vec3(0.785, 0, 0);

		Vec3 forward = m_world->getPlayerInfo()->getDrivingVehicle()->getVehicleController()->getForwardVec();
		zoom -= gEngine.input->getMouseZRel() / 100.0f;
		mouse_rot += rotate;

		if(zoom < 4)
			zoom = 4;
		if(zoom > 20)
			zoom = 20;

		if(mouse_rot.x < 0)
			mouse_rot.x = 0;
		if(mouse_rot.x > 1.56)
			mouse_rot.x = 1.56;
		Vec3 pos2;
		Vec3 pos = Vec3(m_world->getPlayerInfo()->getDrivingVehicle()->getVehicleController()->getActor()->getGlobalPosition().x, m_world->getPlayerInfo()->getDrivingVehicle()->getVehicleController()->getActor()->getGlobalPosition().y, m_world->getPlayerInfo()->getDrivingVehicle()->getVehicleController()->getActor()->getGlobalPosition().z);
		pos2 = pos;
		Mat mRot;
		Vec3 cross;
		D3DXVec3Cross(&cross, &forward, &Vec3(0, 1, 0));

		D3DXMatrixRotationAxis(&mRot, &cross, -mouse_rot.x);
		D3DXVec3TransformNormal(&forward, &forward, &mRot);

		D3DXMatrixRotationAxis(&mRot, &Vec3(0, 1, 0), mouse_rot.y);
		D3DXVec3TransformNormal(&forward, &forward, &mRot);

		pos += forward * zoom;
		pos.y += 1.5;
		m_camera->setPosition((forward * -zoom) + pos2);
		pos.y -= 1;
		m_camera->lookAt(pos + (m_world->getPlayerInfo()->getDrivingVehicle()->getVehicleController()->getForwardVec() * 2.50));
	}
	else
	{
		if(!m_world->getPlayerInfo()->isFreeFlyMode())
			m_camera->setPosition(Vec3(m_world->getPlayerInfo()->getCharController()->getCharacterActor()->getGlobalPosition().x, m_world->getPlayerInfo()->getCharController()->getCharacterActor()->getGlobalPosition().y, m_world->getPlayerInfo()->getCharController()->getCharacterActor()->getGlobalPosition().z));
		m_camera->rotateBy(rotate);
	}
	m_camera->update();

	m_menu.update();
}

void Game::proceed()
{
	m_keyboardManager->update();
	update();
	m_world->render();
}

BoundingBox* Game::getCameraBB()
{
	static BoundingBox* retVal = NULL;
	static Vec3* coneVerts = new Vec3[8];
	if(!retVal)
	{
		retVal = new BoundingBox();
	}

	memcpy((void*)coneVerts, (void*)m_camera->getFrustumVertices(), sizeof(Vec3) * 8);
	Vec3 min = Vec3(100000, 10000, 10000), max = Vec3(-10000, -10000, -10000);
	for(int i = 0; i < 8; i++)
	{
		coneVerts[i] -= *m_camera->getPosition();
		D3DXVec3TransformCoord(&coneVerts[i], &coneVerts[i], m_camera->getViewMat());
		if(min.x > coneVerts[i].x)
			min.x = coneVerts[i].x;
		if(min.y > coneVerts[i].y)
			min.y = coneVerts[i].y;
		if(min.z > coneVerts[i].z)
			min.z = coneVerts[i].z;

		if(max.x < coneVerts[i].x)
			max.x = coneVerts[i].x;
		if(max.y < coneVerts[i].y)
			max.y = coneVerts[i].y;
		if(max.z < coneVerts[i].z)
			max.z = coneVerts[i].z;
	}
	//free(coneVerts);
	retVal->reset(min, max);
	return retVal;
}
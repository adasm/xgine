#include "Tractor.h"

Tractor::Tractor(Vec3 pos, string fname)
{
	m_actualBraking = false;
	VehicleParams *params = new VehicleParams;
	VehicleParamsEx *vp = new VehicleParamsEx;
	vector<CfgVariable*> vars;
	vars = *params->getVariableList();
	m_backTriPod = NULL;
	m_frontTriPod = NULL;
	for(int i = 0; i < vp->getVariableList()->size(); i++)
	{
		vars.push_back((*vp->getVariableList())[i]);
	}
	Vec3 rotateFrontLeftWheel;
	Vec3 rotateFrontRightWheel;
	Vec3 rotateRearLeftWheel;
	Vec3 rotateRearRightWheel;

	Vec3 rotateChassis;

	string paramsType;
	string wheelFrontLeftModel;
	string wheelFrontRightModel;
	string wheelRearLeftModel;
	string wheelRearRightModel;
	string chassisModel;

	string backTriPod;
	string frontTriPod;

	string vehicleEngine;
	
	int hasParkingBrake;

	/*vars.push_back(new CfgVariable(V_VEC3, &rotateFrontLeftWheel, getVarName(rotateFrontLeftWheel)));
	vars.push_back(new CfgVariable(V_VEC3, &rotateFrontRightWheel, getVarName(rotateFrontRightWheel)));
	vars.push_back(new CfgVariable(V_VEC3, &rotateRearLeftWheel, getVarName(rotateRearLeftWheel)));
	vars.push_back(new CfgVariable(V_VEC3, &rotateRearRightWheel, getVarName(rotateRearRightWheel)));
	vars.push_back(new CfgVariable(V_VEC3, &rotateChassis, getVarName(rotateChassis)));*/

	vars.push_back(new CfgVariable(V_STRING, &paramsType, getVarName(paramsType)));

	vars.push_back(new CfgVariable(V_STRING, &wheelFrontLeftModel, getVarName(wheelFrontLeftModel)));
	vars.push_back(new CfgVariable(V_STRING, &wheelFrontRightModel, getVarName(wheelFrontRightModel)));
	vars.push_back(new CfgVariable(V_STRING, &wheelRearLeftModel, getVarName(wheelRearLeftModel)));
	vars.push_back(new CfgVariable(V_STRING, &wheelRearRightModel, getVarName(wheelRearRightModel)));
	vars.push_back(new CfgVariable(V_STRING, &chassisModel, getVarName(chassisModel)));

	vars.push_back(new CfgVariable(V_STRING, &backTriPod, getVarName(backTriPod)));
	vars.push_back(new CfgVariable(V_STRING, &frontTriPod, getVarName(frontTriPod)));

	vars.push_back(new CfgVariable(V_STRING, &vehicleEngine, getVarName(vehicleEngine)));
	vars.push_back(new CfgVariable(V_INT, &hasParkingBrake, getVarName(hasParkingBrake)));

	CfgLoader cfg(fname, &vars);
	m_vehicleEngine = new VehicleEngine(vehicleEngine);

	m_hasParkingBrake = hasParkingBrake;
	/*D3DXMatrixRotationYawPitchRoll(&dimm->matFrontLeftWheelRotation, rotateFrontLeftWheel.y, rotateFrontLeftWheel.x, rotateFrontLeftWheel.z);
	D3DXMatrixRotationYawPitchRoll(&dimm->matFrontRightWheelRotation, rotateFrontRightWheel.y, rotateFrontRightWheel.x, rotateFrontRightWheel.z);
	D3DXMatrixRotationYawPitchRoll(&dimm->matRearLeftWheelRotation, rotateRearLeftWheel.y, rotateRearLeftWheel.x, rotateRearLeftWheel.z);
	D3DXMatrixRotationYawPitchRoll(&dimm->matRearRightWheelRotation, rotateRearRightWheel.y, rotateRearRightWheel.x, rotateRearRightWheel.z);


	D3DXMatrixRotationYawPitchRoll(&dimm->matChassisRotation, rotateChassis.y, rotateChassis.x, rotateChassis.z);*/

	ObjectParams temp;
	temp.loadFromFile("Objects\\" + chassisModel);
	m_objChassis = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));

	temp.loadFromFile("Objects\\" + wheelFrontLeftModel);
	m_objFrontLeftWheel = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));

	temp.loadFromFile("Objects\\" + wheelFrontRightModel);
	m_objFrontRightWheel = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));

	temp.loadFromFile("Objects\\" + wheelRearLeftModel);
	m_objRearLeftWheel = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));

	temp.loadFromFile("Objects\\" + wheelRearRightModel);
	m_objRearRightWheel = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));

	core.game->getWorld()->addToWorld(m_objChassis, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objFrontLeftWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objFrontRightWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objRearLeftWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objRearRightWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);

	params->chassisDimm = Vec3(m_objChassis->boundingBox.getWidth()/2, m_objChassis->boundingBox.getHeight()/6, m_objChassis->boundingBox.getDepth()/2);

	//m_vehicleController = new VehicleController(core.dynamics, pos, dimm);
	if(paramsType == "Extended")
		m_vehicleController = new VehicleController(pos, m_vehicleEngine, params, vp, false);
	else
		m_vehicleController = new VehicleController(pos, m_vehicleEngine, params, NULL, false);
	m_vehicleController->getActor()->userData = (void*)new AdditionalData(DT_VEHICLE, (void*)this);
	m_vehicleController->setBrake();
	m_vehicleController->releaseMotor();
	if(backTriPod != "")
		m_backTriPod = new TriPod(this, backTriPod);
	if(frontTriPod != "")
		m_frontTriPod = new TriPod(this, frontTriPod, 1);
}

void Tractor::update()
{
	if(m_backTriPod)
		m_backTriPod->update();
	if(m_frontTriPod)
		m_frontTriPod->update();
	m_vehicleController->update();
	if(m_vehicleController->getActor()->isSleeping())
		return;
	//m_vehicleEngine->update();
	VehiclePose *vehiclePose = m_vehicleController->getVehiclePose();
	//gEngine.renderer->addTxt("Speed: %f", m_vehicleController->getSpeed());
	m_position = Vec3(m_vehicleController->getActor()->getGlobalPosition().x, m_vehicleController->getActor()->getGlobalPosition().y, m_vehicleController->getActor()->getGlobalPosition().z);
	if(m_objChassis)
	{
		m_objChassis->boundingBox.setTransform(Vec3(vehiclePose->matChassis._41, vehiclePose->matChassis._42, vehiclePose->matChassis._43));
		m_objChassis->world = vehiclePose->matChassis;
		m_objChassis->position = Vec3(vehiclePose->matChassis._41, vehiclePose->matChassis._42, vehiclePose->matChassis._43);
	}

	if(m_objFrontLeftWheel)
	{
		m_objFrontLeftWheel->boundingBox.setTransform(Vec3(vehiclePose->matFrontLeftWheel(3, 0), vehiclePose->matFrontLeftWheel(3, 1), vehiclePose->matFrontLeftWheel(3, 2)));
		m_objFrontLeftWheel->world = vehiclePose->matFrontLeftWheel;
		m_objFrontLeftWheel->position = Vec3(vehiclePose->matFrontLeftWheel._41, vehiclePose->matFrontLeftWheel._42, vehiclePose->matFrontLeftWheel._43);
	}

	if(m_objFrontRightWheel)
	{
		m_objFrontRightWheel->boundingBox.setTransform(Vec3(vehiclePose->matFrontRightWheel(3, 0), vehiclePose->matFrontRightWheel(3, 1), vehiclePose->matFrontRightWheel(3, 2)));
		m_objFrontRightWheel->world = vehiclePose->matFrontRightWheel;
		m_objFrontRightWheel->position = Vec3(vehiclePose->matFrontRightWheel._41, vehiclePose->matFrontRightWheel._42, vehiclePose->matFrontRightWheel._43);
	}

	if(m_objRearLeftWheel)
	{
		m_objRearLeftWheel->boundingBox.setTransform(Vec3(vehiclePose->matRearLeftWheel(3, 0), vehiclePose->matRearLeftWheel(3, 1), vehiclePose->matRearLeftWheel(3, 2)));
		m_objRearLeftWheel->world = vehiclePose->matRearLeftWheel;
		m_objRearLeftWheel->position = Vec3(vehiclePose->matRearLeftWheel._41, vehiclePose->matRearLeftWheel._42, vehiclePose->matRearLeftWheel._43);
	}

	if(m_objRearRightWheel)
	{
		m_objRearRightWheel->boundingBox.setTransform(Vec3(vehiclePose->matRearRightWheel(3, 0), vehiclePose->matRearRightWheel(3, 1), vehiclePose->matRearRightWheel(3, 2)));
		m_objRearRightWheel->world = vehiclePose->matRearRightWheel;
		m_objRearRightWheel->position = Vec3(vehiclePose->matRearRightWheel._41, vehiclePose->matRearRightWheel._42, vehiclePose->matRearRightWheel._43);
	}
}

void Tractor::updateInput()
{
	if(m_actualBraking)
	{
		if(!core.game->getKeyboardManager()->isForward() && !core.game->getKeyboardManager()->isBackward())
			m_actualBraking = false;
	}
	if(getBackTriPod())
	if(getBackTriPod()->isAttached() == AT_TRAILER)
		((ITrailer*)getBackTriPod()->getAttach())->updateInput();
	float angle = NxPi * core.dynamics->getDeltaTime() * 0.5f;
	if(m_vehicleController->getSpeed() > 10)
		angle = angle / (m_vehicleController->getSpeed() * 0.015f);
	if(angle < 0)
		angle = 0;
	if(core.game->getKeyboardManager()->isForward())
	{
		getVehicleController()->releaseParkingBrake();
		if(getVehicleController()->getSpeed() < -5)
		{
			//getVehicleController()->setAccelerate();
			getVehicleController()->setBrake();
			m_actualBraking = true;
		}
		else
			if(!m_actualBraking)
				getVehicleController()->setAccelerate();
	}
	else
	{
		if(core.game->getKeyboardManager()->isBackward())
		{
			getVehicleController()->releaseParkingBrake();
			if(getVehicleController()->getSpeed() > 5)
			{
				m_actualBraking = true;
				getVehicleController()->setBrake();
			}
			else
				if(!m_actualBraking)
					getVehicleController()->setBackward();
		}
		else
		{
			getVehicleController()->releaseMotor();
			getVehicleController()->releaseBrake();
			if(m_hasParkingBrake)
				getVehicleController()->setParkingBrake();
//			getVehicleController()->stopMotorAndBreak();
		}
	}
	if(core.game->getKeyboardManager()->isRight())
		getVehicleController()->setSteerAngle(-angle);
	else
	{
		if(core.game->getKeyboardManager()->isLeft())
			getVehicleController()->setSteerAngle(angle);
		else
		{
			if(getVehicleController()->getSteerAngle() < 0)
				getVehicleController()->setSteerAngle(angle);
			else
			{
				if(getVehicleController()->getSteerAngle() > 0)
					getVehicleController()->setSteerAngle(-angle);
			}
			if(getVehicleController()->getSteerAngle() < angle && getVehicleController()->getSteerAngle() > -angle)
				getVehicleController()->setSteerAngle(0);
		}
	}

	if(core.game->getKeyboardManager()->isLiftDevice())
	{
		if(getBackTriPod())
		{
			if(getBackTriPod()->isAttached() != AT_NONE)
			{
				if(getBackTriPod()->isLifted())
					getBackTriPod()->low();
				else
					getBackTriPod()->lift();
			}
		}
		if(getFrontTriPod())
		{
			if(getFrontTriPod()->isAttached() != AT_NONE)
			{
				if(getFrontTriPod()->isLifted())
					getFrontTriPod()->low();
				else
					getFrontTriPod()->lift();
			}
		}
	}

	if(core.game->getKeyboardManager()->isAttachToVehicle())
	{
		bool detached = false;
		if(getBackTriPod())
		{
			if(getBackTriPod()->isAttached() != AT_NONE)
			{
				if(getBackTriPod()->isAttached() == AT_DEVICE)
				{
					((IAgriDevice*)getBackTriPod()->getAttach())->detach();
				}
				if(getBackTriPod()->isAttached() == AT_TRAILER)
				{
					((ITrailer*)getBackTriPod()->getAttach())->detach();
				}
				getBackTriPod()->detach();
				detached = true;
			}
		}
		if(getFrontTriPod())
		{
			if(getFrontTriPod()->isAttached() != AT_NONE)
			{
				if(getFrontTriPod()->isAttached() == AT_DEVICE)
				{
					((IAgriDevice*)getFrontTriPod()->getAttach())->detach();
				}
				getFrontTriPod()->detach();
				detached = true;
			}
		}
		if(!detached)
		{
			bool backAttached = false;
			float minDistToFront = 4.0f;
			float minDistToBack = 4.0f;
			IAgriDevice* deviceFront = NULL;
			IAgriDevice* deviceBack = NULL;
			ITrailer* trailer = NULL;
			for(int i = 0; i < (*core.game->getWorld()->getAgriDevices()).size(); i++)
			{
				if(!(*core.game->getWorld()->getAgriDevices())[i]->isAttached() && (*core.game->getWorld()->getAgriDevices())[i]->getVehicleTypeDestination() == getVehicleType())
				{
					Vec3 diffFront = getVehicleController()->getFrontJointPoint() - (*core.game->getWorld()->getAgriDevices())[i]->getTriPodPos();
					Vec3 diffBack = getVehicleController()->getBackJointPoint() - (*core.game->getWorld()->getAgriDevices())[i]->getTriPodPos();
					if(minDistToFront > D3DXVec3Length(&diffFront))
					{
						minDistToFront = D3DXVec3Length(&diffFront);
						deviceFront = (*core.game->getWorld()->getAgriDevices())[i];
					}
					if(minDistToBack > D3DXVec3Length(&diffBack))
					{
						minDistToBack = D3DXVec3Length(&diffBack);
						deviceBack = (*core.game->getWorld()->getAgriDevices())[i];
					}
				}
			}
			if(deviceFront)
			{
				if(minDistToFront < minDistToBack)
				{
					getFrontTriPod()->attach(deviceFront);
					deviceFront->attach(getFrontTriPod());
				}
				//m_playerInfo->getDrivingVehicle()->joinTrailer(trailer);
			}
			if(deviceBack)
			{
				if(minDistToFront > minDistToBack)
				{
					backAttached = true;
					getBackTriPod()->attach(deviceBack);
					deviceBack->attach(getBackTriPod());
				}
				//m_playerInfo->getDrivingVehicle()->joinTrailer(trailer);
			}

			///if nothing is attached to back tri pod, try to attach trailer
			if(!backAttached)
			{
				minDistToBack = 4.0f;
				gEngine.renderer->addTxtF("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
				for(int i = 0; i < (*core.game->getWorld()->getTrailers()).size(); i++)
				{
					if(!(*core.game->getWorld()->getTrailers())[i]->isAttached())
					{
						Vec3 diffBack = getVehicleController()->getBackJointPoint() - (*core.game->getWorld()->getTrailers())[i]->getWheelSet()->getConnectorPos();
						if(minDistToBack > D3DXVec3Length(&diffBack))
						{
							minDistToBack = D3DXVec3Length(&diffBack);
							trailer = (*core.game->getWorld()->getTrailers())[i];
						}
					}
				}
				if(trailer)
				{
					getBackTriPod()->attach(trailer);
					trailer->attachToTractor(this);
				}
			}
		}
	}
}

VehicleController *Tractor::getVehicleController()
{
	return m_vehicleController;
}

TriPod *Tractor::getBackTriPod()
{
	return m_backTriPod;
}

TriPod *Tractor::getFrontTriPod()
{
	return m_frontTriPod;
}

Vec3 Tractor::getPos()
{
	return m_position;
}
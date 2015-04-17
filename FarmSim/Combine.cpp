#include "Combine.h"

Combine::Combine(Vec3 pos, string fname)
{
	m_actualBraking = false;
	m_actualPayloadType = NULL;
	CombineParams *params = new CombineParams;
	VehicleParamsEx *vp = new VehicleParamsEx;
	vector<CfgVariable*> vars;
	vars = *params->getVariableList();
	m_backTriPod = NULL;
	m_frontTriPod = NULL;
	m_actualPayload = 0;
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
	string pipeModel;

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
	vars.push_back(new CfgVariable(V_STRING, &pipeModel, getVarName(pipeModel)));

	vars.push_back(new CfgVariable(V_STRING, &backTriPod, getVarName(backTriPod)));
	vars.push_back(new CfgVariable(V_STRING, &frontTriPod, getVarName(frontTriPod)));

	vars.push_back(new CfgVariable(V_STRING, &vehicleEngine, getVarName(vehicleEngine)));
	vars.push_back(new CfgVariable(V_INT, &hasParkingBrake, getVarName(hasParkingBrake)));

	CfgLoader cfg(fname, &vars);
	m_capacity = params->capacity;
	m_dumpPoint = params->dumpPoint;
	m_pipeOut = false;
	m_pipeAngle = 0.0f;
	m_vecPipeRotation = params->vecPipeRotation;
	m_vecPipeTranslation = params->posPipe;


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

	temp.loadFromFile("Objects\\" + pipeModel);
	m_objPipe = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));

	core.game->getWorld()->addToWorld(m_objChassis, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objFrontLeftWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objFrontRightWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objRearLeftWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objRearRightWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objPipe, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);

	params->chassisDimm = Vec3(m_objChassis->boundingBox.getWidth()/2, m_objChassis->boundingBox.getHeight()/6, m_objChassis->boundingBox.getDepth()/2);

	//m_vehicleController = new VehicleController(core.dynamics, pos, dimm);
	if(paramsType == "Extended")
		m_vehicleController = new VehicleController(pos, m_vehicleEngine, (VehicleParams*)params, vp, true);
	else
		m_vehicleController = new VehicleController(pos, m_vehicleEngine, (VehicleParams*)params, NULL, true);
	m_vehicleController->getActor()->userData = (void*)new AdditionalData(DT_VEHICLE, this);
	m_vehicleController->setBrake();
	m_vehicleController->releaseMotor();
	if(backTriPod != "")
		m_backTriPod = new TriPod(this, backTriPod);
	if(frontTriPod != "")
		m_frontTriPod = new TriPod(this, frontTriPod, 1);
}

void Combine::update()
{
	if(m_backTriPod)
		m_backTriPod->update();
	if(m_frontTriPod)
		m_frontTriPod->update();
	m_vehicleController->update();
	//m_vehicleEngine->update();
	VehiclePose *vehiclePose = m_vehicleController->getVehiclePose();
	if(m_actualPayload <= 0)
	{
		m_actualPayload = 0;
		m_actualPayloadType = NULL;
	}

	if(m_pipeOut)
	{
		if(m_pipeAngle < 1.57f)
		{
			m_pipeAngle += gEngine.kernel->tmr->get() * 0.001f;
		}
		else
			m_pipeAngle = 1.57f;
	}
	else
	{
		if(m_pipeAngle > 0.0f)
		{
			m_pipeAngle -= gEngine.kernel->tmr->get() * 0.001f;
		}
		else
			m_pipeAngle = 0.0f;
	}
	
	unloadHarvestToTrailer();
	if(m_vehicleController->getActor()->isSleeping())
		return;
	m_position = Vec3(m_vehicleController->getActor()->getGlobalPosition().x, m_vehicleController->getActor()->getGlobalPosition().y, m_vehicleController->getActor()->getGlobalPosition().z);

	if(m_objChassis)
	{
		m_objChassis->boundingBox.setTransform(Vec3(vehiclePose->matChassis._41, vehiclePose->matChassis._42, vehiclePose->matChassis._43));
		m_objChassis->world = vehiclePose->matChassis;
		m_objChassis->position = Vec3(vehiclePose->matChassis._41, vehiclePose->matChassis._42, vehiclePose->matChassis._43);
	}

	if(m_objFrontLeftWheel)
	{
		//D3DXMatrixTranslation(&vehiclePose->matFrontLeftWheel, getDumpPoint().x, getDumpPoint().y, getDumpPoint().z);
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

	if(m_objPipe)
	{
		Mat pipeRotAxis;
		Mat pipeRot;
		Mat pipeTrans;
		D3DXMatrixRotationY(&pipeRotAxis, m_pipeAngle);
		D3DXMatrixRotationYawPitchRoll(&pipeRot, m_vecPipeRotation.y, m_vecPipeRotation.x, m_vecPipeRotation.z);
		D3DXMatrixTranslation(&pipeTrans, m_vecPipeTranslation.x, m_vecPipeTranslation.y, m_vecPipeTranslation.z);

		pipeRot = pipeRot * pipeRotAxis * pipeTrans * vehiclePose->matChassis;


		m_objPipe->boundingBox.setTransform(Vec3(pipeRot(3, 0), pipeRot(3, 1), pipeRot(3, 2)));
		m_objPipe->world = pipeRot;
		m_objPipe->position = Vec3(pipeRot._41, pipeRot._42, pipeRot._43);
	}
}

void Combine::updateInput()
{
	float angle = NxPi * core.dynamics->getDeltaTime() * 0.5f;
	if(core.game->getKeyboardManager()->isAction1())
	{
		m_pipeOut = !m_pipeOut;
	}

	if(core.game->getKeyboardManager()->isForward())
	{
		getVehicleController()->releaseParkingBrake();
		if(getVehicleController()->getSpeed() < -10)
			//getVehicleController()->setAccelerate();
			getVehicleController()->setBrake();
		else
			getVehicleController()->setAccelerate();
	}
	else
	{
		if(core.game->getKeyboardManager()->isBackward())
		{
			getVehicleController()->releaseParkingBrake();
			if(getVehicleController()->getSpeed() > 10)
				getVehicleController()->setBrake();
			else
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
			float minDistToFront = 4.0f;
			float minDistToBack = 4.0f;
			IAgriDevice* deviceFront = NULL;
			IAgriDevice* deviceBack = NULL;
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
					getBackTriPod()->attach(deviceBack);
					deviceBack->attach(getBackTriPod());
				}
				//m_playerInfo->getDrivingVehicle()->joinTrailer(trailer);
			}
			/*float minDist = 50.0f;
			IAgriDevice* device = NULL;
			for(int i = 0; i < (*core.game->getWorld()->getAgriDevices()).size(); i++)
			{
				if(!(*core.game->getWorld()->getAgriDevices())[i]->isAttached() && (*core.game->getWorld()->getAgriDevices())[i]->getVehicleTypeDestination() == getVehicleType())
				{
					Vec3 diff = getVehicleController()->getBackJointPoint() - (*core.game->getWorld()->getAgriDevices())[i]->getTriPodPos();
					if(minDist > D3DXVec3Length(&diff))
					{
						minDist = D3DXVec3Length(&diff);
						device = (*core.game->getWorld()->getAgriDevices())[i];
					}
				}
			}
			if(device)
			{
				getBackTriPod()->attach(device);
				device->attach(getBackTriPod());
				//m_playerInfo->getDrivingVehicle()->joinTrailer(trailer);
			}*/
		}
	}
}

VehicleController *Combine::getVehicleController()
{
	return m_vehicleController;
}

TriPod *Combine::getBackTriPod()
{
	return m_backTriPod;
}

TriPod *Combine::getFrontTriPod()
{
	return m_frontTriPod;
}

Vec3 Combine::getDumpPoint()
{
	Vec3 diffDumpPipePoint = m_dumpPoint - m_vecPipeTranslation;
	Vec3 retVal;
	D3DXVec3TransformCoord(&retVal, &diffDumpPipePoint, &m_objPipe->world);
	return retVal;
}

void Combine::unloadHarvestToTrailer()
{
	if(!m_actualPayload)
		return;
	Vec3 dumpPoint = getDumpPoint();
	int payloadToTransfer = gEngine.kernel->tmr->get() * 0.5f;
	if(payloadToTransfer > m_actualPayload)
		payloadToTransfer = m_actualPayload;
	for(int i = 0; i < core.game->getWorld()->getTrailers()->size(); i++)
	{
		if((*core.game->getWorld()->getTrailers())[i]->getDumperActionBox()->isPointInside(&dumpPoint))
		{
			if((*core.game->getWorld()->getTrailers())[i]->getPayloadValue() < (*core.game->getWorld()->getTrailers())[i]->getCapacity())
			{
				if((*core.game->getWorld()->getTrailers())[i]->getPayloadValue() + m_actualPayload < (*core.game->getWorld()->getTrailers())[i]->getCapacity())
				{
					if((*core.game->getWorld()->getTrailers())[i]->addPayload(payloadToTransfer, m_actualPayloadType))
					{
						m_actualPayload -= payloadToTransfer;
					}
				}
				else
				{
					payloadToTransfer = (*core.game->getWorld()->getTrailers())[i]->getCapacity() - (*core.game->getWorld()->getTrailers())[i]->getPayloadValue();
					(*core.game->getWorld()->getTrailers())[i]->addPayload(m_actualPayload, m_actualPayloadType);
					m_actualPayload -= payloadToTransfer;
				}
			}
		}
	}
}

void Combine::addPayload(int payloadToAdd, CropType* cropType)
{
	if(m_actualPayloadType == NULL || m_actualPayloadType == cropType)
	{
		m_actualPayload += payloadToAdd;
		m_actualPayloadType = cropType;
	}
}

int Combine::getCapacity()
{
	return m_capacity;
}

int Combine::getPayloadValue()
{
	return m_actualPayload;
}

CropType* Combine::getLoadedCropType()
{
	return m_actualPayloadType;
}

Vec3 Combine::getPos()
{
	return m_position;
}
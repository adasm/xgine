#include "Trailer.h"

Trailer::Trailer(Vec3 pos, string fname)
{
	m_dumperActualPayloadType = NULL;
	m_attachedTractor = NULL;
	m_dumperIsGoingUp = false;
	m_poseGiven = false;
	VehicleParamsEx* paramsEx = new VehicleParamsEx;
	WheelSetParams*	params = new WheelSetParams;
	m_params = new TrailerParams;
	m_pose = new TrailerPose;
	vector<CfgVariable*> vars = *m_params->getVariableList();
	for(int i = 0; i < params->getVariableList()->size(); i++)
	{
		vars.push_back((*params->getVariableList())[i]);
	}

	string wheelLeftFrontModel;
	string wheelRightFrontModel;

	string wheelLeftMiddleModel;
	string wheelRightMiddleModel;

	string wheelLeftRearModel;
	string wheelRightRearModel;

	string chassisModel;
	string dumperModel;
	string harvestModel;

	string paramsType;

	vars.push_back(new CfgVariable(V_STRING, &wheelLeftFrontModel, getVarName(wheelLeftFrontModel)));
	vars.push_back(new CfgVariable(V_STRING, &wheelRightFrontModel, getVarName(wheelRightFrontModel)));

	vars.push_back(new CfgVariable(V_STRING, &wheelLeftMiddleModel, getVarName(wheelLeftMiddleModel)));
	vars.push_back(new CfgVariable(V_STRING, &wheelRightMiddleModel, getVarName(wheelRightMiddleModel)));

	vars.push_back(new CfgVariable(V_STRING, &wheelLeftRearModel, getVarName(wheelLeftRearModel)));
	vars.push_back(new CfgVariable(V_STRING, &wheelRightRearModel, getVarName(wheelRightRearModel)));

	vars.push_back(new CfgVariable(V_STRING, &chassisModel, getVarName(chassisModel)));
	vars.push_back(new CfgVariable(V_STRING, &dumperModel, getVarName(dumperModel)));
	vars.push_back(new CfgVariable(V_STRING, &harvestModel, getVarName(harvestModel)));

	vars.push_back(new CfgVariable(V_STRING, &paramsType, getVarName(paramsType)));

	CfgLoader cfg(fname, &vars);
	CfgLoader cfg1(fname, paramsEx->getVariableList());

	m_dumperActualPayload = 0;
	m_dumperActionBox = new ActionBox(m_params->dumperActionBoxMin, m_params->dumperActionBoxMax);

	m_objChassis = NULL;
	m_objDumper = NULL;
	m_objHarvest = NULL;

	m_objWheelLeftFront = NULL;
	m_objWheelRightFront = NULL;

	m_objWheelLeftMiddle = NULL;
	m_objWheelRightMiddle = NULL;

	m_objWheelLeftRear = NULL;
	m_objWheelRightRear = NULL;


	ObjectParams temp;
	temp.loadFromFile("Objects\\" + chassisModel);
	m_objChassis = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));
	temp.loadFromFile("Objects\\" + dumperModel);
	m_objDumper = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));
	temp.loadFromFile("Objects\\" + harvestModel);
	m_objHarvest = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));

	temp.loadFromFile("Objects\\" + wheelLeftFrontModel);
	m_objWheelLeftFront = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));
	temp.loadFromFile("Objects\\" + wheelRightFrontModel);
	m_objWheelRightFront = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));

	temp.loadFromFile("Objects\\" + wheelLeftMiddleModel);
	m_objWheelLeftMiddle = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));
	temp.loadFromFile("Objects\\" + wheelRightMiddleModel);
	m_objWheelRightMiddle = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));

	temp.loadFromFile("Objects\\" + wheelLeftRearModel);
	m_objWheelLeftRear = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));
	temp.loadFromFile("Objects\\" + wheelRightRearModel);
	m_objWheelRightRear = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));

	core.game->getWorld()->addToWorld(m_objChassis, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objDumper, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objHarvest, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);

	core.game->getWorld()->addToWorld(m_objWheelLeftFront, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objWheelRightFront, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);

	core.game->getWorld()->addToWorld(m_objWheelLeftMiddle, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objWheelRightMiddle, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);

	core.game->getWorld()->addToWorld(m_objWheelLeftRear, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objWheelRightRear, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	m_dumperActualAngle = 0.6f;
	params->chassisDimm = Vec3(m_objChassis->boundingBox.getWidth()/2, m_objChassis->boundingBox.getHeight()/6, m_objChassis->boundingBox.getDepth()/2);

	if(paramsType == "Extended")
		m_wheelSet = new WheelSet(pos, params, paramsEx);
	else
		m_wheelSet = new WheelSet(pos, params, NULL);
	m_wheelSet->getActor()->userData = (void*)new AdditionalData(DT_TRAILER, (void*)this);
}

void Trailer::update()
{
	if(m_dumperActualPayload > m_params->capacity)
		m_dumperActualPayload = m_params->capacity;
	if(m_dumperActualPayload <= 0)
	{
		m_dumperActualPayload = 0;
		m_dumperActualPayloadType = NULL;
	}
	m_poseGiven = false;
	m_wheelSet->update();
	//m_dumperActualPayload = 0;

	if(m_dumperIsGoingUp)
	{
		if(m_dumperActualAngle < 0.9f)
		{
			m_dumperActualAngle += gEngine.kernel->tmr->get() * 0.0001f;
		}
		else
			m_dumperActualAngle = 0.9f;
	}
	else
	{
		if(m_dumperActualAngle > 0.0f)
		{
			m_dumperActualAngle -= gEngine.kernel->tmr->get() * 0.0001f;
		}
		else
			m_dumperActualAngle = 0.0f;
	}

	if(m_objHarvest)
	{
		m_objHarvest->boundingBox.setTransform(Vec3(getPose()->matHarvest._41, getPose()->matHarvest._42, getPose()->matHarvest._43));
		m_objHarvest->world = getPose()->matHarvest;
		m_objHarvest->position = Vec3(getPose()->matHarvest._41, getPose()->matHarvest._42, getPose()->matHarvest._43);
	}
	if(m_objDumper)
	{
		m_objDumper->boundingBox.setTransform(Vec3(getPose()->matDumper._41, getPose()->matDumper._42, getPose()->matDumper._43));
		m_objDumper->world = getPose()->matDumper;
		m_objDumper->position = Vec3(getPose()->matDumper._41, getPose()->matDumper._42, getPose()->matDumper._43);
	}
	if(m_objChassis)
	{
		m_objChassis->boundingBox.setTransform(Vec3(m_wheelSet->getActor()->getGlobalPosition().x, m_wheelSet->getActor()->getGlobalPosition().y, m_wheelSet->getActor()->getGlobalPosition().z));
		m_objChassis->world = getPose()->matChassis;
		m_objChassis->position = Vec3(getPose()->matChassis._41, getPose()->matChassis._42, getPose()->matChassis._43);
	}

	if(m_objWheelLeftFront)
	{
		m_objWheelLeftFront->boundingBox.setTransform(Vec3(getPose()->matWheelLeftFront(3, 0), getPose()->matWheelLeftFront(3, 1), getPose()->matWheelLeftFront(3, 2)));
		m_objWheelLeftFront->world = getPose()->matWheelLeftFront;
		m_objWheelLeftFront->position = Vec3(getPose()->matWheelLeftFront._41, getPose()->matWheelLeftFront._42, getPose()->matWheelLeftFront._43);
	}

	if(m_objWheelRightFront)
	{
		m_objWheelRightFront->boundingBox.setTransform(Vec3(getPose()->matWheelRightFront(3, 0), getPose()->matWheelRightFront(3, 1), getPose()->matWheelRightFront(3, 2)));
		m_objWheelRightFront->world = getPose()->matWheelRightFront;
		m_objWheelRightFront->position = Vec3(getPose()->matWheelRightFront._41, getPose()->matWheelRightFront._42, getPose()->matWheelRightFront._43);
	}

	if(m_objWheelLeftMiddle)
	{
		m_objWheelLeftMiddle->boundingBox.setTransform(Vec3(getPose()->matWheelLeftMiddle(3, 0), getPose()->matWheelLeftMiddle(3, 1), getPose()->matWheelLeftMiddle(3, 2)));
		m_objWheelLeftMiddle->world = getPose()->matWheelLeftMiddle;
		m_objWheelLeftMiddle->position = Vec3(getPose()->matWheelLeftMiddle._41, getPose()->matWheelLeftMiddle._42, getPose()->matWheelLeftMiddle._43);
	}

	if(m_objWheelRightMiddle)
	{
		m_objWheelRightMiddle->boundingBox.setTransform(Vec3(getPose()->matWheelRightMiddle(3, 0), getPose()->matWheelRightMiddle(3, 1), getPose()->matWheelRightMiddle(3, 2)));
		m_objWheelRightMiddle->world = getPose()->matWheelRightMiddle;
		m_objWheelRightMiddle->position = Vec3(getPose()->matWheelRightMiddle._41, getPose()->matWheelRightMiddle._42, getPose()->matWheelRightMiddle._43);
	}

	if(m_objWheelLeftRear)
	{
		m_objWheelLeftRear->boundingBox.setTransform(Vec3(getPose()->matWheelLeftRear(3, 0), getPose()->matWheelLeftRear(3, 1), getPose()->matWheelLeftRear(3, 2)));
		m_objWheelLeftRear->world = getPose()->matWheelLeftRear;
		m_objWheelLeftRear->position = Vec3(getPose()->matWheelLeftRear._41, getPose()->matWheelLeftRear._42, getPose()->matWheelLeftRear._43);
	}

	if(m_objWheelRightRear)
	{
		m_objWheelRightRear->boundingBox.setTransform(Vec3(getPose()->matWheelRightRear(3, 0), getPose()->matWheelRightRear(3, 1), getPose()->matWheelRightRear(3, 2)));
		m_objWheelRightRear->world = getPose()->matWheelRightRear;
		m_objWheelRightRear->position = Vec3(getPose()->matWheelRightRear._41, getPose()->matWheelRightRear._42, getPose()->matWheelRightRear._43);
	}
}

TrailerPose* Trailer::getPose()
{
	if(m_poseGiven)
		return m_pose;
	m_poseGiven = true;
	m_pose->matChassis = m_wheelSet->getPose()->matChassis;

	m_pose->matWheelLeftFront = m_wheelSet->getPose()->matWheelLeftFront;
	m_pose->matWheelRightFront = m_wheelSet->getPose()->matWheelRightFront;

	m_pose->matWheelLeftMiddle = m_wheelSet->getPose()->matWheelLeftMiddle;
	m_pose->matWheelRightMiddle = m_wheelSet->getPose()->matWheelRightMiddle;

	m_pose->matWheelLeftRear = m_wheelSet->getPose()->matWheelLeftRear;
	m_pose->matWheelRightRear = m_wheelSet->getPose()->matWheelRightRear;

	/*DUMPER*/
	Mat dumperTrans;
	Mat dumperTempTrans;
	Mat dumperTempTrans2;
	Mat dumperRot;
	D3DXMatrixTranslation(&dumperTrans, m_params->posDumper.x, m_params->posDumper.y, m_params->posDumper.z);
	D3DXMatrixTranslation(&dumperTempTrans, -m_params->posDumperHinge.x, -m_params->posDumperHinge.y, -m_params->posDumperHinge.z);
	D3DXMatrixTranslation(&dumperTempTrans2, m_params->posDumperHinge.x, m_params->posDumperHinge.y, m_params->posDumperHinge.z);
	D3DXMatrixRotationAxis(&dumperRot, &Vec3(0, 0, 1), m_dumperActualAngle);
	//worldMat(&m_vehiclePose->matDumper, Vec3(m_dumperActor->getGlobalPosition().x, m_dumperActor->getGlobalPosition().y, m_dumperActor->getGlobalPosition().z), Vec3(0, 0, 0));
	//D3DXMatrixRotationQuaternion(&rot, &D3DXQUATERNION(m_dumperActor->getGlobalOrientationQuat().x, m_dumperActor->getGlobalOrientationQuat().y, m_dumperActor->getGlobalOrientationQuat().z, m_actor->getGlobalOrientationQuat().w));
	m_pose->matDumper = dumperTrans * dumperTempTrans * dumperRot * dumperTempTrans2 * m_pose->matChassis;

	/*HARVEST*/
	if(m_dumperActualPayload)
	{
		Mat harvestTrans;
		D3DXMatrixTranslation(&harvestTrans, m_params->posHarvest.x, m_params->posHarvest.y + (float)((float)m_dumperActualPayload / (float)m_params->capacity)*m_params->maxHarvestHeight, m_params->posHarvest.z);
		m_pose->matHarvest = harvestTrans * m_pose->matDumper;
	}
	else
		D3DXMatrixIdentity(&m_pose->matHarvest);

	return m_pose;
}

Tractor* Trailer::isAttached()
{
	return m_attachedTractor;
}

void Trailer::attachToTractor(Tractor *tractor)
{
	m_attachedTractor = tractor;
}

void Trailer::detach()
{
	m_attachedTractor = NULL;
}

void Trailer::updateInput()
{
	if(core.game->getKeyboardManager()->isAction1())
	{

		m_dumperIsGoingUp = !m_dumperIsGoingUp;
	}
}

bool Trailer::addPayload(int payloadToAdd, CropType* cropType)
{
	if(m_dumperActualPayloadType == NULL || m_dumperActualPayloadType == cropType)
	{
		m_dumperActualPayload += payloadToAdd;
		m_dumperActualPayloadType = cropType;
		return true;
	}
	return false;
}

int Trailer::getCapacity()
{
	return m_params->capacity;
}

int Trailer::getPayloadValue()
{
	return m_dumperActualPayload;
}

CropType* Trailer::getLoadedCropType()
{
	return m_dumperActualPayloadType;
}

ActionBox* Trailer::getDumperActionBox()
{
	m_dumperActionBox->setTransform(&getPose()->matDumper);
	return m_dumperActionBox;
}

Vec3 Trailer::getPayloadDumpPos()
{
	Vec3 retVal;
	D3DXVec3TransformCoord(&retVal, &m_params->posPayloadDump, &m_objChassis->world);
	return retVal;
}

//
//Trailer::Trailer(Dynamics *dyn, NxVec3 pos, string fname)
//{
//	m_scene = dyn->getScene();
//	VehicleParamsEx *paramsEx = new VehicleParamsEx;
//	vector<CfgVariable*> vars;
//	m_actor = NULL;
//	m_vehiclePose = new TrailerPose;
//	m_joined = false;
//
//	m_dumperRotateAngle = 0;
//	int dumperCapacity;
//	float rearWheelsRadius;
//	float rearWheelsWidth;
//	float frontWheelsRadius;
//	float frontWheelsWidth;
//	float trailerDensity;
//	float harvestMaxHeight;
//	Vec3 jointPos;
//	Vec3 rotateWheels;
//	Vec3 harvestPos;
//	m_dumperPayload = 1000;
//
//	Vec3 posFrontLeftWheel;
//	Vec3 posFrontRightWheel;
//	Vec3 posRearLeftWheel;
//	Vec3 posRearRightWheel;
//	Vec3 dumperRotateAxis;
//	Vec3 dumperRotatePoint;
//	Vec3 dumperPos;
//
//	Vec3 trailerDimm;
//
//	string paramsType;
//	string modelChassis;
//	string modelDumper;
//	string modelHarvest;
//
//	string modelFrontLeftWheel;
//	string modelFrontRightWheel;
//	string modelRearLeftWheel;
//	string modelRearRightWheel;
//	string trailerWheelNb;
//
//	vars.push_back(new CfgVariable(V_STRING, &paramsType, getVarName(paramsType)));
//	vars.push_back(new CfgVariable(V_INT, &dumperCapacity, getVarName(dumperCapacity)));
//	vars.push_back(new CfgVariable(V_FLOAT, &harvestMaxHeight, getVarName(harvestMaxHeight)));
//	vars.push_back(new CfgVariable(V_VEC3, &harvestPos, getVarName(harvestPos)));
//	vars.push_back(new CfgVariable(V_FLOAT, &trailerDensity, getVarName(trailerDensity)));
//	vars.push_back(new CfgVariable(V_VEC3, &jointPos, getVarName(jointPos)));
//	vars.push_back(new CfgVariable(V_VEC3, &trailerDimm, getVarName(trailerDimm)));
//	vars.push_back(new CfgVariable(V_VEC3, &rotateWheels, getVarName(rotateWheels)));
//	vars.push_back(new CfgVariable(V_VEC3, &dumperPos, getVarName(dumperPos)));
//
//	vars.push_back(new CfgVariable(V_VEC3, &dumperRotateAxis, getVarName(dumperRotateAxis)));
//	vars.push_back(new CfgVariable(V_VEC3, &dumperRotatePoint, getVarName(dumperRotatePoint)));
//
//	vars.push_back(new CfgVariable(V_FLOAT, &frontWheelsRadius, getVarName(frontWheelsRadius)));
//	vars.push_back(new CfgVariable(V_FLOAT, &frontWheelsWidth, getVarName(frontWheelsWidth)));
//	vars.push_back(new CfgVariable(V_FLOAT, &rearWheelsRadius, getVarName(rearWheelsRadius)));
//	vars.push_back(new CfgVariable(V_FLOAT, &rearWheelsWidth, getVarName(rearWheelsWidth)));
//
//	vars.push_back(new CfgVariable(V_VEC3, &posFrontLeftWheel, getVarName(posFrontLeftWheel)));
//	vars.push_back(new CfgVariable(V_VEC3, &posFrontRightWheel, getVarName(posFrontRightWheel)));
//	vars.push_back(new CfgVariable(V_VEC3, &posRearLeftWheel, getVarName(posRearLeftWheel)));
//	vars.push_back(new CfgVariable(V_VEC3, &posRearRightWheel, getVarName(posRearRightWheel)));
//
//	vars.push_back(new CfgVariable(V_STRING, &modelFrontLeftWheel, getVarName(modelFrontLeftWheel)));
//	vars.push_back(new CfgVariable(V_STRING, &modelFrontRightWheel, getVarName(modelFrontRightWheel)));
//	vars.push_back(new CfgVariable(V_STRING, &modelRearLeftWheel, getVarName(modelRearLeftWheel)));
//	vars.push_back(new CfgVariable(V_STRING, &modelRearRightWheel, getVarName(modelRearRightWheel)));
//
//	vars.push_back(new CfgVariable(V_STRING, &modelChassis, getVarName(modelChassis)));
//	vars.push_back(new CfgVariable(V_STRING, &modelDumper, getVarName(modelDumper)));
//	vars.push_back(new CfgVariable(V_STRING, &modelHarvest, getVarName(modelHarvest)));
//
//	vars.push_back(new CfgVariable(V_STRING, &trailerWheelNb, getVarName(trailerWheelNb)));
//	for(int i = 0; i < paramsEx->getVariableList()->size(); i++)
//	{
//		vars.push_back((*paramsEx->getVariableList())[i]);
//	}
//	CfgLoader cfg(fname, &vars);
//	m_dumperCapacity = dumperCapacity;
//	m_harvestMaxHeight = harvestMaxHeight;
//	m_harvestPos = harvestPos;
//
//
//	m_dumperRotateAxis = dumperRotateAxis;
//	m_dumperRotatePoint = dumperRotatePoint;
//	m_dumperPos = dumperPos;
//	m_jointPoint = jointPos;
//	D3DXMatrixRotationYawPitchRoll(&m_matRotateWheels, rotateWheels.y, rotateWheels.x, rotateWheels.z);
//
//	m_trailerWheelNb = TWN_4WHEEL;
//	if(trailerWheelNb == "NoWheel")
//		m_trailerWheelNb = TWN_NO_WHEEL;
//	if(trailerWheelNb == "2Wheel")
//		m_trailerWheelNb = TWN_2WHEEL;
//
//	m_objChassis = NULL;
//	m_objDumper = NULL;
//	m_objFrontLeftWheel = NULL;
//	m_objFrontRightWheel = NULL;
//	m_objRearLeftWheel = NULL;
//	m_objRearRightWheel = NULL;
//	m_objHarvest = NULL;
//
//	switch(m_trailerWheelNb)
//	{
//		case TWN_2WHEEL:
//			m_objFrontLeftWheel = new Surface(modelFrontLeftWheel, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
//			m_objFrontRightWheel = new Surface(modelFrontRightWheel, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
//			core.game->getWorld()->addToWorld(m_objFrontLeftWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
//			core.game->getWorld()->addToWorld(m_objFrontRightWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
//			break;
//		case TWN_4WHEEL:
//			m_objFrontLeftWheel = new Surface(modelFrontLeftWheel, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
//			m_objFrontRightWheel = new Surface(modelFrontRightWheel, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
//			m_objRearLeftWheel = new Surface(modelRearLeftWheel, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
//			m_objRearRightWheel = new Surface(modelRearRightWheel, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
//			core.game->getWorld()->addToWorld(m_objFrontLeftWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
//			core.game->getWorld()->addToWorld(m_objFrontRightWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
//			core.game->getWorld()->addToWorld(m_objRearLeftWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
//			core.game->getWorld()->addToWorld(m_objRearRightWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
//			break;
//	}
//
//	m_actor = core.dynamics->createBox(pos, NxVec3(trailerDimm), trailerDensity);
//	m_objChassis = new Surface(modelChassis, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
//	m_objDumper = new Surface(modelDumper, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
//	m_objHarvest = new Surface(modelHarvest, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
//	core.game->getWorld()->addToWorld(m_objChassis, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
//	core.game->getWorld()->addToWorld(m_objDumper, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
//	core.game->getWorld()->addToWorld(m_objHarvest, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
//
//	// Wheel material globals
//	NxMaterial* wsm = NULL;
//
//	NxTireFunctionDesc lngTFD;
//	lngTFD.extremumSlip = 1.0f;
//	lngTFD.extremumValue = 0.02f;
//	lngTFD.asymptoteSlip = 2.0f;
//	lngTFD.asymptoteValue = 0.01f;	
//	lngTFD.stiffnessFactor = 1000000.0f;
//
//	NxTireFunctionDesc latTFD;
//	latTFD.extremumSlip = 1.0f;
//	latTFD.extremumValue = 0.02f;
//	latTFD.asymptoteSlip = 2.0f;
//	latTFD.asymptoteValue = 0.01f;	
//	latTFD.stiffnessFactor = 1000000.0f;
//
//	NxTireFunctionDesc slipTFD;
//	slipTFD.extremumSlip = 1.0f;
//	slipTFD.extremumValue = 0.02f;
//	slipTFD.asymptoteSlip = 2.0f;
//	slipTFD.asymptoteValue = 0.01f;	
//	slipTFD.stiffnessFactor = 100.0f;  
//
//	NxTireFunctionDesc medTFD;
//	medTFD.extremumSlip = 1.0f;
//	medTFD.extremumValue = 0.02f;
//	medTFD.asymptoteSlip = 2.0f;
//	medTFD.asymptoteValue = 0.01f;	
//	medTFD.stiffnessFactor = 10000.0f;
//
//	// Front left wheel
//	NxWheelDesc wheelDesc;
//	wheelDesc.frictionToFront = 100.0f;
//	wheelDesc.frictionToSide = 1.0f;
//
//
//	wheelDesc.wheelApproximation = 1;
//	wheelDesc.wheelRadius = frontWheelsRadius;
//	wheelDesc.wheelWidth = frontWheelsWidth;  // 0.1
//	wheelDesc.wheelSuspension = 10;  
//	wheelDesc.springRestitution = 0.1f;
//	wheelDesc.springDamping = 10;
//	wheelDesc.springBias = 10;  
//	wheelDesc.maxBrakeForce = 0.01f;
//	wheelDesc.position = NxVec3(posFrontLeftWheel);
//	wheelDesc.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | 
//						   NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
//
//	// Front right wheel
//	NxWheelDesc wheelDesc2;
//	wheelDesc2.frictionToFront = 100.0f;
//	wheelDesc2.frictionToSide = 1.0f;
//
//
//
//	wheelDesc2.wheelApproximation = 1;
//	wheelDesc2.wheelRadius = frontWheelsRadius;
//	wheelDesc2.wheelWidth = frontWheelsWidth;  // 0.1
//	wheelDesc2.wheelSuspension = 10;  
//	wheelDesc2.springRestitution = 0.1f;
//	wheelDesc2.springDamping = 10;
//	wheelDesc2.springBias = 10;  
//	wheelDesc.maxBrakeForce = 0.01f;
//	wheelDesc2.position = NxVec3(posFrontRightWheel);
//	wheelDesc2.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | 
//							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
//
//	// Rear left wheel
//	NxWheelDesc wheelDesc3;
//	wheelDesc3.frictionToFront = 100.0f;
//	wheelDesc3.frictionToSide = 1.0f;
//
//
//
//	wheelDesc3.wheelApproximation = 1;
//	wheelDesc3.wheelRadius = rearWheelsRadius;
//	wheelDesc3.wheelWidth = rearWheelsWidth;  // 0.1
//	wheelDesc3.wheelSuspension = 10;  
//	wheelDesc3.springRestitution = 0.1f;
//	wheelDesc3.springDamping = 10;
//	wheelDesc3.springBias = 10;  
//	wheelDesc3.maxBrakeForce = 0.01f;
//	wheelDesc3.position = NxVec3(posRearLeftWheel);
//	wheelDesc3.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | 
//							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
//
//	// Rear right wheel
//	NxWheelDesc wheelDesc4;
//	wheelDesc4.frictionToFront = 100.0f;
//	wheelDesc4.frictionToSide = 1.0f;
//
//
//	wheelDesc4.wheelApproximation = 1;
//	wheelDesc4.wheelRadius = rearWheelsRadius;
//	wheelDesc4.wheelWidth = rearWheelsWidth;  // 0.1
//	wheelDesc4.wheelSuspension = 10;  
//	wheelDesc4.springRestitution = 0.1f;
//	wheelDesc4.springDamping = 10;
//	wheelDesc4.springBias = 10;  
//	wheelDesc4.maxBrakeForce = 0.01f;
//	wheelDesc4.position = NxVec3(posRearRightWheel);
//	wheelDesc4.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
//
//
//	if(paramsType == "Extended")
//	{
//		wheelDesc4.frictionToFront = wheelDesc3.frictionToFront = wheelDesc2.frictionToFront = wheelDesc.frictionToFront = paramsEx->frictionToFront;
//		wheelDesc4.frictionToSide = wheelDesc3.frictionToSide = wheelDesc2.frictionToSide = wheelDesc.frictionToSide = paramsEx->frictionToSide;
//
//		wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = paramsEx->wheelSuspension;
//		wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = paramsEx->springRestitution;
//		wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = paramsEx->springDamping;
//		wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = paramsEx->springBias;
//		wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = paramsEx->maxBrakeForce; // 100;
//	}
//	else
//	{
//		wheelDesc4.frictionToFront = wheelDesc3.frictionToFront = wheelDesc2.frictionToFront = wheelDesc.frictionToFront = 10.0f;
//		wheelDesc4.frictionToSide = wheelDesc3.frictionToSide = wheelDesc2.frictionToSide = wheelDesc.frictionToSide = 10.0f;
//
//		wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.5f / 10;
//		wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 1000.0f / 100000000000;
//		wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.5f / 10000000000000;
//		wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 1.0f / 10000000000000;
//		wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01f; // 100;
//	}
//
//	if(m_actor)
//	if(m_trailerWheelNb != TWN_NO_WHEEL)
//	{
//		m_wheelFrontLeft = AddWheelToActor(m_actor, &wheelDesc);
//		m_wheelFrontRight = AddWheelToActor(m_actor, &wheelDesc2);
//
//		m_wheelFrontLeft->userData = (void*)new ShapeUserData;
//		m_wheelFrontRight->userData = (void*)new ShapeUserData;
//
//		m_wheelFrontLeft->setBrakeTorque(0);
//		m_wheelFrontRight->setBrakeTorque(0);
//
//		if(m_trailerWheelNb == TWN_4WHEEL)
//		{
//			m_wheelRearLeft = AddWheelToActor(m_actor, &wheelDesc3);
//			m_wheelRearRight = AddWheelToActor(m_actor, &wheelDesc4);
//			m_wheelRearLeft->userData = (void*)new ShapeUserData;
//			m_wheelRearRight->userData = (void*)new ShapeUserData;
//
//			m_wheelRearLeft->setBrakeTorque(0);
//			m_wheelRearRight->setBrakeTorque(0);
//		}
//	}
//
//	// LOWER THE CENTER OF MASS
//	NxVec3 massPos;
//	if(m_actor)
//	{
//		if(paramsEx->massCenter != Vec3(0, 0, 0))
//			massPos = NxVec3(paramsEx->massCenter);
//		else
//		{
//			massPos = m_actor->getCMassLocalPosition();
//			massPos.y -= 1;
//		}
//		m_actor->setCMassOffsetLocalPosition(massPos);
//		SetActorCollisionGroup(m_actor, GROUP_COLLIDABLE_NON_PUSHABLE);
//	}
//
//}
//
//Trailer::~Trailer()
//{
//}
//
//NxActor* Trailer::getActor()
//{
//	return m_actor;
//}
//
//void Trailer::update()
//{
//	m_poseGiven = false;
//
//	if(m_objHarvest)
//	{
//		m_objHarvest->boundingBox.setTransform(Vec3(m_actor->getGlobalPosition().x + m_dumperPos.x, m_actor->getGlobalPosition().y + m_dumperPos.y, m_actor->getGlobalPosition().z + m_dumperPos.z));
//		m_objHarvest->world = ((TrailerPose*)getVehiclePose())->matHarvest;
//		m_objHarvest->position = Vec3(((TrailerPose*)getVehiclePose())->matHarvest._41, ((TrailerPose*)getVehiclePose())->matHarvest._42, ((TrailerPose*)getVehiclePose())->matHarvest._43);
//	}
//	if(m_objDumper)
//	{
//		m_objDumper->boundingBox.setTransform(Vec3(m_actor->getGlobalPosition().x + m_dumperPos.x, m_actor->getGlobalPosition().y + m_dumperPos.y, m_actor->getGlobalPosition().z + m_dumperPos.z));
//		m_objDumper->world = ((TrailerPose*)getVehiclePose())->matDumper;
//		m_objDumper->position = Vec3(((TrailerPose*)getVehiclePose())->matDumper._41, ((TrailerPose*)getVehiclePose())->matDumper._42, ((TrailerPose*)getVehiclePose())->matDumper._43);
//	}
//	if(m_objChassis)
//	{
//		m_objChassis->boundingBox.setTransform(Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z));
//		m_objChassis->world = ((TrailerPose*)getVehiclePose())->matChassis;
//		m_objChassis->position = Vec3(((TrailerPose*)getVehiclePose())->matChassis._41, ((TrailerPose*)getVehiclePose())->matChassis._42, ((TrailerPose*)getVehiclePose())->matChassis._43);
//	}
//
//	if(m_objFrontLeftWheel)
//	{
//		m_objFrontLeftWheel->boundingBox.setTransform(Vec3(((TrailerPose*)getVehiclePose())->matFrontLeftWheel(3, 0), ((TrailerPose*)getVehiclePose())->matFrontLeftWheel(3, 1), ((TrailerPose*)getVehiclePose())->matFrontLeftWheel(3, 2)));
//		m_objFrontLeftWheel->world = ((TrailerPose*)getVehiclePose())->matFrontLeftWheel;
//		m_objFrontLeftWheel->position = Vec3(((TrailerPose*)getVehiclePose())->matFrontLeftWheel._41, ((TrailerPose*)getVehiclePose())->matFrontLeftWheel._42, ((TrailerPose*)getVehiclePose())->matFrontLeftWheel._43);
//	}
//
//	if(m_objFrontRightWheel)
//	{
//		m_objFrontRightWheel->boundingBox.setTransform(Vec3(((TrailerPose*)getVehiclePose())->matFrontRightWheel(3, 0), ((TrailerPose*)getVehiclePose())->matFrontRightWheel(3, 1), ((TrailerPose*)getVehiclePose())->matFrontRightWheel(3, 2)));
//		m_objFrontRightWheel->world = ((TrailerPose*)getVehiclePose())->matFrontRightWheel;
//		m_objFrontRightWheel->position = Vec3(((TrailerPose*)getVehiclePose())->matFrontRightWheel._41, ((TrailerPose*)getVehiclePose())->matFrontRightWheel._42, ((TrailerPose*)getVehiclePose())->matFrontRightWheel._43);
//	}
//
//	if(m_objRearLeftWheel)
//	{
//		m_objRearLeftWheel->boundingBox.setTransform(Vec3(((TrailerPose*)getVehiclePose())->matRearLeftWheel(3, 0), ((TrailerPose*)getVehiclePose())->matRearLeftWheel(3, 1), ((TrailerPose*)getVehiclePose())->matRearLeftWheel(3, 2)));
//		m_objRearLeftWheel->world = ((TrailerPose*)getVehiclePose())->matRearLeftWheel;
//		m_objRearLeftWheel->position = Vec3(((TrailerPose*)getVehiclePose())->matRearLeftWheel._41, ((TrailerPose*)getVehiclePose())->matRearLeftWheel._42, ((TrailerPose*)getVehiclePose())->matRearLeftWheel._43);
//	}
//
//	if(m_objRearRightWheel)
//	{
//		m_objRearRightWheel->boundingBox.setTransform(Vec3(((TrailerPose*)getVehiclePose())->matRearRightWheel(3, 0), ((TrailerPose*)getVehiclePose())->matRearRightWheel(3, 1), ((TrailerPose*)getVehiclePose())->matRearRightWheel(3, 2)));
//		m_objRearRightWheel->world = ((TrailerPose*)getVehiclePose())->matRearRightWheel;
//		m_objRearRightWheel->position = Vec3(((TrailerPose*)getVehiclePose())->matRearRightWheel._41, ((TrailerPose*)getVehiclePose())->matRearRightWheel._42, ((TrailerPose*)getVehiclePose())->matRearRightWheel._43);
//	}
//}
//
//void* Trailer::getVehiclePose()
//{
//	if(m_poseGiven)
//		return (void*)m_vehiclePose;
//	m_poseGiven = true;
//	/*CHASSIS*/
//	Mat rot;
//	worldMat(&m_vehiclePose->matChassis, Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z), Vec3(0, 0, 0));
//	D3DXMatrixRotationQuaternion(&rot, &D3DXQUATERNION(m_actor->getGlobalOrientationQuat().x, m_actor->getGlobalOrientationQuat().y, m_actor->getGlobalOrientationQuat().z, m_actor->getGlobalOrientationQuat().w));
//	m_vehiclePose->matChassis = rot * m_vehiclePose->matChassis;
//
//	/*DUMPER*/
//	Mat dumperTrans;
//	Mat dumperTempTrans;
//	Mat dumperTempTrans2;
//	Mat dumperRot;
//	D3DXMatrixTranslation(&dumperTrans, m_dumperPos.x, m_dumperPos.y, m_dumperPos.z);
//	D3DXMatrixTranslation(&dumperTempTrans, -m_dumperRotatePoint.x, -m_dumperRotatePoint.y, -m_dumperRotatePoint.z);
//	D3DXMatrixTranslation(&dumperTempTrans2, m_dumperRotatePoint.x, m_dumperRotatePoint.y, m_dumperRotatePoint.z);
//	D3DXMatrixRotationAxis(&dumperRot, &m_dumperRotateAxis, m_dumperRotateAngle);
//	//worldMat(&m_vehiclePose->matDumper, Vec3(m_dumperActor->getGlobalPosition().x, m_dumperActor->getGlobalPosition().y, m_dumperActor->getGlobalPosition().z), Vec3(0, 0, 0));
//	//D3DXMatrixRotationQuaternion(&rot, &D3DXQUATERNION(m_dumperActor->getGlobalOrientationQuat().x, m_dumperActor->getGlobalOrientationQuat().y, m_dumperActor->getGlobalOrientationQuat().z, m_actor->getGlobalOrientationQuat().w));
//	m_vehiclePose->matDumper = dumperTrans * dumperTempTrans * dumperRot * dumperTempTrans2 * m_vehiclePose->matChassis;
//
//	D3DXVec3TransformNormal(&m_forward, &Vec3(1, 0, 0), &rot);
//
//	/*HARVEST*/
//	Mat harvestTrans;
//	D3DXMatrixTranslation(&harvestTrans, m_harvestPos.x, m_harvestPos.y + (m_dumperPayload / m_dumperCapacity)*m_harvestMaxHeight, m_harvestPos.z);
//	m_vehiclePose->matHarvest = harvestTrans * m_vehiclePose->matDumper;
//
//	if(m_trailerWheelNb == TWN_NO_WHEEL)
//		return (void*)m_vehiclePose;
//	ShapeUserData* sud;
//	//Wheels
//
//	//Front Left wheel
//
//	sud = (ShapeUserData*)(m_wheelFrontLeft->userData);
//	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matFrontLeftWheel);
//	m_vehiclePose->matFrontLeftWheel = m_matRotateWheels * m_vehiclePose->matFrontLeftWheel;
//
//	//Front Right wheel
//
//	sud = (ShapeUserData*)(m_wheelFrontRight->userData);
//	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matFrontRightWheel);
//	m_vehiclePose->matFrontRightWheel = m_matRotateWheels * m_vehiclePose->matFrontRightWheel;
//
//	if(m_trailerWheelNb != TWN_4WHEEL)
//		return (void*)m_vehiclePose;
//	//Rear Left wheel
//	
//	sud = (ShapeUserData*)(m_wheelRearLeft->userData);
//	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matRearLeftWheel);
//	m_vehiclePose->matRearLeftWheel = m_matRotateWheels * m_vehiclePose->matRearLeftWheel;
//
//	//Rear Right wheel
//	
//	sud = (ShapeUserData*)(m_wheelRearRight->userData);
//	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matRearRightWheel);
//	m_vehiclePose->matRearRightWheel = m_matRotateWheels * m_vehiclePose->matRearRightWheel;
//
//	return (void*)m_vehiclePose;
//}
//
//int Trailer::getSpeed()
//{
//	/*Vec3 Way = Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z) - m_oldPos;
//	m_oldPos = Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z);
//	return D3DXVec3Length(&Way) / core.dynamics->getDeltaTime();*/
//	return 0;
//}
//
//
//Vec3 Trailer::getJointPoint()
//{
//	Vec3 retVal;
//	D3DXVec3TransformCoord(&retVal, &m_jointPoint, &((TrailerPose*)getVehiclePose())->matChassis);
//	return retVal;
//}
//
//void Trailer::joinVehicle()
//{
//	m_joined = true;
//}
//
//void Trailer::unjoinVehicle()
//{
//	m_joined = false;
//}
//
//void Trailer::action1()
//{
//}
//
//void Trailer::action2()
//{
//}
//
//void Trailer::action3()
//{
//}

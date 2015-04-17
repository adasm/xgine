#include "WheelSet.h"

vector<CfgVariable*> *WheelSetParams::getVariableList()
{
	if(m_varList)
		return m_varList;
	m_varList = new vector<CfgVariable*>;

	m_varList->push_back(new CfgVariable(V_FLOAT, &chassisDensity, getVarName(chassisDensity)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &frontWheelsRadius, getVarName(frontWheelsRadius)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &frontWheelsWidth, getVarName(frontWheelsWidth)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &middleWheelsRadius, getVarName(middleWheelsRadius)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &middleWheelsWidth, getVarName(middleWheelsWidth)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &rearWheelsRadius, getVarName(rearWheelsRadius)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &rearWheelsWidth, getVarName(rearWheelsWidth)));

	m_varList->push_back(new CfgVariable(V_VEC3, &posWheelLeftFront, getVarName(posWheelLeftFront)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posWheelRightFront, getVarName(posWheelRightFront)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posWheelLeftMiddle, getVarName(posWheelLeftMiddle)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posWheelRightMiddle, getVarName(posWheelRightMiddle)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posWheelLeftRear, getVarName(posWheelLeftRear)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posWheelRightRear, getVarName(posWheelRightRear)));

	m_varList->push_back(new CfgVariable(V_VEC3, &vecWheelLeftFrontRotation, getVarName(vecWheelLeftFrontRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecWheelRightFrontRotation, getVarName(vecWheelRightFrontRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecWheelLeftMiddleRotation, getVarName(vecWheelLeftMiddleRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecWheelRightMiddleRotation, getVarName(vecWheelRightMiddleRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecWheelLeftRearRotation, getVarName(vecWheelLeftRearRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecWheelRightRearRotation, getVarName(vecWheelRightRearRotation)));
	//m_varList->push_back(new CfgVariable(V_VEC3, &vecChassisForward, getVarName(vecChassisForward)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posConnector, getVarName(posConnector)));

	return m_varList;
}


WheelSet::WheelSet(Vec3 pos, WheelSetParams* params, VehicleParamsEx* paramsEx)
{
	m_pose = new WheelSetPose;

	m_forward = Vec3(1, 0, 0);

	m_params = params;
	m_paramsEx = paramsEx;

	m_wheelLeftFront = NULL;
	m_wheelRightFront = NULL;

	m_wheelLeftMiddle = NULL;
	m_wheelRightMiddle = NULL;

	m_wheelLeftRear = NULL;
	m_wheelRightRear = NULL;

	m_poseGiven = false;


	m_forward = Vec3(0, 0, 0);

	m_scene = core.dynamics->getScene();

	D3DXMatrixIdentity(&m_pose->matChassis);
	D3DXMatrixIdentity(&m_pose->matWheelLeftFront);
	D3DXMatrixIdentity(&m_pose->matWheelRightFront);
	D3DXMatrixIdentity(&m_pose->matWheelLeftMiddle);
	D3DXMatrixIdentity(&m_pose->matWheelRightMiddle);
	D3DXMatrixIdentity(&m_pose->matWheelLeftRear);
	D3DXMatrixIdentity(&m_pose->matWheelRightRear);

	m_actor = core.dynamics->createBox(NxVec3(pos), NxVec3(m_params->chassisDimm), m_params->chassisDensity);
	SetActorCollisionGroup(m_actor, GROUP_COLLIDABLE_NON_PUSHABLE);

	//Wheel material globals
	NxMaterial* wsm = NULL;

	NxTireFunctionDesc lngTFD;
	lngTFD.extremumSlip = 1.0f;
	lngTFD.extremumValue = 0.02f;
	lngTFD.asymptoteSlip = 2.0f;
	lngTFD.asymptoteValue = 0.01f;	
	lngTFD.stiffnessFactor = 1000000.0f;

	NxTireFunctionDesc latTFD;
	latTFD.extremumSlip = 1.0f;
	latTFD.extremumValue = 0.02f;
	latTFD.asymptoteSlip = 2.0f;
	latTFD.asymptoteValue = 0.01f;	
	latTFD.stiffnessFactor = 1000000.0f;

	NxTireFunctionDesc slipTFD;
	slipTFD.extremumSlip = 1.0f;
	slipTFD.extremumValue = 0.02f;
	slipTFD.asymptoteSlip = 2.0f;
	slipTFD.asymptoteValue = 0.01f;	
	slipTFD.stiffnessFactor = 100.0f;  

	NxTireFunctionDesc medTFD;
	medTFD.extremumSlip = 1.0f;
	medTFD.extremumValue = 0.02f;
	medTFD.asymptoteSlip = 2.0f;
	medTFD.asymptoteValue = 0.01f;	
	medTFD.stiffnessFactor = 10000.0f;

	// Front left wheel
	NxWheelDesc wheelDesc;
	wheelDesc.frictionToFront = 100.0f;
	wheelDesc.frictionToSide = 1.0f;


	wheelDesc.wheelApproximation = 0;
	wheelDesc.wheelRadius = m_params->frontWheelsRadius;
	wheelDesc.wheelWidth = m_params->frontWheelsWidth;  // 0.1
	wheelDesc.wheelSuspension = 10;  
	wheelDesc.springRestitution = 0.1f;
	wheelDesc.springDamping = 10;
	wheelDesc.springBias = 10;  
	wheelDesc.maxBrakeForce = 0.01f;
	wheelDesc.position = NxVec3(m_params->posWheelLeftFront);
	wheelDesc.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
						   NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT | NX_WF_AXLE_SPEED_OVERRIDE;

	// Front right wheel
	NxWheelDesc wheelDesc2;
	wheelDesc2.frictionToFront = 100.0f;
	wheelDesc2.frictionToSide = 1.0f;



	wheelDesc2.wheelApproximation = 0;
	wheelDesc2.wheelRadius = m_params->frontWheelsRadius;
	wheelDesc2.wheelWidth = m_params->frontWheelsWidth;  // 0.1
	wheelDesc2.wheelSuspension = 10;  
	wheelDesc2.springRestitution = 0.1f;
	wheelDesc2.springDamping = 10;
	wheelDesc2.springBias = 10;  
	wheelDesc2.maxBrakeForce = 0.01f;
	wheelDesc2.position = NxVec3(m_params->posWheelRightFront);
	wheelDesc2.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT | NX_WF_AXLE_SPEED_OVERRIDE;

	// Middle left wheel
	NxWheelDesc wheelDesc3;
	wheelDesc3.frictionToFront = 100.0f;
	wheelDesc3.frictionToSide = 1.0f;



	wheelDesc3.wheelApproximation = 0;
	wheelDesc3.wheelRadius = m_params->middleWheelsRadius;
	wheelDesc3.wheelWidth = m_params->middleWheelsWidth;  // 0.1
	wheelDesc3.wheelSuspension = 10;  
	wheelDesc3.springRestitution = 0.1f;
	wheelDesc3.springDamping = 10;
	wheelDesc3.springBias = 10;  
	wheelDesc3.maxBrakeForce = 0.01f;
	wheelDesc3.position = NxVec3(m_params->posWheelLeftMiddle);
	wheelDesc3.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT | NX_WF_AXLE_SPEED_OVERRIDE;

	// Middle right wheel
	NxWheelDesc wheelDesc4;
	wheelDesc4.frictionToFront = 100.0f;
	wheelDesc4.frictionToSide = 1.0f;


	wheelDesc4.wheelApproximation = 0;
	wheelDesc4.wheelRadius = m_params->middleWheelsRadius;
	wheelDesc4.wheelWidth = m_params->middleWheelsWidth;  // 0.1
	wheelDesc4.wheelSuspension = 10;  
	wheelDesc4.springRestitution = 0.1f;
	wheelDesc4.springDamping = 10;
	wheelDesc4.springBias = 10;  
	wheelDesc4.maxBrakeForce = 0.01f;
	wheelDesc4.position = NxVec3(m_params->posWheelRightMiddle);
	wheelDesc4.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT | NX_WF_AXLE_SPEED_OVERRIDE;

	// Rear left wheel
	NxWheelDesc wheelDesc5;
	wheelDesc5.frictionToFront = 100.0f;
	wheelDesc5.frictionToSide = 1.0f;



	wheelDesc5.wheelApproximation = 0;
	wheelDesc5.wheelRadius = m_params->rearWheelsRadius;
	wheelDesc5.wheelWidth = m_params->rearWheelsWidth;  // 0.1
	wheelDesc5.wheelSuspension = 10;  
	wheelDesc5.springRestitution = 0.1f;
	wheelDesc5.springDamping = 10;
	wheelDesc5.springBias = 10;  
	wheelDesc5.maxBrakeForce = 0.01f;
	wheelDesc5.position = NxVec3(m_params->posWheelLeftRear);
	wheelDesc5.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT | NX_WF_AXLE_SPEED_OVERRIDE;

	// Rear right wheel
	NxWheelDesc wheelDesc6;
	wheelDesc6.frictionToFront = 100.0f;
	wheelDesc6.frictionToSide = 1.0f;


	wheelDesc6.wheelApproximation = 0;
	wheelDesc6.wheelRadius = m_params->rearWheelsRadius;
	wheelDesc6.wheelWidth = m_params->rearWheelsWidth;  // 0.1
	wheelDesc6.wheelSuspension = 10;  
	wheelDesc6.springRestitution = 0.1f;
	wheelDesc6.springDamping = 10;
	wheelDesc6.springBias = 10;  
	wheelDesc6.maxBrakeForce = 0.01f;
	wheelDesc6.position = NxVec3(m_params->posWheelRightRear);
	wheelDesc6.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT | NX_WF_AXLE_SPEED_OVERRIDE;


	wheelDesc6.frictionToFront = wheelDesc5.frictionToFront = wheelDesc4.frictionToFront = wheelDesc3.frictionToFront = wheelDesc2.frictionToFront = wheelDesc.frictionToFront = 3.0f;
	wheelDesc6.frictionToSide = wheelDesc5.frictionToSide = 3.0f;
	wheelDesc4.frictionToSide = wheelDesc3.frictionToSide = 3.0f;
	wheelDesc2.frictionToSide = wheelDesc.frictionToSide = 3.0f;


	/*wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.0001;
	wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 0.0001;
	wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.001;
	wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 0.001;
	wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01;*/

	if(!m_paramsEx)
	{
		wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.05f;
		wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 1000.0f;
		wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.5f / 10000000000000;
		wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 1.0f / 10000000000000;
		wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01f; // 100;
		/*
		wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.5f;
		wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 1000.0f;
		wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.5f / 1;
		wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 1.0f / 100000;
		wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01f; // 100;*/
	}
	else
	{
		wheelDesc6.frictionToFront = wheelDesc5.frictionToFront = wheelDesc4.frictionToFront = wheelDesc3.frictionToFront = wheelDesc2.frictionToFront = wheelDesc.frictionToFront = m_paramsEx->frictionToFront;
		wheelDesc6.frictionToSide = wheelDesc5.frictionToSide = wheelDesc4.frictionToSide = wheelDesc3.frictionToSide = wheelDesc2.frictionToSide = wheelDesc.frictionToSide = m_paramsEx->frictionToSide;

		wheelDesc6.wheelSuspension = wheelDesc5.wheelSuspension = wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = m_paramsEx->wheelSuspension;
		wheelDesc6.springRestitution = wheelDesc5.springRestitution = wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = m_paramsEx->springRestitution;
		wheelDesc6.springDamping = wheelDesc5.springDamping = wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = m_paramsEx->springDamping;
		wheelDesc6.springBias = wheelDesc5.springBias = wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = m_paramsEx->springBias;
		wheelDesc6.maxBrakeForce = wheelDesc5.maxBrakeForce = wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = m_paramsEx->maxBrakeForce; // 100;
	}

	if(m_params->posWheelLeftFront != Vec3(0, 0, 0))
	{
		m_wheelLeftFront = AddWheelToActor(m_actor, &wheelDesc);
		m_wheelLeftFront->userData = (void*)new ShapeUserData;
		((ShapeUserData*)m_wheelLeftFront->userData)->vehicleActor = m_actor;
	}
	if(m_params->posWheelRightFront != Vec3(0, 0, 0))
	{
		m_wheelRightFront = AddWheelToActor(m_actor, &wheelDesc2);
		m_wheelRightFront->userData = (void*)new ShapeUserData;
		((ShapeUserData*)m_wheelRightFront->userData)->vehicleActor = m_actor;
	}

	if(m_params->posWheelLeftMiddle != Vec3(0, 0, 0))
	{
		m_wheelLeftMiddle = AddWheelToActor(m_actor, &wheelDesc3);
		m_wheelLeftMiddle->userData = (void*)new ShapeUserData;
		((ShapeUserData*)m_wheelLeftMiddle->userData)->vehicleActor = m_actor;
	}
	if(m_params->posWheelRightMiddle != Vec3(0, 0, 0))
	{
		m_wheelRightMiddle = AddWheelToActor(m_actor, &wheelDesc4);
		m_wheelRightMiddle->userData = (void*)new ShapeUserData;
		((ShapeUserData*)m_wheelRightMiddle->userData)->vehicleActor = m_actor;
	}

	if(m_params->posWheelLeftRear != Vec3(0, 0, 0))
	{
		m_wheelLeftRear = AddWheelToActor(m_actor, &wheelDesc5);
		m_wheelLeftRear->userData = (void*)new ShapeUserData;
		((ShapeUserData*)m_wheelLeftRear->userData)->vehicleActor = m_actor;
	}
	if(m_params->posWheelRightRear != Vec3(0, 0, 0))
	{
		m_wheelRightRear = AddWheelToActor(m_actor, &wheelDesc6);
		m_wheelRightRear->userData = (void*)new ShapeUserData;
		((ShapeUserData*)m_wheelRightRear->userData)->vehicleActor = m_actor;
	}



	// LOWER THE CENTER OF MASS
	NxVec3 massPos = m_actor->getCMassLocalPosition();
	massPos.y = m_params->posWheelLeftFront.y - m_params->frontWheelsRadius;
	m_actor->setCMassOffsetLocalPosition(massPos);
	m_actor->wakeUp(1e30);
	//stopMotorAndBreak();
}

void WheelSet::update()
{
	m_poseGiven = false;
}

Vec3 WheelSet::getForwardVec()
{
	getPose();
	return m_forward;
}

Vec3 WheelSet::getConnectorPos()
{
	Vec3 retVal;
	D3DXVec3TransformCoord(&retVal, &m_params->posConnector, &getPose()->matChassis);
	return retVal;
}

NxActor* WheelSet::getActor()
{
	return m_actor;
}

WheelSetPose* WheelSet::getPose()
{
	if(m_poseGiven)
		return m_pose;
	m_poseGiven = true;
	/*CHASSIS*/
	Mat rot;
	worldMat(&m_pose->matChassis, Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z), Vec3(0, 0, 0));
	D3DXMatrixRotationQuaternion(&rot, &D3DXQUATERNION(m_actor->getGlobalOrientationQuat().x, m_actor->getGlobalOrientationQuat().y, m_actor->getGlobalOrientationQuat().z, m_actor->getGlobalOrientationQuat().w));

	Vec3 cross;
	/*Mat chassisRot;
	D3DXVec3Cross(&cross, &Vec3(1, 0, 0), &m_params->vecChassisForward);

	D3DXMatrixRotationAxis(&chassisRot, &cross, acos(D3DXVec3Dot(&Vec3(1, 0, 0), &m_params->vecChassisForward)));*/
	m_pose->matChassis = /*chassisRot * */rot * m_pose->matChassis;
	D3DXVec3TransformNormal(&m_forward, &Vec3(1, 0, 0), &rot);

	ShapeUserData* sud;


	//Front Left wheel

	Mat wheelRot;
	if(m_wheelLeftFront)
	{
		D3DXMatrixRotationYawPitchRoll(&wheelRot, m_params->vecWheelLeftFrontRotation.y, m_params->vecWheelLeftFrontRotation.x, m_params->vecWheelLeftFrontRotation.z);
		sud = (ShapeUserData*)(m_wheelLeftFront->userData);
		sud->wheelShapePose.getColumnMajor44((NxF32*)&m_pose->matWheelLeftFront);
		m_pose->matWheelLeftFront = wheelRot * m_pose->matWheelLeftFront;
	}

	//Front Right wheel

	if(m_wheelRightFront)
	{
		D3DXMatrixRotationYawPitchRoll(&wheelRot, m_params->vecWheelRightFrontRotation.y, m_params->vecWheelRightFrontRotation.x, m_params->vecWheelRightFrontRotation.z);
		sud = (ShapeUserData*)(m_wheelRightFront->userData);
		sud->wheelShapePose.getColumnMajor44((NxF32*)&m_pose->matWheelRightFront);
		m_pose->matWheelRightFront = wheelRot * m_pose->matWheelRightFront;
	}

	//Middle Left wheel
	
	if(m_wheelLeftMiddle)
	{
		D3DXMatrixRotationYawPitchRoll(&wheelRot, m_params->vecWheelLeftMiddleRotation.y, m_params->vecWheelLeftMiddleRotation.x, m_params->vecWheelLeftMiddleRotation.z);
		sud = (ShapeUserData*)(m_wheelLeftMiddle->userData);
		sud->wheelShapePose.getColumnMajor44((NxF32*)&m_pose->matWheelLeftMiddle);
		m_pose->matWheelLeftMiddle = wheelRot * m_pose->matWheelLeftMiddle;
	}

	//Middle Right wheel
	
	if(m_wheelRightMiddle)
	{
		D3DXMatrixRotationYawPitchRoll(&wheelRot, m_params->vecWheelRightMiddleRotation.y, m_params->vecWheelRightMiddleRotation.x, m_params->vecWheelRightMiddleRotation.z);
		sud = (ShapeUserData*)(m_wheelRightMiddle->userData);
		sud->wheelShapePose.getColumnMajor44((NxF32*)&m_pose->matWheelRightMiddle);
		m_pose->matWheelRightMiddle = wheelRot * m_pose->matWheelRightMiddle;
	}

	//Rear Left wheel
	
	if(m_wheelLeftRear)
	{
		D3DXMatrixRotationYawPitchRoll(&wheelRot, m_params->vecWheelLeftRearRotation.y, m_params->vecWheelLeftRearRotation.x, m_params->vecWheelLeftRearRotation.z);
		sud = (ShapeUserData*)(m_wheelLeftRear->userData);
		sud->wheelShapePose.getColumnMajor44((NxF32*)&m_pose->matWheelLeftRear);
		m_pose->matWheelLeftRear = wheelRot * m_pose->matWheelLeftRear;
	}

	//Rear Right wheel
	
	if(m_wheelRightRear)
	{
		D3DXMatrixRotationYawPitchRoll(&wheelRot, m_params->vecWheelRightRearRotation.y, m_params->vecWheelRightRearRotation.x, m_params->vecWheelRightRearRotation.z);
		sud = (ShapeUserData*)(m_wheelRightRear->userData);
		sud->wheelShapePose.getColumnMajor44((NxF32*)&m_pose->matWheelRightRear);
		m_pose->matWheelRightRear = wheelRot * m_pose->matWheelRightRear;
	}

	return m_pose;
}
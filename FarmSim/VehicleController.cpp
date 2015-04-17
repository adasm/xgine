#include "VehicleController.h"

//VehicleController::VehicleController(Dynamics *dyn, NxVec3 pos, VehicleDimms *dimm, bool steerableWheelsFront, VehicleParamsEx *params)
//{
//	m_scene = dyn->getScene();
//
//	m_vehiclePose = new VehiclePose;
//	m_vehicleDimms = dimm;
//
//	m_poseGiven = false;
//
//	m_setCurrentSteerAngle = false;
//	m_setCurrentMotorTorque = false;
//	m_setCurrentBrakeTorque = false;
//	m_steerableWheelsFront = steerableWheelsFront;
//
//	m_currentSteerAngle = 0.0f;
//	m_currentMotorTorque = 0.0f;
//	m_currentBrakeTorque = 0.0f;
//
//	D3DXMatrixIdentity(&m_vehiclePose->matChassis);
//	D3DXMatrixIdentity(&m_vehiclePose->matFrontLeftWheel);
//	D3DXMatrixIdentity(&m_vehiclePose->matFrontRightWheel);
//	D3DXMatrixIdentity(&m_vehiclePose->matRearLeftWheel);
//	D3DXMatrixIdentity(&m_vehiclePose->matRearRightWheel);
//
//	m_actor = dyn->createBox(pos, NxVec3(m_vehicleDimms->chassisDimm), m_vehicleDimms->chassisDensity);
//	SetActorCollisionGroup(m_actor, GROUP_COLLIDABLE_NON_PUSHABLE);
//
//	//Wheel material globals
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
//	wheelDesc.wheelApproximation = 0;
//	wheelDesc.wheelRadius = m_vehicleDimms->frontWheelsRadius;
//	wheelDesc.wheelWidth = m_vehicleDimms->frontWheelsWidth;  // 0.1
//	wheelDesc.wheelSuspension = 10;  
//	wheelDesc.springRestitution = 0.1f;
//	wheelDesc.springDamping = 10;
//	wheelDesc.springBias = 10;  
//	wheelDesc.maxBrakeForce = 0.01f;
//	wheelDesc.position = NxVec3(m_vehicleDimms->posFrontLeftWheel);
//	wheelDesc.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//						   NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT | NX_WF_AXLE_SPEED_OVERRIDE;
//
//	// Front right wheel
//	NxWheelDesc wheelDesc2;
//	wheelDesc2.frictionToFront = 100.0f;
//	wheelDesc2.frictionToSide = 1.0f;
//
//
//
//	wheelDesc2.wheelApproximation = 0;
//	wheelDesc2.wheelRadius = m_vehicleDimms->frontWheelsRadius;
//	wheelDesc2.wheelWidth = m_vehicleDimms->frontWheelsWidth;  // 0.1
//	wheelDesc2.wheelSuspension = 10;  
//	wheelDesc2.springRestitution = 0.1f;
//	wheelDesc2.springDamping = 10;
//	wheelDesc2.springBias = 10;  
//	wheelDesc.maxBrakeForce = 0.01f;
//	wheelDesc2.position = NxVec3(m_vehicleDimms->posFrontRightWheel);
//	wheelDesc2.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT | NX_WF_AXLE_SPEED_OVERRIDE;
//
//	// Rear left wheel
//	NxWheelDesc wheelDesc3;
//	wheelDesc3.frictionToFront = 100.0f;
//	wheelDesc3.frictionToSide = 1.0f;
//
//
//
//	wheelDesc3.wheelApproximation = 0;
//	wheelDesc3.wheelRadius = m_vehicleDimms->rearWheelsRadius;
//	wheelDesc3.wheelWidth = m_vehicleDimms->rearWheelsWidth;  // 0.1
//	wheelDesc3.wheelSuspension = 10;  
//	wheelDesc3.springRestitution = 0.1f;
//	wheelDesc3.springDamping = 10;
//	wheelDesc3.springBias = 10;  
//	wheelDesc3.maxBrakeForce = 0.01f;
//	wheelDesc3.position = NxVec3(m_vehicleDimms->posRearLeftWheel);
//	wheelDesc3.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT | NX_WF_AXLE_SPEED_OVERRIDE;
//
//	// Rear right wheel
//	NxWheelDesc wheelDesc4;
//	wheelDesc4.frictionToFront = 100.0f;
//	wheelDesc4.frictionToSide = 1.0f;
//
//
//	wheelDesc4.wheelApproximation = 0;
//	wheelDesc4.wheelRadius = m_vehicleDimms->rearWheelsRadius;
//	wheelDesc4.wheelWidth = m_vehicleDimms->rearWheelsWidth;  // 0.1
//	wheelDesc4.wheelSuspension = 10;  
//	wheelDesc4.springRestitution = 0.1f;
//	wheelDesc4.springDamping = 10;
//	wheelDesc4.springBias = 10;  
//	wheelDesc4.maxBrakeForce = 0.01f;
//	wheelDesc4.position = NxVec3(m_vehicleDimms->posRearRightWheel);
//	wheelDesc4.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT | NX_WF_AXLE_SPEED_OVERRIDE;
//
//
//	wheelDesc4.frictionToFront = wheelDesc3.frictionToFront = wheelDesc2.frictionToFront = wheelDesc.frictionToFront = 3.0f;
//	wheelDesc4.frictionToSide = wheelDesc3.frictionToSide = 3.0f;
//	wheelDesc2.frictionToSide = wheelDesc.frictionToSide = 3.0f;
//
//
//	/*wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.0001;
//	wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 0.0001;
//	wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.001;
//	wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 0.001;
//	wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01;*/
//
//	if(!params)
//	{
//		wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.05f;
//		wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 1000.0f;
//		wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.5f / 10000000000000;
//		wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 1.0f / 10000000000000;
//		wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01f; // 100;
//	}
//	else
//	{
//		wheelDesc4.frictionToFront = wheelDesc3.frictionToFront = wheelDesc2.frictionToFront = wheelDesc.frictionToFront = params->frictionToFront;
//		wheelDesc4.frictionToSide = wheelDesc3.frictionToSide = wheelDesc2.frictionToSide = wheelDesc.frictionToSide = params->frictionToSide;
//
//		wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = params->wheelSuspension;
//		wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = params->springRestitution;
//		wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = params->springDamping;
//		wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = params->springBias;
//		wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = params->maxBrakeForce; // 100;
//	}
//
//	m_wheelFrontLeft = AddWheelToActor(m_actor, &wheelDesc);
//	m_wheelFrontRight = AddWheelToActor(m_actor, &wheelDesc2);
//	m_wheelRearLeft = AddWheelToActor(m_actor, &wheelDesc3);
//	m_wheelRearRight = AddWheelToActor(m_actor, &wheelDesc4);
//
//	m_wheelFrontLeft->userData = (void*)new ShapeUserData;
//	m_wheelFrontRight->userData = (void*)new ShapeUserData;
//	m_wheelRearLeft->userData = (void*)new ShapeUserData;
//	m_wheelRearRight->userData = (void*)new ShapeUserData;
//
//	// LOWER THE CENTER OF MASS
//	NxVec3 massPos = m_actor->getCMassLocalPosition();
//	massPos.y = m_vehicleDimms->posFrontLeftWheel.y - m_vehicleDimms->frontWheelsRadius;
//	m_actor->setCMassOffsetLocalPosition(massPos);
//	m_actor->wakeUp(1e30);
//	stopMotorAndBreak();
//}
//
//
//NxActor* VehicleController::getActor()
//{
//	return m_actor;
//}
//
//VehiclePose* VehicleController::getVehiclePose()
//{
//	if(m_poseGiven)
//		return m_vehiclePose;
//	m_poseGiven = true;
//	/*CHASSIS*/
//	Mat rot;
//	worldMat(&m_vehiclePose->matChassis, Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z), Vec3(0, 0, 0));
//	D3DXMatrixRotationQuaternion(&rot, &D3DXQUATERNION(m_actor->getGlobalOrientationQuat().x, m_actor->getGlobalOrientationQuat().y, m_actor->getGlobalOrientationQuat().z, m_actor->getGlobalOrientationQuat().w));
//	m_vehiclePose->matChassis = m_vehicleDimms->matChassisRotation * rot * m_vehiclePose->matChassis;
//	D3DXVec3TransformNormal(&m_forward, &Vec3(1, 0, 0), &rot);
//
//	ShapeUserData* sud;
//	//Wheels
//
//	//Front Left wheel
//
//	sud = (ShapeUserData*)(m_wheelFrontLeft->userData);
//	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matFrontLeftWheel);
//	m_vehiclePose->matFrontLeftWheel = m_vehicleDimms->matFrontLeftWheelRotation * m_vehiclePose->matFrontLeftWheel;
//
//	//Front Right wheel
//
//	sud = (ShapeUserData*)(m_wheelFrontRight->userData);
//	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matFrontRightWheel);
//	m_vehiclePose->matFrontRightWheel = m_vehicleDimms->matFrontRightWheelRotation * m_vehiclePose->matFrontRightWheel;
//
//	//Rear Left wheel
//	
//	sud = (ShapeUserData*)(m_wheelRearLeft->userData);
//	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matRearLeftWheel);
//	m_vehiclePose->matRearLeftWheel = m_vehicleDimms->matRearLeftWheelRotation * m_vehiclePose->matRearLeftWheel;
//
//	//Rear Right wheel
//	
//	sud = (ShapeUserData*)(m_wheelRearRight->userData);
//	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matRearRightWheel);
//	m_vehiclePose->matRearRightWheel = m_vehicleDimms->matRearRightWheelRotation * m_vehiclePose->matRearRightWheel;
//
//	return m_vehiclePose;
//}
//VehicleDimms* VehicleController::getVehicleDimms()
//{
//	return m_vehicleDimms;
//}
//void VehicleController::update()
//{
//	m_poseGiven = false;
//	VehiclePose *vehiclePose = (VehiclePose*)getVehiclePose();
//	m_oldPos = Vec3(vehiclePose->matChassis._41, vehiclePose->matChassis._42, vehiclePose->matChassis._43);
//	if (m_setCurrentSteerAngle)
//	{
//		m_setCurrentSteerAngle = false;
//        m_wheelFrontLeft->setSteerAngle(m_currentSteerAngle);
//        m_wheelFrontRight->setSteerAngle(m_currentSteerAngle);
//	}
//
//	if (m_setCurrentMotorTorque)
//	{
//		m_setCurrentMotorTorque = false;
//        m_wheelFrontLeft->setMotorTorque(m_currentMotorTorque);
//        m_wheelFrontRight->setMotorTorque(m_currentMotorTorque);
//		m_wheelRearLeft->setMotorTorque(m_currentMotorTorque);
//        m_wheelRearRight->setMotorTorque(m_currentMotorTorque);
//	}
//
//	if (m_setCurrentBrakeTorque)
//	{
//		m_setCurrentBrakeTorque = false;
//		m_wheelFrontLeft->setBrakeTorque(m_currentBrakeTorque);
//		m_wheelFrontRight->setBrakeTorque(m_currentBrakeTorque);
//		m_wheelRearLeft->setBrakeTorque(m_currentBrakeTorque);
//		m_wheelRearRight->setBrakeTorque(m_currentBrakeTorque);
//	}
//
//	/*if(m_objChassis)
//	{
//		m_objChassis->boundingBox.setTransform(Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z));
//		m_objChassis->world = vehiclePose->matChassis;
//		m_objChassis->position = Vec3(vehiclePose->matChassis._41, vehiclePose->matChassis._42, vehiclePose->matChassis._43);
//	}
//
//	if(m_objFrontLeftWheel)
//	{
//		m_objFrontLeftWheel->boundingBox.setTransform(Vec3(vehiclePose->matFrontLeftWheel(3, 0), vehiclePose->matFrontLeftWheel(3, 1), vehiclePose->matFrontLeftWheel(3, 2)));
//		m_objFrontLeftWheel->world = vehiclePose->matFrontLeftWheel;
//		m_objFrontLeftWheel->position = Vec3(vehiclePose->matFrontLeftWheel._41, vehiclePose->matFrontLeftWheel._42, vehiclePose->matFrontLeftWheel._43);
//	}
//
//	if(m_objFrontRightWheel)
//	{
//		m_objFrontRightWheel->boundingBox.setTransform(Vec3(vehiclePose->matFrontRightWheel(3, 0), vehiclePose->matFrontRightWheel(3, 1), vehiclePose->matFrontRightWheel(3, 2)));
//		m_objFrontRightWheel->world = vehiclePose->matFrontRightWheel;
//		m_objFrontRightWheel->position = Vec3(vehiclePose->matFrontRightWheel._41, vehiclePose->matFrontRightWheel._42, vehiclePose->matFrontRightWheel._43);
//	}
//
//	if(m_objRearLeftWheel)
//	{
//		m_objRearLeftWheel->boundingBox.setTransform(Vec3(vehiclePose->matRearLeftWheel(3, 0), vehiclePose->matRearLeftWheel(3, 1), vehiclePose->matRearLeftWheel(3, 2)));
//		m_objRearLeftWheel->world = vehiclePose->matRearLeftWheel;
//		m_objRearLeftWheel->position = Vec3(vehiclePose->matRearLeftWheel._41, vehiclePose->matRearLeftWheel._42, vehiclePose->matRearLeftWheel._43);
//	}
//
//	if(m_objRearRightWheel)
//	{
//		m_objRearRightWheel->boundingBox.setTransform(Vec3(vehiclePose->matRearRightWheel(3, 0), vehiclePose->matRearRightWheel(3, 1), vehiclePose->matRearRightWheel(3, 2)));
//		m_objRearRightWheel->world = vehiclePose->matRearRightWheel;
//		m_objRearRightWheel->position = Vec3(vehiclePose->matRearRightWheel._41, vehiclePose->matRearRightWheel._42, vehiclePose->matRearRightWheel._43);
//	}*/
//}
//
//void VehicleController::steerAngle(float angle)
//{
//	if(m_steerableWheelsFront)
//		m_currentSteerAngle += angle;
//	else
//		m_currentSteerAngle -= angle;
//	if(m_currentSteerAngle >= m_vehicleDimms->maxSteerAngle)
//		m_currentSteerAngle = m_vehicleDimms->maxSteerAngle;
//	if(m_currentSteerAngle <= -m_vehicleDimms->maxSteerAngle)
//		m_currentSteerAngle = -m_vehicleDimms->maxSteerAngle;
//	m_setCurrentSteerAngle = true;
//}
//
//void VehicleController::increaseMotorTorque(float torque)
//{
//	m_currentBrakeTorque = 0;
//	m_currentMotorTorque += torque;
//	m_setCurrentMotorTorque = true;
//	m_setCurrentBrakeTorque = true;
//}
//
//void VehicleController::increaseBreakTorque(float torque)
//{
//	m_currentBrakeTorque += torque;
//	m_setCurrentBrakeTorque = true;
//}
//
//float VehicleController::getSpeed()
//{
//	return m_wheelRearLeft->getAxleSpeed() * (m_wheelRearLeft->getRadius() * 2 * D3DX_PI) / gEngine.kernel->tmr->get();
//}
//
//void VehicleController::adjustSteerAngle()
//{
//	m_currentSteerAngle = 0;
//	m_setCurrentSteerAngle = true;
//}
//void VehicleController::stopMotorAndBreak()
//{
//	m_currentBrakeTorque = 3;
//	m_currentMotorTorque = 0;
//	m_setCurrentMotorTorque = true;
//	m_setCurrentBrakeTorque = true;
//}
//void VehicleController::stopMotor()
//{
//	m_currentMotorTorque = 0;
//	m_setCurrentMotorTorque = true;
//}
//
//void VehicleController::setMotorTorque(float torque)
//{
//	stopMotor();
//	increaseMotorTorque(torque);
//}
//
//float VehicleController::getMotorTorque()
//{
//	return m_currentMotorTorque;
//}
//
//Vec3 VehicleController::getForwardVec()
//{
//	getVehiclePose();
//	return m_forward;
//}
//
//Vec3 VehicleController::getFrontJointPoint()
//{
//	Vec3 retVal;
//	D3DXVec3TransformCoord(&retVal, &getVehicleDimms()->posTriPodFront, &getVehiclePose()->matChassis);
//	return retVal;
//}
//
//Vec3 VehicleController::getBackJointPoint()
//{
//	Vec3 retVal;
//	D3DXVec3TransformCoord(&retVal, &getVehicleDimms()->posTriPodBack, &getVehiclePose()->matChassis);
//	return retVal;
//}
//
//float VehicleController::getSteerAngle()
//{
//	if(m_steerableWheelsFront)
//		return m_currentSteerAngle;
//	return -m_currentSteerAngle;
//}

vector<CfgVariable*> *VehicleParamsEx::getVariableList()
{
	if(varList)
		return varList;
	varList = new vector<CfgVariable*>;

	varList->push_back(new CfgVariable(V_FLOAT, &frictionToFront, getVarName(frictionToFront)));
	varList->push_back(new CfgVariable(V_FLOAT, &frictionToSide, getVarName(frictionToSide)));
	varList->push_back(new CfgVariable(V_FLOAT, &wheelSuspension, getVarName(wheelSuspension)));
	varList->push_back(new CfgVariable(V_FLOAT, &springRestitution, getVarName(springRestitution)));
	varList->push_back(new CfgVariable(V_FLOAT, &springDamping, getVarName(springDamping)));
	varList->push_back(new CfgVariable(V_FLOAT, &springBias, getVarName(springBias)));
	varList->push_back(new CfgVariable(V_FLOAT, &maxBrakeForce, getVarName(maxBrakeForce)));

	varList->push_back(new CfgVariable(V_VEC3, &massCenter, getVarName(massCenter)));

	return varList;
}


VehicleController::VehicleController(Vec3 pos, VehicleEngine* vehEngine, VehicleParams *vehParams, VehicleParamsEx *vehParamsEx, bool steerableWheelsFront)
{
	m_forward = Vec3(0, 0, 0);

	m_scene = core.dynamics->getScene();
	m_vehiclePose = new VehiclePose;
	m_vehicleParams = vehParams;
	m_vehicleParamsEx = vehParamsEx;
	m_vehicleEngine = vehEngine;
	m_actor = NULL;

	m_wheelFrontLeft = NULL;
	m_wheelFrontRight = NULL;
	m_wheelRearLeft = NULL;
	m_wheelRearRight = NULL;

	m_poseGiven = false;

	m_setCurrentSteerAngle = false;
	m_setCurrentMotorTorque = false;
	m_setCurrentBrakeTorque = false;

	m_currentSteerAngle = 0.0f;
	m_currentMotorTorque = 0.0f;
	m_currentBrakeTorque = 0.0f;

	m_steerableWheelsFront = steerableWheelsFront;

	switch(m_vehicleParams->wheelDriveType)
	{
		case 0:
			m_wheelDriveType = WDT_2WD;
			break;
		case 1:
			m_wheelDriveType = WDT_4WD;
			break;
		case 2:
			m_wheelDriveType = WDT_FWD;
			break;
		default:
			m_wheelDriveType = WDT_2WD;
	}

	D3DXMatrixIdentity(&m_vehiclePose->matChassis);
	D3DXMatrixIdentity(&m_vehiclePose->matFrontLeftWheel);
	D3DXMatrixIdentity(&m_vehiclePose->matFrontRightWheel);
	D3DXMatrixIdentity(&m_vehiclePose->matRearLeftWheel);
	D3DXMatrixIdentity(&m_vehiclePose->matRearRightWheel);

	m_actor = core.dynamics->createBox(NxVec3(pos), NxVec3(m_vehicleParams->chassisDimm), m_vehicleParams->chassisDensity);
	SetActorCollisionGroup(m_actor, GROUP_COLLIDABLE_NON_PUSHABLE);

	//Wheel material globals
	NxMaterial* wsm = NULL;

	NxTireFunctionDesc lngTFD;
	lngTFD.extremumSlip = 1.0f;
	lngTFD.extremumValue = 0.02f;
	lngTFD.asymptoteSlip = 2.0f;
	lngTFD.asymptoteValue = 0.01f;	
	lngTFD.stiffnessFactor = 100.0f;

	NxTireFunctionDesc latTFD;
	latTFD.extremumSlip = 1.0f;
	latTFD.extremumValue = 0.02f;
	latTFD.asymptoteSlip = 2.0f;
	latTFD.asymptoteValue = 0.01f;	
	latTFD.stiffnessFactor = 100.0f;

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
	medTFD.stiffnessFactor = 100.0f;

	// Front left wheel
	NxWheelDesc wheelDesc;
	wheelDesc.frictionToFront = 3.0f;
	wheelDesc.frictionToSide = 3.0f;


	/*wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.0001;
	wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 0.0001;
	wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.001;
	wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 0.001;
	wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01;*/

	if(!m_vehicleParamsEx)
	{
		wheelDesc.wheelSuspension = 0.05f;
		wheelDesc.springRestitution = 1000.0f;
		wheelDesc.springDamping = 0.5f / 10000000000000;
		wheelDesc.springBias = 1.0f / 10000000000000;
		wheelDesc.maxBrakeForce = 0.01f; // 100;
		/*
		wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.5f;
		wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 1000.0f;
		wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.5f / 1;
		wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 1.0f / 100000;
		wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01f; // 100;*/
	}
	else
	{
		wheelDesc.frictionToFront = m_vehicleParamsEx->frictionToFront;
		wheelDesc.frictionToSide = m_vehicleParamsEx->frictionToSide;

		wheelDesc.wheelSuspension = m_vehicleParamsEx->wheelSuspension;
		wheelDesc.springRestitution = m_vehicleParamsEx->springRestitution;
		wheelDesc.springDamping = m_vehicleParamsEx->springDamping;
		wheelDesc.springBias = m_vehicleParamsEx->springBias;
		wheelDesc.maxBrakeForce = m_vehicleParamsEx->maxBrakeForce; // 100;
	}


	wheelDesc.wheelApproximation = 0;
	wheelDesc.wheelRadius = m_vehicleParams->frontWheelsRadius;
	wheelDesc.wheelWidth = m_vehicleParams->frontWheelsWidth;  // 0.1
	wheelDesc.position = NxVec3(m_vehicleParams->posFrontLeftWheel);
	wheelDesc.wheelFlags = NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
						   NX_WF_STEERABLE_INPUT | NX_WF_CLAMPED_FRICTION;

	// Front right wheel
	NxWheelDesc wheelDesc2 = wheelDesc;
	wheelDesc2.wheelRadius = m_vehicleParams->frontWheelsRadius;
	wheelDesc2.wheelWidth = m_vehicleParams->frontWheelsWidth;  // 0.1
	wheelDesc2.position = NxVec3(m_vehicleParams->posFrontRightWheel);
	
	// Rear left wheel
	NxWheelDesc wheelDesc3 = wheelDesc;
	wheelDesc3.wheelRadius = m_vehicleParams->rearWheelsRadius;
	wheelDesc3.wheelWidth = m_vehicleParams->rearWheelsWidth;  // 0.1
	wheelDesc3.position = NxVec3(m_vehicleParams->posRearLeftWheel);
	
	// Rear right wheel
	NxWheelDesc wheelDesc4 = wheelDesc;
	wheelDesc4.wheelRadius = m_vehicleParams->rearWheelsRadius;
	wheelDesc4.wheelWidth = m_vehicleParams->rearWheelsWidth;  // 0.1
	wheelDesc4.position = NxVec3(m_vehicleParams->posRearRightWheel);


	m_wheelFrontLeft = AddWheelToActor(m_actor, &wheelDesc);
	m_wheelFrontRight = AddWheelToActor(m_actor, &wheelDesc2);
	m_wheelRearLeft = AddWheelToActor(m_actor, &wheelDesc3);
	m_wheelRearRight = AddWheelToActor(m_actor, &wheelDesc4);

	m_wheelFrontLeft->userData = (void*)new ShapeUserData;
	m_wheelFrontRight->userData = (void*)new ShapeUserData;
	m_wheelRearLeft->userData = (void*)new ShapeUserData;
	m_wheelRearRight->userData = (void*)new ShapeUserData;

	((ShapeUserData*)m_wheelFrontLeft->userData)->vehicleActor = m_actor;
	((ShapeUserData*)m_wheelFrontRight->userData)->vehicleActor = m_actor;
	((ShapeUserData*)m_wheelRearLeft->userData)->vehicleActor = m_actor;
	((ShapeUserData*)m_wheelRearRight->userData)->vehicleActor = m_actor;

	m_wheelFrontLeft->setWheelFlags(m_wheelFrontLeft->getWheelFlags() | NX_WF_INPUT_LAT_SLIPVELOCITY  );
	m_wheelFrontRight->setWheelFlags(m_wheelFrontRight->getWheelFlags() | NX_WF_INPUT_LAT_SLIPVELOCITY  );
	m_wheelRearLeft->setWheelFlags(m_wheelRearLeft->getWheelFlags() | NX_WF_INPUT_LAT_SLIPVELOCITY  );
	m_wheelRearRight->setWheelFlags(m_wheelRearRight->getWheelFlags() | NX_WF_INPUT_LAT_SLIPVELOCITY  );

	// LOWER THE CENTER OF MASS
	NxVec3 massPos = m_actor->getCMassLocalPosition();
	massPos.y = m_vehicleParams->posFrontLeftWheel.y - m_vehicleParams->frontWheelsRadius;
	m_actor->setCMassOffsetLocalPosition(massPos);
	m_actor->wakeUp(1e30);
	//stopMotorAndBreak();
}

void VehicleController::setSteerAngle(float angle)
{
	if(!angle)
	{
		m_currentSteerAngle = 0;
		m_setCurrentSteerAngle = true;
		return;
	}
	if(!m_steerableWheelsFront)
		m_currentSteerAngle += angle;
	else
		m_currentSteerAngle -= angle;
	if(m_currentSteerAngle >= m_vehicleParams->maxSteerAngle)
		m_currentSteerAngle = m_vehicleParams->maxSteerAngle;
	if(m_currentSteerAngle <= -m_vehicleParams->maxSteerAngle)
		m_currentSteerAngle = -m_vehicleParams->maxSteerAngle;
	m_setCurrentSteerAngle = true;
}

void VehicleController::setAccelerate()
{
	m_vehicleEngine->setAccelerate();
	//m_currentBrakeTorque = 0;
	m_currentMotorTorque = 4.0f;
	m_setCurrentMotorTorque = true;
	//m_setCurrentBrakeTorque = true;
}

void VehicleController::setBackward()
{
	m_vehicleEngine->setReverse();
	//m_currentBrakeTorque = 0;
	m_currentMotorTorque = -4.0f;
	m_setCurrentMotorTorque = true;
	//m_setCurrentBrakeTorque = true;
}

void VehicleController::setBrake()
{
	m_vehicleEngine->setBrake();
	m_currentBrakeTorque = 4.0f;
	//m_currentMotorTorque = 4.0f;
	//m_setCurrentMotorTorque = true;
	m_setCurrentBrakeTorque = true;
}

void VehicleController::setParkingBrake()
{
	m_currentBrakeTorque = 4.0f;
	//m_currentMotorTorque = 4.0f;
	//m_setCurrentMotorTorque = true;
	m_setCurrentBrakeTorque = true;
}

void VehicleController::releaseBrake()
{
	m_currentBrakeTorque = 0;
	m_setCurrentBrakeTorque = true;
}

void VehicleController::releaseParkingBrake()
{
	m_currentBrakeTorque = 0;
	m_setCurrentBrakeTorque = true;
}

void VehicleController::releaseMotor()
{
	m_currentMotorTorque = 0.0f;
	m_setCurrentMotorTorque = true;
}

void VehicleController::update()
{
	//gEngine.renderer->addTxt("AxleSpeed: %f AxleSpeed/Radius: %f Speed: %f", m_wheelRearLeft->getAxleSpeed(), m_wheelRearLeft->getAxleSpeed()/m_wheelRearLeft->getRadius(), getSpeed());
	m_poseGiven = false;
	VehiclePose *vehiclePose = (VehiclePose*)getVehiclePose();
	m_vehicleEngine->update();
	if(m_setCurrentSteerAngle)
	{
		m_setCurrentSteerAngle = false;
        m_wheelFrontLeft->setSteerAngle(m_currentSteerAngle);
        m_wheelFrontRight->setSteerAngle(m_currentSteerAngle);
	}

/*	if (m_setCurrentMotorTorque)
	{
		m_setCurrentMotorTorque = false;
        m_wheelFrontLeft->setMotorTorque(m_currentMotorTorque);
        m_wheelFrontRight->setMotorTorque(m_currentMotorTorque);
		m_wheelRearLeft->setMotorTorque(m_currentMotorTorque);
        m_wheelRearRight->setMotorTorque(m_currentMotorTorque);
	}
*/
	if (m_setCurrentBrakeTorque)
	{
		m_setCurrentBrakeTorque = false;
		m_wheelFrontLeft->setBrakeTorque(m_currentBrakeTorque);
		m_wheelFrontRight->setBrakeTorque(m_currentBrakeTorque);
		m_wheelRearLeft->setBrakeTorque(m_currentBrakeTorque);
		m_wheelRearRight->setBrakeTorque(m_currentBrakeTorque);
	}
	m_wheelFrontLeft->setMotorTorque(0);
    m_wheelFrontRight->setMotorTorque(0);
	m_wheelRearLeft->setMotorTorque(0);
    m_wheelRearRight->setMotorTorque(0);

	m_wheelFrontLeft->setBrakeTorque(m_vehicleEngine->getBrake());
	m_wheelFrontRight->setBrakeTorque(m_vehicleEngine->getBrake());
	m_wheelRearLeft->setBrakeTorque(m_vehicleEngine->getBrake());
	m_wheelRearRight->setBrakeTorque(m_vehicleEngine->getBrake());

	switch(m_wheelDriveType)
	{
		case WDT_2WD:
			m_wheelRearLeft->setMotorTorque(m_vehicleEngine->getAcceleration() / (NxTwoPi * m_wheelRearLeft->getRadius()));
			m_wheelRearRight->setMotorTorque(m_vehicleEngine->getAcceleration() / (NxTwoPi * m_wheelRearRight->getRadius()));
			break;
		case WDT_4WD:
			m_wheelFrontLeft->setMotorTorque(m_vehicleEngine->getAcceleration() / (NxTwoPi * m_wheelFrontLeft->getRadius()));
			m_wheelFrontRight->setMotorTorque(m_vehicleEngine->getAcceleration() / (NxTwoPi * m_wheelFrontRight->getRadius()));
			m_wheelRearLeft->setMotorTorque(m_vehicleEngine->getAcceleration() / (NxTwoPi * m_wheelRearLeft->getRadius()));
			m_wheelRearRight->setMotorTorque(m_vehicleEngine->getAcceleration() / (NxTwoPi * m_wheelRearRight->getRadius()));
			break;
		case WDT_FWD:
			m_wheelFrontLeft->setMotorTorque(m_vehicleEngine->getAcceleration() / (NxTwoPi * m_wheelFrontLeft->getRadius()));
			m_wheelFrontRight->setMotorTorque(m_vehicleEngine->getAcceleration() / (NxTwoPi * m_wheelFrontRight->getRadius()));
			break;
	}
	m_vehicleEngine->setSpeed(getSpeed());
	/*m_wheelFrontRight->setBrakeTorque(m_currentBrakeTorque);
	m_wheelRearLeft->setBrakeTorque(m_currentBrakeTorque);
	m_wheelRearRight->setBrakeTorque(m_currentBrakeTorque);*/

	/*if(m_objChassis)
	{
		m_objChassis->boundingBox.setTransform(Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z));
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
	}*/
}

float VehicleController::getSteerAngle()
{
	if(!m_steerableWheelsFront)
		return m_currentSteerAngle;
	return -m_currentSteerAngle;
}

Vec3 VehicleController::getForwardVec()
{
	getVehiclePose();
	return m_forward;
}

Vec3 VehicleController::getLeftVec()
{
	getVehiclePose();
	return m_left;
}

Vec3 VehicleController::getFrontJointPoint()
{
	Vec3 retVal;
	D3DXVec3TransformCoord(&retVal, &getVehicleParams()->posTriPodFront, &getVehiclePose()->matChassis);
	return retVal;
}

Vec3 VehicleController::getBackJointPoint()
{
	Vec3 retVal;
	D3DXVec3TransformCoord(&retVal, &getVehicleParams()->posTriPodBack, &getVehiclePose()->matChassis);
	return retVal;
}

VehiclePose* VehicleController::getVehiclePose()
{
	if(m_poseGiven)
		return m_vehiclePose;
	m_poseGiven = true;
	Mat backup;
	Vec3 wheelPos;
	NxWheelContactData dest;
	NxShape* cont;
	float contactPos;
	Vec3 suspensionTravel;
	/*CHASSIS*/
	Mat rot;
	worldMat(&m_vehiclePose->matChassis, Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z), Vec3(0, 0, 0));
	D3DXMatrixRotationQuaternion(&rot, &D3DXQUATERNION(m_actor->getGlobalOrientationQuat().x, m_actor->getGlobalOrientationQuat().y, m_actor->getGlobalOrientationQuat().z, m_actor->getGlobalOrientationQuat().w));

	Vec3 cross;
	Mat chassisRot;
	D3DXVec3Cross(&cross, &Vec3(1, 0, 0), &m_vehicleParams->vecChassisForward);

	D3DXMatrixRotationAxis(&chassisRot, &cross, acos(D3DXVec3Dot(&Vec3(1, 0, 0), &m_vehicleParams->vecChassisForward)));
	m_vehiclePose->matChassis = chassisRot * rot * m_vehiclePose->matChassis;
	D3DXVec3TransformCoord(&m_forward, &Vec3(1, 0, 0), &rot);
	D3DXVec3TransformCoord(&m_left, &Vec3(0, 0, -1), &rot);
	D3DXVec3Normalize(&m_forward, &m_forward);
	D3DXVec3Normalize(&m_left, &m_left);


	ShapeUserData* sud;
	//Wheels

	//Front Left wheel

	
	Mat wheelRot;
	D3DXMatrixRotationYawPitchRoll(&wheelRot, m_vehicleParams->vecFrontLeftWheelRotation.y, m_vehicleParams->vecFrontLeftWheelRotation.x, m_vehicleParams->vecFrontLeftWheelRotation.z);
	sud = (ShapeUserData*)(m_wheelFrontLeft->userData);
	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matFrontLeftWheel);
	m_vehiclePose->matFrontLeftWheel = wheelRot * m_vehiclePose->matFrontLeftWheel;

	wheelPos = getVehicleParams()->posFrontLeftWheel;
	D3DXVec3TransformCoord(&wheelPos, &wheelPos, &m_vehiclePose->matChassis);
	cont = m_wheelFrontLeft->getContact(dest);
	backup = m_vehiclePose->matChassis;
	backup._41 = 0;
	backup._42 = 0;
	backup._43 = 0;
	contactPos = dest.contactPosition;
	if(!cont)
		contactPos = m_wheelFrontLeft->getSuspensionTravel();
	else
		contactPos -= m_wheelFrontLeft->getRadius();
	suspensionTravel = Vec3(0, contactPos, 0);
	D3DXVec3TransformCoord(&suspensionTravel, &suspensionTravel, &backup);
	//if(dest.contactForce)
		wheelPos -= suspensionTravel;

	m_vehiclePose->matFrontLeftWheel._41 = wheelPos.x;
	m_vehiclePose->matFrontLeftWheel._42 = wheelPos.y;
	m_vehiclePose->matFrontLeftWheel._43 = wheelPos.z;

	//Front Right wheel

	D3DXMatrixRotationYawPitchRoll(&wheelRot, m_vehicleParams->vecFrontRightWheelRotation.y, m_vehicleParams->vecFrontRightWheelRotation.x, m_vehicleParams->vecFrontRightWheelRotation.z);
	sud = (ShapeUserData*)(m_wheelFrontRight->userData);
	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matFrontRightWheel);
	m_vehiclePose->matFrontRightWheel = wheelRot * m_vehiclePose->matFrontRightWheel;

	wheelPos = getVehicleParams()->posFrontRightWheel;
	D3DXVec3TransformCoord(&wheelPos, &wheelPos, &m_vehiclePose->matChassis);
	cont = m_wheelFrontRight->getContact(dest);
	backup = m_vehiclePose->matChassis;
	backup._41 = 0;
	backup._42 = 0;
	backup._43 = 0;
	contactPos = dest.contactPosition;
	if(!cont)
		contactPos = m_wheelFrontRight->getSuspensionTravel();
	else
		contactPos -= m_wheelFrontRight->getRadius();
	suspensionTravel = Vec3(0, contactPos, 0);
	D3DXVec3TransformCoord(&suspensionTravel, &suspensionTravel, &backup);
	//if(dest.contactForce)
		wheelPos -= suspensionTravel;

	m_vehiclePose->matFrontRightWheel._41 = wheelPos.x;
	m_vehiclePose->matFrontRightWheel._42 = wheelPos.y;
	m_vehiclePose->matFrontRightWheel._43 = wheelPos.z;

	//Rear Left wheel
	
	D3DXMatrixRotationYawPitchRoll(&wheelRot, m_vehicleParams->vecRearLeftWheelRotation.y, m_vehicleParams->vecRearLeftWheelRotation.x, m_vehicleParams->vecRearLeftWheelRotation.z);
	sud = (ShapeUserData*)(m_wheelRearLeft->userData);
	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matRearLeftWheel);
	m_vehiclePose->matRearLeftWheel = wheelRot * m_vehiclePose->matRearLeftWheel;

	wheelPos = getVehicleParams()->posRearLeftWheel;
	D3DXVec3TransformCoord(&wheelPos, &wheelPos, &m_vehiclePose->matChassis);
	cont = m_wheelRearLeft->getContact(dest);
	backup = m_vehiclePose->matChassis;
	backup._41 = 0;
	backup._42 = 0;
	backup._43 = 0;
	contactPos = dest.contactPosition;
	if(!cont)
		contactPos = m_wheelRearLeft->getSuspensionTravel();
	else
		contactPos -= m_wheelRearLeft->getRadius();
	suspensionTravel = Vec3(0, contactPos, 0);
	D3DXVec3TransformCoord(&suspensionTravel, &suspensionTravel, &backup);
	//if(dest.contactForce)
		wheelPos -= suspensionTravel;

	m_vehiclePose->matRearLeftWheel._41 = wheelPos.x;
	m_vehiclePose->matRearLeftWheel._42 = wheelPos.y;
	m_vehiclePose->matRearLeftWheel._43 = wheelPos.z;

	//Rear Right wheel
	
	D3DXMatrixRotationYawPitchRoll(&wheelRot, m_vehicleParams->vecRearRightWheelRotation.y, m_vehicleParams->vecRearRightWheelRotation.x, m_vehicleParams->vecRearRightWheelRotation.z);
	sud = (ShapeUserData*)(m_wheelRearRight->userData);
	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matRearRightWheel);
	m_vehiclePose->matRearRightWheel = wheelRot * m_vehiclePose->matRearRightWheel;

	wheelPos = getVehicleParams()->posRearRightWheel;
	D3DXVec3TransformCoord(&wheelPos, &wheelPos, &m_vehiclePose->matChassis);
	cont = m_wheelRearRight->getContact(dest);
	backup = m_vehiclePose->matChassis;
	backup._41 = 0;
	backup._42 = 0;
	backup._43 = 0;
	contactPos = dest.contactPosition;
	if(!cont)
		contactPos = m_wheelRearRight->getSuspensionTravel();
	else
		contactPos -= m_wheelRearRight->getRadius();
	suspensionTravel = Vec3(0, contactPos, 0);
	D3DXVec3TransformCoord(&suspensionTravel, &suspensionTravel, &backup);
	//if(dest.contactForce)
		wheelPos -= suspensionTravel;

	m_vehiclePose->matRearRightWheel._41 = wheelPos.x;
	m_vehiclePose->matRearRightWheel._42 = wheelPos.y;
	m_vehiclePose->matRearRightWheel._43 = wheelPos.z;

	return m_vehiclePose;
}

float VehicleController::getSpeed()
{
	return m_wheelRearLeft->getAxleSpeed()/m_wheelRearLeft->getRadius(), m_wheelRearLeft->getAxleSpeed() * (NxTwoPi * m_wheelRearLeft->getRadius());
}

VehicleParams* VehicleController::getVehicleParams()
{
	return m_vehicleParams;
}

VehicleParamsEx* VehicleController::getVehicleParamsEx()
{
	return m_vehicleParamsEx;
}

VehicleEngine* VehicleController::getVehicleEngine()
{
	return m_vehicleEngine;
}

void VehicleController::resetPose()
{
	NxQuat quat;
	quat.x = 0;
	quat.y = 1;
	quat.z = 0;
	quat.w = 0;
	NxMat33 rot(quat);
	m_actor->setGlobalPose(NxMat34(rot, m_actor->getGlobalPosition() + NxVec3(0, 5, 0)));
}
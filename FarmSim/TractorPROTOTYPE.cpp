#include "Tractor.h"

Tractor::Tractor(Dynamics *dyn, NxVec3 pos, string fname)
{
	m_vehicleDimms = new VehicleDimms;
	//VehicleDimms *vd = new VehicleDimms;
	VehicleParamsEx *vp = new VehicleParamsEx;
	vector<CfgVariable*> vars;
	vars = *m_vehicleDimms->getVariableList();
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

	vars.push_back(new CfgVariable(V_VEC3, &rotateFrontLeftWheel, getVarName(rotateFrontLeftWheel)));
	vars.push_back(new CfgVariable(V_VEC3, &rotateFrontRightWheel, getVarName(rotateFrontRightWheel)));
	vars.push_back(new CfgVariable(V_VEC3, &rotateRearLeftWheel, getVarName(rotateRearLeftWheel)));
	vars.push_back(new CfgVariable(V_VEC3, &rotateRearRightWheel, getVarName(rotateRearRightWheel)));
	vars.push_back(new CfgVariable(V_VEC3, &rotateChassis, getVarName(rotateChassis)));

	vars.push_back(new CfgVariable(V_STRING, &paramsType, getVarName(paramsType)));

	vars.push_back(new CfgVariable(V_STRING, &wheelFrontLeftModel, getVarName(wheelFrontLeftModel)));
	vars.push_back(new CfgVariable(V_STRING, &wheelFrontRightModel, getVarName(wheelFrontRightModel)));
	vars.push_back(new CfgVariable(V_STRING, &wheelRearLeftModel, getVarName(wheelRearLeftModel)));
	vars.push_back(new CfgVariable(V_STRING, &wheelRearRightModel, getVarName(wheelRearRightModel)));
	vars.push_back(new CfgVariable(V_STRING, &chassisModel, getVarName(chassisModel)));

	CfgLoader cfg(fname, &vars);

	D3DXMatrixRotationYawPitchRoll(&m_vehicleDimms->matFrontLeftWheelRotation, rotateFrontLeftWheel.y, rotateFrontLeftWheel.x, rotateFrontLeftWheel.z);
	D3DXMatrixRotationYawPitchRoll(&m_vehicleDimms->matFrontRightWheelRotation, rotateFrontRightWheel.y, rotateFrontRightWheel.x, rotateFrontRightWheel.z);
	D3DXMatrixRotationYawPitchRoll(&m_vehicleDimms->matRearLeftWheelRotation, rotateRearLeftWheel.y, rotateRearLeftWheel.x, rotateRearLeftWheel.z);
	D3DXMatrixRotationYawPitchRoll(&m_vehicleDimms->matRearRightWheelRotation, rotateRearRightWheel.y, rotateRearRightWheel.x, rotateRearRightWheel.z);


	D3DXMatrixRotationYawPitchRoll(&m_vehicleDimms->matChassisRotation, rotateChassis.y, rotateChassis.x, rotateChassis.z);

	m_objChassis = new RendObj(chassisModel, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
	m_objFrontLeftWheel = new RendObj(wheelFrontLeftModel, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
	m_objFrontRightWheel = new RendObj(wheelFrontRightModel, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
	m_objRearLeftWheel = new RendObj(wheelRearLeftModel, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
	m_objRearRightWheel = new RendObj(wheelRearRightModel, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));

	core.game->getWorld()->addToWorld(m_objChassis, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
	core.game->getWorld()->addToWorld(m_objFrontLeftWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
	core.game->getWorld()->addToWorld(m_objFrontRightWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
	core.game->getWorld()->addToWorld(m_objRearLeftWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
	core.game->getWorld()->addToWorld(m_objRearRightWheel, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);

	m_vehicleDimms->chassisDimm = Vec3(m_objChassis->getPose()->boundingBox.getWidth()/2, m_objChassis->getPose()->boundingBox.getHeight()/6, m_objChassis->getPose()->boundingBox.getDepth()/2);

	stopMotorAndBreak();

	m_jointPoint = m_vehicleDimms->jointPoint;
	m_joinedTrailer = NULL;
	m_joint = NULL;

	m_oldPos = Vec3(0, 0, 0);
	m_forward = Vec3(1, 0, 0);
	m_poseGiven = false;
	m_scene = dyn->getScene();
	m_setCurrentSteerAngle = false;
	m_setCurrentMotorTorque = false;
	m_setCurrentBrakeTorque = false;
	m_currentSteerAngle = 0;
	m_currentMotorTorque = 0;
	m_currentBrakeTorque = 0;
	m_vehiclePose = new VehiclePose;

	m_joint = NULL;
	m_joinedTrailer = NULL;

	D3DXMatrixIdentity(&m_vehiclePose->matChassis);
	D3DXMatrixIdentity(&m_vehiclePose->matFrontLeftWheel);
	D3DXMatrixIdentity(&m_vehiclePose->matFrontRightWheel);
	D3DXMatrixIdentity(&m_vehiclePose->matRearLeftWheel);
	D3DXMatrixIdentity(&m_vehiclePose->matRearRightWheel);

	//m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(2,0.25,1), 10);
	m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(m_vehicleDimms->chassisDimm), m_vehicleDimms->chassisDensity);
	SetActorCollisionGroup(m_actor, GROUP_COLLIDABLE_NON_PUSHABLE);
	//m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(2,0.25,1), dimm->chassisDensity);

// Wheel material globals
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
	wheelDesc.wheelRadius = m_vehicleDimms->frontWheelsRadius;
	wheelDesc.wheelWidth = m_vehicleDimms->frontWheelsWidth;  // 0.1
	wheelDesc.wheelSuspension = 10;  
	wheelDesc.springRestitution = 0.1;
	wheelDesc.springDamping = 10;
	wheelDesc.springBias = 10;  
	wheelDesc.maxBrakeForce = 0.01;
	wheelDesc.position = NxVec3(m_vehicleDimms->posFrontLeftWheel);
	wheelDesc.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
						   NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;

	// Front right wheel
	NxWheelDesc wheelDesc2;
	wheelDesc2.frictionToFront = 100.0f;
	wheelDesc2.frictionToSide = 1.0f;



	wheelDesc2.wheelApproximation = 0;
	wheelDesc2.wheelRadius = m_vehicleDimms->frontWheelsRadius;
	wheelDesc2.wheelWidth = m_vehicleDimms->frontWheelsWidth;  // 0.1
	wheelDesc2.wheelSuspension = 10;  
	wheelDesc2.springRestitution = 0.1;
	wheelDesc2.springDamping = 10;
	wheelDesc2.springBias = 10;  
	wheelDesc.maxBrakeForce = 0.01;
	wheelDesc2.position = NxVec3(m_vehicleDimms->posFrontRightWheel);
	wheelDesc2.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;

	// Rear left wheel
	NxWheelDesc wheelDesc3;
	wheelDesc3.frictionToFront = 100.0f;
	wheelDesc3.frictionToSide = 1.0f;



	wheelDesc3.wheelApproximation = 0;
	wheelDesc3.wheelRadius = m_vehicleDimms->rearWheelsRadius;
	wheelDesc3.wheelWidth = m_vehicleDimms->rearWheelsWidth;  // 0.1
	wheelDesc3.wheelSuspension = 10;  
	wheelDesc3.springRestitution = 0.1;
	wheelDesc3.springDamping = 10;
	wheelDesc3.springBias = 10;  
	wheelDesc3.maxBrakeForce = 0.01;
	wheelDesc3.position = NxVec3(m_vehicleDimms->posRearLeftWheel);
	wheelDesc3.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;

	// Rear right wheel
	NxWheelDesc wheelDesc4;
	wheelDesc4.frictionToFront = 100.0f;
	wheelDesc4.frictionToSide = 1.0f;


	wheelDesc4.wheelApproximation = 0;
	wheelDesc4.wheelRadius = m_vehicleDimms->rearWheelsRadius;
	wheelDesc4.wheelWidth = m_vehicleDimms->rearWheelsWidth;  // 0.1
	wheelDesc4.wheelSuspension = 10;  
	wheelDesc4.springRestitution = 0.1;
	wheelDesc4.springDamping = 10;
	wheelDesc4.springBias = 10;  
	wheelDesc4.maxBrakeForce = 0.01;
	wheelDesc4.position = NxVec3(m_vehicleDimms->posRearRightWheel);
	wheelDesc4.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;


	wheelDesc4.frictionToFront = wheelDesc3.frictionToFront = wheelDesc2.frictionToFront = wheelDesc.frictionToFront = 100.0f;
	wheelDesc4.frictionToSide = wheelDesc3.frictionToSide = wheelDesc2.frictionToSide = wheelDesc.frictionToSide = 1.0f;


	/*wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.0001;
	wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 0.0001;
	wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.001;
	wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 0.001;
	wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01;*/

	if(paramsType != "Extended")
	{
		wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.5f / 10;
		wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 1000.0f / 100000000000;
		wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.5f / 10000000000000;
		wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 1.0f / 10000000000000;
		wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01f; // 100;
	}
	else
	{
		wheelDesc4.frictionToFront = wheelDesc3.frictionToFront = wheelDesc2.frictionToFront = wheelDesc.frictionToFront = vp->frictionToFront;
		wheelDesc4.frictionToSide = wheelDesc3.frictionToSide = wheelDesc2.frictionToSide = wheelDesc.frictionToSide = vp->frictionToSide;

		wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = vp->wheelSuspension;
		wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = vp->springRestitution;
		wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = vp->springDamping;
		wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = vp->springBias;
		wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = vp->maxBrakeForce; // 100;
	}

	m_wheelFrontLeft = AddWheelToActor(m_actor, &wheelDesc);
	m_wheelFrontRight = AddWheelToActor(m_actor, &wheelDesc2);
	m_wheelRearLeft = AddWheelToActor(m_actor, &wheelDesc3);
	m_wheelRearRight = AddWheelToActor(m_actor, &wheelDesc4);

	m_wheelFrontLeft->userData = (void*)new ShapeUserData;
	m_wheelFrontRight->userData = (void*)new ShapeUserData;
	m_wheelRearLeft->userData = (void*)new ShapeUserData;
	m_wheelRearRight->userData = (void*)new ShapeUserData;

	// LOWER THE CENTER OF MASS
	NxVec3 massPos = m_actor->getCMassLocalPosition();
	massPos.y -= 1;
	m_actor->setCMassOffsetLocalPosition(massPos);
	m_actor->wakeUp(1e30);
}
//
//Tractor::Tractor(Dynamics *dyn, NxVec3 pos, VehicleDimms *dimm)
//{
//	m_objChassis = NULL;
//	m_objFrontLeftWheel = NULL;
//	m_objFrontRightWheel = NULL;
//	m_objRearLeftWheel = NULL;
//	m_objRearRightWheel = NULL;
//
//	m_jointPoint = dimm->jointPoint;
//	m_joinedTrailer = NULL;
//	m_joint = NULL;
//
//	m_oldPos = Vec3(0, 0, 0);
//	m_forward = Vec3(1, 0, 0);
//	m_poseGiven = false;
//	m_scene = dyn->getScene();
//	m_setCurrentSteerAngle = false;
//	m_setCurrentMotorTorque = false;
//	m_setCurrentBrakeTorque = false;
//	m_currentSteerAngle = 0;
//	m_currentMotorTorque = 0;
//	m_currentBrakeTorque = 0;
//	m_vehiclePose = new VehiclePose;
//	m_vehicleDimms = dimm;
//
//	m_joint = NULL;
//	m_joinedTrailer = NULL;
//
//	D3DXMatrixIdentity(&m_vehiclePose->matChassis);
//	D3DXMatrixIdentity(&m_vehiclePose->matFrontLeftWheel);
//	D3DXMatrixIdentity(&m_vehiclePose->matFrontRightWheel);
//	D3DXMatrixIdentity(&m_vehiclePose->matRearLeftWheel);
//	D3DXMatrixIdentity(&m_vehiclePose->matRearRightWheel);
//
//	//m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(2,0.25,1), 10);
//	m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(dimm->chassisDimm), dimm->chassisDensity);
//	SetActorCollisionGroup(m_actor, GROUP_COLLIDABLE_NON_PUSHABLE);
//	//m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(2,0.25,1), dimm->chassisDensity);
//
//// Wheel material globals
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
//	wheelDesc.wheelRadius = dimm->frontWheelsRadius;
//	wheelDesc.wheelWidth = dimm->frontWheelsWidth;  // 0.1
//	wheelDesc.wheelSuspension = 10;  
//	wheelDesc.springRestitution = 0.1;
//	wheelDesc.springDamping = 10;
//	wheelDesc.springBias = 10;  
//	wheelDesc.maxBrakeForce = 0.01;
//	wheelDesc.position = NxVec3(dimm->posFrontLeftWheel);
//	wheelDesc.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//						   NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
//
//	// Front right wheel
//	NxWheelDesc wheelDesc2;
//	wheelDesc2.frictionToFront = 100.0f;
//	wheelDesc2.frictionToSide = 1.0f;
//
//
//
//	wheelDesc2.wheelApproximation = 0;
//	wheelDesc2.wheelRadius = dimm->frontWheelsRadius;
//	wheelDesc2.wheelWidth = dimm->frontWheelsWidth;  // 0.1
//	wheelDesc2.wheelSuspension = 10;  
//	wheelDesc2.springRestitution = 0.1;
//	wheelDesc2.springDamping = 10;
//	wheelDesc2.springBias = 10;  
//	wheelDesc.maxBrakeForce = 0.01;
//	wheelDesc2.position = NxVec3(dimm->posFrontRightWheel);
//	wheelDesc2.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
//
//	// Rear left wheel
//	NxWheelDesc wheelDesc3;
//	wheelDesc3.frictionToFront = 100.0f;
//	wheelDesc3.frictionToSide = 1.0f;
//
//
//
//	wheelDesc3.wheelApproximation = 0;
//	wheelDesc3.wheelRadius = dimm->rearWheelsRadius;
//	wheelDesc3.wheelWidth = dimm->rearWheelsWidth;  // 0.1
//	wheelDesc3.wheelSuspension = 10;  
//	wheelDesc3.springRestitution = 0.1;
//	wheelDesc3.springDamping = 10;
//	wheelDesc3.springBias = 10;  
//	wheelDesc3.maxBrakeForce = 0.01;
//	wheelDesc3.position = NxVec3(dimm->posRearLeftWheel);
//	wheelDesc3.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
//
//	// Rear right wheel
//	NxWheelDesc wheelDesc4;
//	wheelDesc4.frictionToFront = 100.0f;
//	wheelDesc4.frictionToSide = 1.0f;
//
//
//	wheelDesc4.wheelApproximation = 0;
//	wheelDesc4.wheelRadius = dimm->rearWheelsRadius;
//	wheelDesc4.wheelWidth = dimm->rearWheelsWidth;  // 0.1
//	wheelDesc4.wheelSuspension = 10;  
//	wheelDesc4.springRestitution = 0.1;
//	wheelDesc4.springDamping = 10;
//	wheelDesc4.springBias = 10;  
//	wheelDesc4.maxBrakeForce = 0.01;
//	wheelDesc4.position = NxVec3(dimm->posRearRightWheel);
//	wheelDesc4.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
//
//
//	wheelDesc4.frictionToFront = wheelDesc3.frictionToFront = wheelDesc2.frictionToFront = wheelDesc.frictionToFront = 100.0f;
//	wheelDesc4.frictionToSide = wheelDesc3.frictionToSide = wheelDesc2.frictionToSide = wheelDesc.frictionToSide = 1.0f;
//
//
//	/*wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.0001;
//	wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 0.0001;
//	wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.001;
//	wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 0.001;
//	wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01;*/
//	wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.5f / 10;
//	wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 1000.0f / 100000000000;
//	wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.5f / 10000000000000;
//	wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 1.0f / 10000000000000;
//	wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01f; // 100;
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
//	massPos.y -= 1;
//	m_actor->setCMassOffsetLocalPosition(massPos);
//	m_actor->wakeUp(1e30);
//}
//
//Tractor::Tractor(Dynamics *dyn, NxVec3 pos, VehicleDimms *dimm, VehicleParamsEx *paramsEx)
//{
//	m_objChassis = NULL;
//	m_objFrontLeftWheel = NULL;
//	m_objFrontRightWheel = NULL;
//	m_objRearLeftWheel = NULL;
//	m_objRearRightWheel = NULL;
//
//	m_jointPoint = dimm->jointPoint;
//	m_joinedTrailer = NULL;
//	m_joint = NULL;
//
//	m_oldPos = Vec3(0, 0, 0);
//	m_forward = Vec3(1, 0, 0);
//	m_poseGiven = false;
//	m_scene = dyn->getScene();
//	m_setCurrentSteerAngle = false;
//	m_setCurrentMotorTorque = false;
//	m_setCurrentBrakeTorque = false;
//	m_currentSteerAngle = 0;
//	m_currentMotorTorque = 0;
//	m_currentBrakeTorque = 0;
//	m_vehiclePose = new VehiclePose;
//	m_vehicleDimms = dimm;
//
//	m_joint = NULL;
//	m_joinedTrailer = NULL;
//
//	D3DXMatrixIdentity(&m_vehiclePose->matChassis);
//	D3DXMatrixIdentity(&m_vehiclePose->matFrontLeftWheel);
//	D3DXMatrixIdentity(&m_vehiclePose->matFrontRightWheel);
//	D3DXMatrixIdentity(&m_vehiclePose->matRearLeftWheel);
//	D3DXMatrixIdentity(&m_vehiclePose->matRearRightWheel);
//
//	//m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(2,0.25,1), 10);
//	m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(dimm->chassisDimm), dimm->chassisDensity);
//	SetActorCollisionGroup(m_actor, GROUP_COLLIDABLE_NON_PUSHABLE);
//	//m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(2,0.25,1), dimm->chassisDensity);
//
//// Wheel material globals
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
//	wheelDesc.wheelRadius = dimm->frontWheelsRadius;
//	wheelDesc.wheelWidth = dimm->frontWheelsWidth;  // 0.1
//	wheelDesc.wheelSuspension = 10;  
//	wheelDesc.springRestitution = 0.1;
//	wheelDesc.springDamping = 10;
//	wheelDesc.springBias = 10;  
//	wheelDesc.maxBrakeForce = 0.01;
//	wheelDesc.position = NxVec3(dimm->posFrontLeftWheel);
//	wheelDesc.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//						   NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
//
//	// Front right wheel
//	NxWheelDesc wheelDesc2;
//	wheelDesc2.frictionToFront = 100.0f;
//	wheelDesc2.frictionToSide = 1.0f;
//
//
//
//	wheelDesc2.wheelApproximation = 0;
//	wheelDesc2.wheelRadius = dimm->frontWheelsRadius;
//	wheelDesc2.wheelWidth = dimm->frontWheelsWidth;  // 0.1
//	wheelDesc2.wheelSuspension = 10;  
//	wheelDesc2.springRestitution = 0.1;
//	wheelDesc2.springDamping = 10;
//	wheelDesc2.springBias = 10;  
//	wheelDesc.maxBrakeForce = 0.01;
//	wheelDesc2.position = NxVec3(dimm->posFrontRightWheel);
//	wheelDesc2.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
//
//	// Rear left wheel
//	NxWheelDesc wheelDesc3;
//	wheelDesc3.frictionToFront = 100.0f;
//	wheelDesc3.frictionToSide = 1.0f;
//
//
//
//	wheelDesc3.wheelApproximation = 0;
//	wheelDesc3.wheelRadius = dimm->rearWheelsRadius;
//	wheelDesc3.wheelWidth = dimm->rearWheelsWidth;  // 0.1
//	wheelDesc3.wheelSuspension = 10;  
//	wheelDesc3.springRestitution = 0.1;
//	wheelDesc3.springDamping = 10;
//	wheelDesc3.springBias = 10;  
//	wheelDesc3.maxBrakeForce = 0.01;
//	wheelDesc3.position = NxVec3(dimm->posRearLeftWheel);
//	wheelDesc3.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
//
//	// Rear right wheel
//	NxWheelDesc wheelDesc4;
//	wheelDesc4.frictionToFront = 100.0f;
//	wheelDesc4.frictionToSide = 1.0f;
//
//
//	wheelDesc4.wheelApproximation = 0;
//	wheelDesc4.wheelRadius = dimm->rearWheelsRadius;
//	wheelDesc4.wheelWidth = dimm->rearWheelsWidth;  // 0.1
//	wheelDesc4.wheelSuspension = 10;  
//	wheelDesc4.springRestitution = 0.1;
//	wheelDesc4.springDamping = 10;
//	wheelDesc4.springBias = 10;  
//	wheelDesc4.maxBrakeForce = 0.01;
//	wheelDesc4.position = NxVec3(dimm->posRearRightWheel);
//	wheelDesc4.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
//							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
//
//
//	wheelDesc4.frictionToFront = wheelDesc3.frictionToFront = wheelDesc2.frictionToFront = wheelDesc.frictionToFront = paramsEx->frictionToFront;
//	wheelDesc4.frictionToSide = wheelDesc3.frictionToSide = wheelDesc2.frictionToSide = wheelDesc.frictionToSide = paramsEx->frictionToSide;
//
//	wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = paramsEx->wheelSuspension;
//	wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = paramsEx->springRestitution;
//	wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = paramsEx->springDamping;
//	wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = paramsEx->springBias;
//	wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = paramsEx->maxBrakeForce; // 100;
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
//	NxVec3 massPos;
//	if(paramsEx->massCenter != Vec3(0, 0, 0))
//		massPos = NxVec3(paramsEx->massCenter);
//	else
//	{
//		massPos = m_actor->getCMassLocalPosition();
//		massPos.y -= 1;
//	}
//	m_actor->setCMassOffsetLocalPosition(massPos);
//	m_actor->wakeUp(1e30);
//	stopMotorAndBreak();
//}

Tractor::~Tractor()
{
}

NxActor* Tractor::getActor()
{
	return m_actor;
}

void Tractor::update()
{
	m_poseGiven = false;
	VehiclePose *vehiclePose = (VehiclePose*)getVehiclePose();
	if (m_setCurrentSteerAngle)
	{
		m_setCurrentSteerAngle = false;
        m_wheelFrontLeft->setSteerAngle(m_currentSteerAngle);
        m_wheelFrontRight->setSteerAngle(m_currentSteerAngle);
		gEngine.renderer->addTxt("Front Wheel Steer Angle: %d", (int)(m_currentSteerAngle*180.0/NxPi));
	}

	if (m_setCurrentMotorTorque)
	{
		m_setCurrentMotorTorque = false;
        m_wheelFrontLeft->setMotorTorque(m_currentMotorTorque);
        m_wheelFrontRight->setMotorTorque(m_currentMotorTorque);
		m_wheelRearLeft->setMotorTorque(m_currentMotorTorque);
        m_wheelRearRight->setMotorTorque(m_currentMotorTorque);

		gEngine.renderer->addTxt("Rear Wheel Motor Torque: %f", m_currentMotorTorque);
	}

	if (m_setCurrentBrakeTorque)
	{
		m_setCurrentBrakeTorque = false;
		m_wheelFrontLeft->setBrakeTorque(m_currentBrakeTorque);
		m_wheelFrontRight->setBrakeTorque(m_currentBrakeTorque);
		m_wheelRearLeft->setBrakeTorque(m_currentBrakeTorque);
		m_wheelRearRight->setBrakeTorque(m_currentBrakeTorque);
		gEngine.renderer->addTxt("Rear Wheel Brake Torque: %d", m_currentBrakeTorque);
	}

	if(m_objChassis)
	{
		m_objChassis->getPose()->boundingBox.setTransform(Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z));
		m_objChassis->getPose()->world = vehiclePose->matChassis;
		Vec3 min = Vec3(-2, -2, -2);
		Vec3 max = Vec3(2, 2, 2);
		Vec3 min2 = min;
		min2.x *= -1;
		Vec3 max2 = max;
		max2.x *= -1;
		Mat rot;
		//D3DXMatrixRotationY(&rot, -acos(D3DXVec3Dot(&Vec3(1, 0, 0), &getForward())));
		/*D3DXVec3TransformCoord(&min, &min, &rot);
		D3DXVec3TransformCoord(&max, &max, &rot);
		D3DXVec3TransformCoord(&min2, &min2, &rot);
		D3DXVec3TransformCoord(&max2, &max2, &rot);*/
		D3DXVec3TransformCoord(&min, &min, &vehiclePose->matChassis);
		D3DXVec3TransformCoord(&max, &max, &vehiclePose->matChassis);
		D3DXVec3TransformCoord(&min2, &min2, &vehiclePose->matChassis);
		D3DXVec3TransformCoord(&max2, &max2, &vehiclePose->matChassis);
		Vec3 center = m_objChassis->getPose()->boundingBox.getCenter();
		/*min += center;
		max += center;
		min2 += center;
		max2 += center;*/
		/*D3DXVec3TransformCoord(&min, &m_objChassis->getPose()->boundingBox.Min, &vehiclePose->matChassis);
		D3DXVec3TransformCoord(&max, &m_objChassis->getPose()->boundingBox.Max, &vehiclePose->matChassis);*/
		core.game->getWorld()->getGrassManager()->changeTerrain(&center, 2.0f);
		m_objChassis->getPose()->position = Vec3(vehiclePose->matChassis._41, vehiclePose->matChassis._42, vehiclePose->matChassis._43);
		boxMin->getPose()->position = min2;
		boxMax->getPose()->position = max2;
		worldMat(&boxMin->getPose()->world, boxMin->getPose()->position, Vec3(0, 0, 0));
		worldMat(&boxMax->getPose()->world, boxMax->getPose()->position, Vec3(0, 0, 0));
		boxMin->getPose()->boundingBox.setTransform(min2);
		boxMax->getPose()->boundingBox.setTransform(max2);
	}

	if(m_objFrontLeftWheel)
	{
		m_objFrontLeftWheel->getPose()->boundingBox.setTransform(Vec3(vehiclePose->matFrontLeftWheel(3, 0), vehiclePose->matFrontLeftWheel(3, 1), vehiclePose->matFrontLeftWheel(3, 2)));
		m_objFrontLeftWheel->getPose()->world = vehiclePose->matFrontLeftWheel;
		m_objFrontLeftWheel->getPose()->position = Vec3(vehiclePose->matFrontLeftWheel._41, vehiclePose->matFrontLeftWheel._42, vehiclePose->matFrontLeftWheel._43);
	}

	if(m_objFrontRightWheel)
	{
		m_objFrontRightWheel->getPose()->boundingBox.setTransform(Vec3(vehiclePose->matFrontRightWheel(3, 0), vehiclePose->matFrontRightWheel(3, 1), vehiclePose->matFrontRightWheel(3, 2)));
		m_objFrontRightWheel->getPose()->world = vehiclePose->matFrontRightWheel;
		m_objFrontRightWheel->getPose()->position = Vec3(vehiclePose->matFrontRightWheel._41, vehiclePose->matFrontRightWheel._42, vehiclePose->matFrontRightWheel._43);
	}

	if(m_objRearLeftWheel)
	{
		m_objRearLeftWheel->getPose()->boundingBox.setTransform(Vec3(vehiclePose->matRearLeftWheel(3, 0), vehiclePose->matRearLeftWheel(3, 1), vehiclePose->matRearLeftWheel(3, 2)));
		m_objRearLeftWheel->getPose()->world = vehiclePose->matRearLeftWheel;
		m_objRearLeftWheel->getPose()->position = Vec3(vehiclePose->matRearLeftWheel._41, vehiclePose->matRearLeftWheel._42, vehiclePose->matRearLeftWheel._43);
	}

	if(m_objRearRightWheel)
	{
		m_objRearRightWheel->getPose()->boundingBox.setTransform(Vec3(vehiclePose->matRearRightWheel(3, 0), vehiclePose->matRearRightWheel(3, 1), vehiclePose->matRearRightWheel(3, 2)));
		m_objRearRightWheel->getPose()->world = vehiclePose->matRearRightWheel;
		m_objRearRightWheel->getPose()->position = Vec3(vehiclePose->matRearRightWheel._41, vehiclePose->matRearRightWheel._42, vehiclePose->matRearRightWheel._43);
	}
}

void Tractor::steerAngle(float angle)
{
	m_currentSteerAngle += angle;
	if(m_currentSteerAngle >= m_vehicleDimms->maxSteerAngle)
		m_currentSteerAngle = m_vehicleDimms->maxSteerAngle;
	if(m_currentSteerAngle <= -m_vehicleDimms->maxSteerAngle)
		m_currentSteerAngle = -m_vehicleDimms->maxSteerAngle;
	m_setCurrentSteerAngle = true;
}
void Tractor::increaseMotorTorque(float torque)
{
	m_currentBrakeTorque = 0;
	m_currentMotorTorque += torque;
	m_setCurrentMotorTorque = true;
	m_setCurrentBrakeTorque = true;
}
void Tractor::increaseBreakTorque(float torque)
{
	m_currentBrakeTorque += torque;
	m_setCurrentBrakeTorque = true;
}
void Tractor::stopMotorAndBreak()
{
	m_currentBrakeTorque = 1;
	m_currentMotorTorque = 0;
	m_setCurrentMotorTorque = true;
	m_setCurrentBrakeTorque = true;
}

void Tractor::stopMotor()
{
	m_currentMotorTorque = 0;
	m_setCurrentMotorTorque = true;
}

void Tractor::adjustSteerAngle()
{
	m_currentSteerAngle = 0;
	m_setCurrentSteerAngle = true;
}

void* Tractor::getVehiclePose()
{
	if(m_poseGiven)
		return (void*)m_vehiclePose;
	m_poseGiven = true;
	/*CHASSIS*/
	Mat rot;
	worldMat(&m_vehiclePose->matChassis, Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z), Vec3(0, 0, 0));
	D3DXMatrixRotationQuaternion(&rot, &D3DXQUATERNION(m_actor->getGlobalOrientationQuat().x, m_actor->getGlobalOrientationQuat().y, m_actor->getGlobalOrientationQuat().z, m_actor->getGlobalOrientationQuat().w));
	m_vehiclePose->matChassis = m_vehicleDimms->matChassisRotation * rot * m_vehiclePose->matChassis;
	D3DXVec3TransformNormal(&m_forward, &Vec3(1, 0, 0), &rot);

	ShapeUserData* sud;
	//Wheels

	//Front Left wheel

	sud = (ShapeUserData*)(m_wheelFrontLeft->userData);
	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matFrontLeftWheel);
	m_vehiclePose->matFrontLeftWheel = m_vehicleDimms->matFrontLeftWheelRotation * m_vehiclePose->matFrontLeftWheel;

	//Front Right wheel

	sud = (ShapeUserData*)(m_wheelFrontRight->userData);
	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matFrontRightWheel);
	m_vehiclePose->matFrontRightWheel = m_vehicleDimms->matFrontRightWheelRotation * m_vehiclePose->matFrontRightWheel;

	//Rear Left wheel
	
	sud = (ShapeUserData*)(m_wheelRearLeft->userData);
	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matRearLeftWheel);
	m_vehiclePose->matRearLeftWheel = m_vehicleDimms->matRearLeftWheelRotation * m_vehiclePose->matRearLeftWheel;

	//Rear Right wheel
	
	sud = (ShapeUserData*)(m_wheelRearRight->userData);
	sud->wheelShapePose.getColumnMajor44((NxF32*)&m_vehiclePose->matRearRightWheel);
	m_vehiclePose->matRearRightWheel = m_vehicleDimms->matRearRightWheelRotation * m_vehiclePose->matRearRightWheel;

	return (void*)m_vehiclePose;
}

int Tractor::getSpeed()
{
	Vec3 Way = Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z) - m_oldPos;
	m_oldPos = Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z);
	return D3DXVec3Length(&Way) / core.dynamics->getDeltaTime();
}

void Tractor::attachRendObj(RendObj *chassis, RendObj *frontLeftWheel, RendObj *frontRightWheel, RendObj *rearLeftWheel, RendObj *rearRightWheel)
{
	m_objChassis = chassis;
	m_objFrontLeftWheel = frontLeftWheel;
	m_objFrontRightWheel = frontRightWheel;
	m_objRearLeftWheel = rearLeftWheel;
	m_objRearRightWheel = rearRightWheel;
}

Vec3 Tractor::getJointPoint()
{
	Vec3 retVal;
	D3DXVec3TransformCoord(&retVal, &m_jointPoint, &((VehiclePose*)getVehiclePose())->matChassis);
	return retVal;
}

void Tractor::joinTrailer(ITrailer* trailer)
{
	if(m_joinedTrailer && m_joint)
		return;
	NxD6JointDesc joint;
	Vec3 move = getJointPoint() - trailer->getJointPoint();
	//if(trailer->getJointPoint().y < getJointPoint().y)
		//trailer->getActor()->setGlobalPosition(trailer->getActor()->getGlobalPosition() + NxVec3(0, getJointPoint().y - trailer->getJointPoint().y, 0));
	trailer->getActor()->setGlobalPosition(trailer->getActor()->getGlobalPosition() + NxVec3(move));
	joint.actor[0] = getActor();
	joint.actor[1] = trailer->getActor();

	joint.setGlobalAnchor(NxVec3(getJointPoint()));
	joint.setGlobalAxis(NxVec3(-m_forward));

	joint.xMotion = NX_D6JOINT_MOTION_LOCKED;
	joint.yMotion = NX_D6JOINT_MOTION_LOCKED;
	joint.zMotion = NX_D6JOINT_MOTION_LOCKED;
	joint.swing1Motion = NX_D6JOINT_MOTION_LIMITED;
	joint.swing2Motion = NX_D6JOINT_MOTION_LIMITED;
	joint.twistMotion = NX_D6JOINT_MOTION_LIMITED;

	joint.swing1Limit.value = 0.3f;
	joint.swing2Limit.value = 1.8f;
	joint.twistLimit.low.value = -0.1f;
	joint.twistLimit.high.value = 0.1f;

	m_joint = m_scene->createJoint(joint);
	m_joinedTrailer = trailer;
	m_joinedTrailer->joinVehicle();
	return;
}

void Tractor::unjoinTrailer()
{
	if(m_joint && m_joinedTrailer)
	{
		m_scene->releaseJoint(*m_joint);
		m_joint = NULL;
		m_joinedTrailer->unjoinVehicle();
		m_joinedTrailer = NULL;
	}
}

VehicleDimms* Tractor::getVehicleDimms()
{
	return m_vehicleDimms;
}

void Tractor::setMotorTorque(float torque)
{
	stopMotor();
	increaseMotorTorque(torque);
}

float Tractor::getMotorTorque()
{
	return m_currentMotorTorque;
}

Vec3 Tractor::getForward()
{
	return m_forward;
}

float Tractor::getSteerAngle()
{
	return m_currentSteerAngle;
}



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
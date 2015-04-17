#include "4WheelVehicle.h"


Vehicle::Vehicle(Dynamics *dyn, NxVec3 pos, VehicleDimms *dimm)
{
	m_objChassis = NULL;
	m_objFrontLeftWheel = NULL;
	m_objFrontRightWheel = NULL;
	m_objRearLeftWheel = NULL;
	m_objRearRightWheel = NULL;

	m_jointPoint = dimm->jointPoint;

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
	m_vehicleDimms = dimm;

	m_joint = NULL;
	m_joinedTrailer = NULL;

	D3DXMatrixIdentity(&m_vehiclePose->matChassis);
	D3DXMatrixIdentity(&m_vehiclePose->matFrontLeftWheel);
	D3DXMatrixIdentity(&m_vehiclePose->matFrontRightWheel);
	D3DXMatrixIdentity(&m_vehiclePose->matRearLeftWheel);
	D3DXMatrixIdentity(&m_vehiclePose->matRearRightWheel);

	//m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(2,0.25,1), 10);
	m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(dimm->chassisDimm), dimm->chassisDensity);
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
	wheelDesc.wheelRadius = dimm->frontWheelsRadius;
	wheelDesc.wheelWidth = dimm->frontWheelsWidth;  // 0.1
	wheelDesc.wheelSuspension = 10;  
	wheelDesc.springRestitution = 0.1;
	wheelDesc.springDamping = 10;
	wheelDesc.springBias = 10;  
	wheelDesc.maxBrakeForce = 0.01;
	wheelDesc.position = NxVec3(dimm->posFrontLeftWheel);
	wheelDesc.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
						   NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;

	// Front right wheel
	NxWheelDesc wheelDesc2;
	wheelDesc2.frictionToFront = 100.0f;
	wheelDesc2.frictionToSide = 1.0f;



	wheelDesc2.wheelApproximation = 0;
	wheelDesc2.wheelRadius = dimm->frontWheelsRadius;
	wheelDesc2.wheelWidth = dimm->frontWheelsWidth;  // 0.1
	wheelDesc2.wheelSuspension = 10;  
	wheelDesc2.springRestitution = 0.1;
	wheelDesc2.springDamping = 10;
	wheelDesc2.springBias = 10;  
	wheelDesc.maxBrakeForce = 0.01;
	wheelDesc2.position = NxVec3(dimm->posFrontRightWheel);
	wheelDesc2.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;

	// Rear left wheel
	NxWheelDesc wheelDesc3;
	wheelDesc3.frictionToFront = 100.0f;
	wheelDesc3.frictionToSide = 1.0f;



	wheelDesc3.wheelApproximation = 0;
	wheelDesc3.wheelRadius = dimm->rearWheelsRadius;
	wheelDesc3.wheelWidth = dimm->rearWheelsWidth;  // 0.1
	wheelDesc3.wheelSuspension = 10;  
	wheelDesc3.springRestitution = 0.1;
	wheelDesc3.springDamping = 10;
	wheelDesc3.springBias = 10;  
	wheelDesc3.maxBrakeForce = 0.01;
	wheelDesc3.position = NxVec3(dimm->posRearLeftWheel);
	wheelDesc3.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;

	// Rear right wheel
	NxWheelDesc wheelDesc4;
	wheelDesc4.frictionToFront = 100.0f;
	wheelDesc4.frictionToSide = 1.0f;


	wheelDesc4.wheelApproximation = 0;
	wheelDesc4.wheelRadius = dimm->rearWheelsRadius;
	wheelDesc4.wheelWidth = dimm->rearWheelsWidth;  // 0.1
	wheelDesc4.wheelSuspension = 10;  
	wheelDesc4.springRestitution = 0.1;
	wheelDesc4.springDamping = 10;
	wheelDesc4.springBias = 10;  
	wheelDesc4.maxBrakeForce = 0.01;
	wheelDesc4.position = NxVec3(dimm->posRearRightWheel);
	wheelDesc4.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;


	wheelDesc4.frictionToFront = wheelDesc3.frictionToFront = wheelDesc2.frictionToFront = wheelDesc.frictionToFront = 100.0f;
	wheelDesc4.frictionToSide = wheelDesc3.frictionToSide = wheelDesc2.frictionToSide = wheelDesc.frictionToSide = 1.0f;


	/*wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.0001;
	wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 0.0001;
	wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.001;
	wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 0.001;
	wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01;*/
	wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = 0.5f / 10;
	wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = 1000.0f / 100000000000;
	wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = 0.5f / 10000000000000;
	wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = 1.0f / 10000000000000;
	wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = 0.01f; // 100;

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

Vehicle::Vehicle(Dynamics *dyn, NxVec3 pos, VehicleDimms *dimm, VehicleParamsEx *paramsEx)
{
	m_objChassis = NULL;
	m_objFrontLeftWheel = NULL;
	m_objFrontRightWheel = NULL;
	m_objRearLeftWheel = NULL;
	m_objRearRightWheel = NULL;

	m_jointPoint = dimm->jointPoint;

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
	m_vehicleDimms = dimm;

	m_joint = NULL;
	m_joinedTrailer = NULL;

	D3DXMatrixIdentity(&m_vehiclePose->matChassis);
	D3DXMatrixIdentity(&m_vehiclePose->matFrontLeftWheel);
	D3DXMatrixIdentity(&m_vehiclePose->matFrontRightWheel);
	D3DXMatrixIdentity(&m_vehiclePose->matRearLeftWheel);
	D3DXMatrixIdentity(&m_vehiclePose->matRearRightWheel);

	//m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(2,0.25,1), 10);
	m_actor = dyn->createBox(pos + NxVec3(0,0.5,0), NxVec3(dimm->chassisDimm), dimm->chassisDensity);
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
	wheelDesc.wheelRadius = dimm->frontWheelsRadius;
	wheelDesc.wheelWidth = dimm->frontWheelsWidth;  // 0.1
	wheelDesc.wheelSuspension = 10;  
	wheelDesc.springRestitution = 0.1;
	wheelDesc.springDamping = 10;
	wheelDesc.springBias = 10;  
	wheelDesc.maxBrakeForce = 0.01;
	wheelDesc.position = NxVec3(dimm->posFrontLeftWheel);
	wheelDesc.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
						   NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;

	// Front right wheel
	NxWheelDesc wheelDesc2;
	wheelDesc2.frictionToFront = 100.0f;
	wheelDesc2.frictionToSide = 1.0f;



	wheelDesc2.wheelApproximation = 0;
	wheelDesc2.wheelRadius = dimm->frontWheelsRadius;
	wheelDesc2.wheelWidth = dimm->frontWheelsWidth;  // 0.1
	wheelDesc2.wheelSuspension = 10;  
	wheelDesc2.springRestitution = 0.1;
	wheelDesc2.springDamping = 10;
	wheelDesc2.springBias = 10;  
	wheelDesc.maxBrakeForce = 0.01;
	wheelDesc2.position = NxVec3(dimm->posFrontRightWheel);
	wheelDesc2.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;

	// Rear left wheel
	NxWheelDesc wheelDesc3;
	wheelDesc3.frictionToFront = 100.0f;
	wheelDesc3.frictionToSide = 1.0f;



	wheelDesc3.wheelApproximation = 0;
	wheelDesc3.wheelRadius = dimm->rearWheelsRadius;
	wheelDesc3.wheelWidth = dimm->rearWheelsWidth;  // 0.1
	wheelDesc3.wheelSuspension = 10;  
	wheelDesc3.springRestitution = 0.1;
	wheelDesc3.springDamping = 10;
	wheelDesc3.springBias = 10;  
	wheelDesc3.maxBrakeForce = 0.01;
	wheelDesc3.position = NxVec3(dimm->posRearLeftWheel);
	wheelDesc3.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;

	// Rear right wheel
	NxWheelDesc wheelDesc4;
	wheelDesc4.frictionToFront = 100.0f;
	wheelDesc4.frictionToSide = 1.0f;


	wheelDesc4.wheelApproximation = 0;
	wheelDesc4.wheelRadius = dimm->rearWheelsRadius;
	wheelDesc4.wheelWidth = dimm->rearWheelsWidth;  // 0.1
	wheelDesc4.wheelSuspension = 10;  
	wheelDesc4.springRestitution = 0.1;
	wheelDesc4.springDamping = 10;
	wheelDesc4.springBias = 10;  
	wheelDesc4.maxBrakeForce = 0.01;
	wheelDesc4.position = NxVec3(dimm->posRearRightWheel);
	wheelDesc4.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
							NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;


	wheelDesc4.frictionToFront = wheelDesc3.frictionToFront = wheelDesc2.frictionToFront = wheelDesc.frictionToFront = paramsEx->frictionToFront;
	wheelDesc4.frictionToSide = wheelDesc3.frictionToSide = wheelDesc2.frictionToSide = wheelDesc.frictionToSide = paramsEx->frictionToSide;

	wheelDesc4.wheelSuspension = wheelDesc3.wheelSuspension = wheelDesc2.wheelSuspension = wheelDesc.wheelSuspension = paramsEx->wheelSuspension;
	wheelDesc4.springRestitution = wheelDesc3.springRestitution = wheelDesc2.springRestitution = wheelDesc.springRestitution = paramsEx->springRestitution;
	wheelDesc4.springDamping = wheelDesc3.springDamping = wheelDesc2.springDamping = wheelDesc.springDamping = paramsEx->springDamping;
	wheelDesc4.springBias = wheelDesc3.springBias = wheelDesc2.springBias = wheelDesc.springBias = paramsEx->springBias;
	wheelDesc4.maxBrakeForce = wheelDesc3.maxBrakeForce = wheelDesc2.maxBrakeForce = wheelDesc.maxBrakeForce = paramsEx->maxBrakeForce; // 100;

	m_wheelFrontLeft = AddWheelToActor(m_actor, &wheelDesc);
	m_wheelFrontRight = AddWheelToActor(m_actor, &wheelDesc2);
	m_wheelRearLeft = AddWheelToActor(m_actor, &wheelDesc3);
	m_wheelRearRight = AddWheelToActor(m_actor, &wheelDesc4);

	m_wheelFrontLeft->userData = (void*)new ShapeUserData;
	m_wheelFrontRight->userData = (void*)new ShapeUserData;
	m_wheelRearLeft->userData = (void*)new ShapeUserData;
	m_wheelRearRight->userData = (void*)new ShapeUserData;

	// LOWER THE CENTER OF MASS
	NxVec3 massPos;
	if(paramsEx->massCenter != Vec3(0, 0, 0))
		massPos = NxVec3(paramsEx->massCenter);
	else
	{
		massPos = m_actor->getCMassLocalPosition();
		massPos.y -= 1;
	}
	m_actor->setCMassOffsetLocalPosition(massPos);
	m_actor->wakeUp(1e30);
	stopMotorAndBreak();
}

Vehicle::~Vehicle()
{
}

NxActor* Vehicle::getActor()
{
	return m_actor;
}

void Vehicle::update()
{
	m_poseGiven = false;
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
		m_objChassis->getPose()->world = getVehiclePose()->matChassis;
		Vec3 min = Vec3(-2, -2, -2);
		Vec3 max = Vec3(2, 2, 2);
		Vec3 min2 = min;
		min2.x *= -1;
		Vec3 max2 = max;
		max2.x *= -1;
		Mat rot;
		D3DXMatrixRotationY(&rot, acos(D3DXVec3Dot(&Vec3(1, 0, 0), &getForward())));
		D3DXVec3TransformCoord(&min, &min, &rot);
		D3DXVec3TransformCoord(&max, &max, &rot);
		D3DXVec3TransformCoord(&min2, &min2, &rot);
		D3DXVec3TransformCoord(&max2, &max2, &rot);
		Vec3 center = m_objChassis->getPose()->boundingBox.getCenter();
		min += center;
		max += center;
		min2 += center;
		max2 += center;
		/*D3DXVec3TransformCoord(&min, &m_objChassis->getPose()->boundingBox.Min, &getVehiclePose()->matChassis);
		D3DXVec3TransformCoord(&max, &m_objChassis->getPose()->boundingBox.Max, &getVehiclePose()->matChassis);*/
		//core.game->getWorld()->getGrassManager()->changeTerrain(&min, &max, &min2, &max2);
	}

	if(m_objFrontLeftWheel)
	{
		m_objFrontLeftWheel->getPose()->boundingBox.setTransform(Vec3(getVehiclePose()->matFrontLeftWheel(3, 0), getVehiclePose()->matFrontLeftWheel(3, 1), getVehiclePose()->matFrontLeftWheel(3, 2)));
		m_objFrontLeftWheel->getPose()->world = getVehiclePose()->matFrontLeftWheel;
		m_objFrontLeftWheel->getPose()->position = Vec3(getVehiclePose()->matFrontLeftWheel._41, getVehiclePose()->matFrontLeftWheel._42, getVehiclePose()->matFrontLeftWheel._43);
	}

	if(m_objFrontRightWheel)
	{
		m_objFrontRightWheel->getPose()->boundingBox.setTransform(Vec3(getVehiclePose()->matFrontRightWheel(3, 0), getVehiclePose()->matFrontRightWheel(3, 1), getVehiclePose()->matFrontRightWheel(3, 2)));
		m_objFrontRightWheel->getPose()->world = getVehiclePose()->matFrontRightWheel;
		m_objFrontRightWheel->getPose()->position = Vec3(getVehiclePose()->matFrontRightWheel._41, getVehiclePose()->matFrontRightWheel._42, getVehiclePose()->matFrontRightWheel._43);
	}

	if(m_objRearLeftWheel)
	{
		m_objRearLeftWheel->getPose()->boundingBox.setTransform(Vec3(getVehiclePose()->matRearLeftWheel(3, 0), getVehiclePose()->matRearLeftWheel(3, 1), getVehiclePose()->matRearLeftWheel(3, 2)));
		m_objRearLeftWheel->getPose()->world = getVehiclePose()->matRearLeftWheel;
		m_objRearLeftWheel->getPose()->position = Vec3(getVehiclePose()->matRearLeftWheel._41, getVehiclePose()->matRearLeftWheel._42, getVehiclePose()->matRearLeftWheel._43);
	}

	if(m_objRearRightWheel)
	{
		m_objRearRightWheel->getPose()->boundingBox.setTransform(Vec3(getVehiclePose()->matRearRightWheel(3, 0), getVehiclePose()->matRearRightWheel(3, 1), getVehiclePose()->matRearRightWheel(3, 2)));
		m_objRearRightWheel->getPose()->world = getVehiclePose()->matRearRightWheel;
		m_objRearRightWheel->getPose()->position = Vec3(getVehiclePose()->matRearRightWheel._41, getVehiclePose()->matRearRightWheel._42, getVehiclePose()->matRearRightWheel._43);
	}
}

void Vehicle::steerAngle(float angle)
{
	m_currentSteerAngle += angle;
	if(m_currentSteerAngle >= m_vehicleDimms->maxSteerAngle)
		m_currentSteerAngle = m_vehicleDimms->maxSteerAngle;
	if(m_currentSteerAngle <= -m_vehicleDimms->maxSteerAngle)
		m_currentSteerAngle = -m_vehicleDimms->maxSteerAngle;
	m_setCurrentSteerAngle = true;
}
void Vehicle::increaseMotorTorque(float torque)
{
	m_currentBrakeTorque = 0;
	m_currentMotorTorque += torque;
	m_setCurrentMotorTorque = true;
	m_setCurrentBrakeTorque = true;
}
void Vehicle::increaseBreakTorque(float torque)
{
	m_currentBrakeTorque += torque;
	m_setCurrentBrakeTorque = true;
}
void Vehicle::stopMotorAndBreak()
{
	m_currentBrakeTorque = 1;
	m_currentMotorTorque = 0;
	m_setCurrentMotorTorque = true;
	m_setCurrentBrakeTorque = true;
}

void Vehicle::stopMotor()
{
	m_currentMotorTorque = 0;
	m_setCurrentMotorTorque = true;
}

void Vehicle::adjustSteerAngle()
{
	m_currentSteerAngle = 0;
	m_setCurrentSteerAngle = true;
}

VehiclePose* Vehicle::getVehiclePose()
{
	if(m_poseGiven)
		return m_vehiclePose;
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

	return m_vehiclePose;
}

int Vehicle::getSpeed()
{
	Vec3 Way = Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z) - m_oldPos;
	m_oldPos = Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z);
	return D3DXVec3Length(&Way) / core.dynamics->getDeltaTime();
}

void Vehicle::attachRendObj(RendObj *chassis, RendObj *frontLeftWheel, RendObj *frontRightWheel, RendObj *rearLeftWheel, RendObj *rearRightWheel)
{
	m_objChassis = chassis;
	m_objFrontLeftWheel = frontLeftWheel;
	m_objFrontRightWheel = frontRightWheel;
	m_objRearLeftWheel = rearLeftWheel;
	m_objRearRightWheel = rearRightWheel;
}

Vec3 Vehicle::getJointPoint()
{
	Vec3 retVal;
	D3DXVec3TransformCoord(&retVal, &m_jointPoint, &getVehiclePose()->matChassis);
	return retVal;
}

void Vehicle::joinTrailer(ITrailer* trailer)
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

void Vehicle::unjoinTrailer()
{
	if(m_joint && m_joinedTrailer)
	{
		m_scene->releaseJoint(*m_joint);
		m_joint = NULL;
		m_joinedTrailer->unjoinVehicle();
		m_joinedTrailer = NULL;
	}
}

VehicleDimms* Vehicle::getVehicleDimms()
{
	return m_vehicleDimms;
}

void Vehicle::setMotorTorque(float torque)
{
	stopMotor();
	increaseMotorTorque(torque);
}

float Vehicle::getMotorTorque()
{
	return m_currentMotorTorque;
}

Vec3 Vehicle::getForward()
{
	return m_forward;
}

float Vehicle::getSteerAngle()
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
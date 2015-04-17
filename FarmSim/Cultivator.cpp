#include "Cultivator.h"

Cultivator::Cultivator(Dynamics *dyns, NxVec3 pos, string fname)
{
	/*m_vehicleDimms = new CultivatorDimms;
	VehicleParamsEx *paramsEx = new VehicleParamsEx;
	vector<CfgVariable*> vars;
	vars = *m_vehicleDimms->getVariableList();
	for(int i = 0; i < paramsEx->getVariableList()->size(); i++)
	{
		vars.push_back((*paramsEx->getVariableList())[i]);
	}
	Vec3 rotateFrontLeftWheel;
	Vec3 rotateFrontRightWheel;
	Vec3 rotateChassis;

	string paramsType;
	string wheelFrontLeftModel;
	string wheelFrontRightModel;
	string chassisModel;

	vars.push_back(new CfgVariable(V_VEC3, &rotateFrontLeftWheel, getVarName(rotateFrontLeftWheel)));
	vars.push_back(new CfgVariable(V_VEC3, &rotateFrontRightWheel, getVarName(rotateFrontRightWheel)));
	vars.push_back(new CfgVariable(V_VEC3, &rotateChassis, getVarName(rotateChassis)));

	vars.push_back(new CfgVariable(V_STRING, &paramsType, getVarName(paramsType)));

	vars.push_back(new CfgVariable(V_STRING, &wheelFrontLeftModel, getVarName(wheelFrontLeftModel)));
	vars.push_back(new CfgVariable(V_STRING, &wheelFrontRightModel, getVarName(wheelFrontRightModel)));

	vars.push_back(new CfgVariable(V_STRING, &chassisModel, getVarName(chassisModel)));
	vars.push_back(new CfgVariable(V_STRING, &strtrailerType, getVarName(trailerType)));

	CfgLoader cfg(fname, &vars);

	D3DXMatrixRotationYawPitchRoll(&m_vehicleDimms->matFrontLeftWheelRotation, rotateFrontLeftWheel.y, rotateFrontLeftWheel.x, rotateFrontLeftWheel.z);
	D3DXMatrixRotationYawPitchRoll(&m_vehicleDimms->matFrontRightWheelRotation, rotateFrontRightWheel.y, rotateFrontRightWheel.x, rotateFrontRightWheel.z);
	D3DXMatrixRotationYawPitchRoll(&m_vehicleDimms->matChassisRotation, rotateChassis.y, rotateChassis.x, rotateChassis.z);

	m_objChassis = new RendObj(chassisModel, new Material("wood.jpg", "", ""), Vec3(0, 0, 0));
	core.game->getWorld()->addToWorld(m_objChassis, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);*/
}

Cultivator::~Cultivator()
{
}

NxActor* Cultivator::getActor()
{
	return m_actor;
}

void Cultivator::update()
{
	m_poseGiven = false;
	if(m_objChassis)
	{
		m_objChassis->getPose()->boundingBox.setTransform(Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z));
		m_objChassis->getPose()->world = ((TrailerPose*)((TrailerPose*)getVehiclePose()))->matChassis;
	}

	if(m_objFrontLeftWheel)
	{
		m_objFrontLeftWheel->getPose()->boundingBox.setTransform(Vec3(((TrailerPose*)getVehiclePose())->matFrontLeftWheel(3, 0), ((TrailerPose*)getVehiclePose())->matFrontLeftWheel(3, 1), ((TrailerPose*)getVehiclePose())->matFrontLeftWheel(3, 2)));
		m_objFrontLeftWheel->getPose()->world = ((TrailerPose*)((TrailerPose*)getVehiclePose()))->matFrontLeftWheel;
	}

	if(m_objFrontRightWheel)
	{
		m_objFrontRightWheel->getPose()->boundingBox.setTransform(Vec3(((TrailerPose*)getVehiclePose())->matFrontRightWheel(3, 0), ((TrailerPose*)getVehiclePose())->matFrontRightWheel(3, 1), ((TrailerPose*)getVehiclePose())->matFrontRightWheel(3, 2)));
		m_objFrontRightWheel->getPose()->world = ((TrailerPose*)((TrailerPose*)getVehiclePose()))->matFrontRightWheel;
	}
}

void* Cultivator::getVehiclePose()
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

	return (void*)m_vehiclePose;
}

int Cultivator::getSpeed()
{
	Vec3 Way = Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z) - m_oldPos;
	m_oldPos = Vec3(m_actor->getGlobalPosition().x, m_actor->getGlobalPosition().y, m_actor->getGlobalPosition().z);
	return D3DXVec3Length(&Way) / core.dynamics->getDeltaTime();
}

void Cultivator::attachRendObj(RendObj *m_objChassis, RendObj *frontLeftWheel, RendObj *frontRightWheel, RendObj *rearLeftWheel, RendObj *rearRightWheel)
{
	m_objChassis = m_objChassis;
	m_objFrontLeftWheel = frontLeftWheel;
	m_objFrontRightWheel = frontRightWheel;
	m_objRearLeftWheel = rearLeftWheel;
	m_objRearRightWheel = rearRightWheel;
}


Vec3 Cultivator::getJointPoint()
{
	Vec3 retVal;
	D3DXVec3TransformCoord(&retVal, &m_jointPoint, &((TrailerPose*)getVehiclePose())->matChassis);
	return retVal;
}

void Cultivator::joinVehicle()
{
	m_joined = true;
}

void Cultivator::unjoinVehicle()
{
	m_joined = false;
}

void* Cultivator::getVehicleDimms()
{
	return (void*)m_vehicleDimms;
}


void Cultivator::action1()
{
}

void Cultivator::action2()
{
}

void Cultivator::action3()
{
}

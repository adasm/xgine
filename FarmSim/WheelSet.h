#pragma once
#include "Headers.h"

struct WheelSetParams
{
	WheelSetParams()
	{
		frontWheelsRadius = 0.0f;
		frontWheelsWidth = 0.0f;

		middleWheelsRadius = 0.0f;
		middleWheelsWidth = 0.0f;

		rearWheelsRadius = 0.0f;
		rearWheelsWidth = 0.0f;

		posWheelLeftFront = Vec3(0, 0, 0);
		posWheelRightFront = Vec3(0, 0, 0);

		posWheelLeftMiddle = Vec3(0, 0, 0);
		posWheelRightMiddle = Vec3(0, 0, 0);

		posWheelLeftRear = Vec3(0, 0, 0);
		posWheelRightRear = Vec3(0, 0, 0);
		posConnector = Vec3(0, 0, 0);

		vecWheelLeftFrontRotation = Vec3(0, 0, 0);
		vecWheelRightFrontRotation = Vec3(0, 0, 0);

		vecWheelLeftMiddleRotation = Vec3(0, 0, 0);
		vecWheelRightMiddleRotation = Vec3(0, 0, 0);

		vecWheelLeftRearRotation = Vec3(0, 0, 0);
		vecWheelRightRearRotation = Vec3(0, 0, 0);

		chassisDimm = Vec3(0, 0, 0);
		chassisDensity = 0;
		m_varList = NULL;
	}
	float chassisDensity;

	float frontWheelsRadius;
	float frontWheelsWidth;

	float middleWheelsRadius;
	float middleWheelsWidth;

	float rearWheelsRadius;
	float rearWheelsWidth;
	Vec3 posWheelLeftFront;
	Vec3 posWheelRightFront;

	Vec3 posWheelLeftMiddle;
	Vec3 posWheelRightMiddle;

	Vec3 posWheelLeftRear;
	Vec3 posWheelRightRear;

	Vec3 vecWheelLeftFrontRotation;
	Vec3 vecWheelRightFrontRotation;

	Vec3 vecWheelLeftMiddleRotation;
	Vec3 vecWheelRightMiddleRotation;

	Vec3 vecWheelLeftRearRotation;
	Vec3 vecWheelRightRearRotation;

	Vec3 chassisDimm;

	Vec3 posConnector;

	vector<CfgVariable*> *getVariableList();

protected:
	vector<CfgVariable*> *m_varList;
};

class WheelSet
{
public:
	WheelSet(Vec3 pos, WheelSetParams* params, VehicleParamsEx* paramsEx = NULL);

	WheelSetPose*		getPose();
	NxActor*			getActor();

	Vec3				getForwardVec();
	Vec3				getConnectorPos();
	void				update();
protected:
	WheelSetPose*		m_pose;

	Vec3				m_forward;
	//Vec3				m_oldPos;

	NxScene				*m_scene;
	WheelSetParams		*m_params;
	VehicleParamsEx		*m_paramsEx;
	NxActor				*m_actor;

	NxWheelShape		*m_wheelLeftFront;
	NxWheelShape		*m_wheelRightFront;

	NxWheelShape		*m_wheelLeftMiddle;
	NxWheelShape		*m_wheelRightMiddle;

	NxWheelShape		*m_wheelLeftRear;
	NxWheelShape		*m_wheelRightRear;

	bool				m_poseGiven;
};
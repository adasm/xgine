#pragma once
#include "Headers.h"

enum WheelDriveType
{
	WDT_2WD = 0,
	WDT_4WD = 1,
	WDT_FWD = 2,
};

struct VehicleParamsEx
{
	float frictionToFront;
	float frictionToSide;

	float wheelSuspension;
	float springRestitution;
	float springDamping;
	float springBias;
	float maxBrakeForce;

	Vec3 massCenter;

	VehicleParamsEx()
	{
		frictionToFront = 0.0f;
		frictionToSide = 0.0f;

		wheelSuspension = 0.0f;
		springRestitution = 0.0f;
		springDamping = 0.0f;
		springBias = 0.0f;
		maxBrakeForce = 0.0f;

		massCenter = Vec3(0, 0, 0);
		varList = NULL;
	}
	vector<CfgVariable*> *getVariableList();
protected:
	vector<CfgVariable*> *varList;
};


class VehicleController
{
public:
	VehicleController(Vec3 pos, VehicleEngine* vehEngine, VehicleParams *vehParams, VehicleParamsEx *vehParamsEx = NULL, bool steerableWheelsFront = false);
	//VehicleController(Vec3 pos, VehicleEngine* vehEngine, VehicleParams *vehParams, VehicleParamsEx *vehParamsEx = NULL);
	NxActor*			getActor() { return m_actor; }

	VehicleParams*		getVehicleParams();
	VehicleParamsEx*	getVehicleParamsEx();
	VehiclePose*		getVehiclePose();
	VehicleEngine*		getVehicleEngine();

	void				setSteerAngle(float angle = NxPi*0.00015); // angle > 0 = left, angle < 0 = right
	void				setAccelerate(); //range: -1.0...1.0 5 = automatic
	void				setBackward();
	void				setBrake(); //range -1.0...1.0 5 = automatic
	void				setParkingBrake();
	void				releaseBrake();
	void				releaseParkingBrake();
	void				releaseMotor();

	void				update();

	float				getSteerAngle();
	Vec3				getForwardVec();
	Vec3				getLeftVec();
	Vec3				getFrontJointPoint();
	Vec3				getBackJointPoint();
	WheelDriveType		getWheelDriveType() { return m_wheelDriveType; }

	float				getSpeed();

	void				resetPose();

protected:
	Vec3				m_forward;
	Vec3				m_left;
	//Vec3				m_oldPos;

	NxScene				*m_scene;
	VehiclePose			*m_vehiclePose;
	VehicleParams		*m_vehicleParams;
	VehicleParamsEx		*m_vehicleParamsEx;
	VehicleEngine		*m_vehicleEngine;
	NxActor				*m_actor;

	NxWheelShape		*m_wheelFrontLeft;
	NxWheelShape		*m_wheelFrontRight;
	NxWheelShape		*m_wheelRearLeft;
	NxWheelShape		*m_wheelRearRight;

	bool				m_poseGiven;

	bool				m_setCurrentSteerAngle;
	bool				m_setCurrentMotorTorque;
	bool				m_setCurrentBrakeTorque;

	float				m_currentSteerAngle;
	float				m_currentMotorTorque;
	float				m_currentBrakeTorque;

	bool				m_steerableWheelsFront;
	WheelDriveType		m_wheelDriveType;
};
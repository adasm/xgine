#pragma once
#include "Headers.h"

struct Tractor : IVehicle
{
public:
	Tractor(Dynamics *dyn, NxVec3 pos, string fname);
	//Tractor(Dynamics *dyns, NxVec3 pos, VehicleDimms *dimm);
	//Tractor(Dynamics *dyns, NxVec3 pos, VehicleDimms *dimm, VehicleParamsEx *paramsEx);
	~Tractor();

	NxActor* getActor();

	void*	getVehiclePose();
	VehicleDimms*	getVehicleDimms();
	void attachRendObj(RendObj* chassis, RendObj* frontLeftWheel, RendObj* frontRightWheel, RendObj* rearLeftWheel, RendObj* rearRightWheel);
	void update();

	void steerAngle(float angle = NxPi*0.00015); // angle > 0 = left, angle < 0 = right
	void increaseMotorTorque(float torque = 1);
	void increaseBreakTorque(float torque = 1);
	int getSpeed();

	void adjustSteerAngle();
	void stopMotorAndBreak();
	void stopMotor();

	void setMotorTorque(float torque);
	float getMotorTorque();
	Vec3 getForward();
	Vec3 getJointPoint();
	float getSteerAngle();

	void joinTrailer(ITrailer* trailer);
	void unjoinTrailer();
	VehicleType getVehicleType() { return VT_TRACTOR; }

	void action1() { };
	void action2() { };
protected:

	Vec3			m_oldPos;
	Vec3			m_forward;
	Vec3			m_jointPoint;

	VehicleDimms*	m_vehicleDimms;
	VehiclePose*	m_vehiclePose;

	NxWheelShape*	m_wheelFrontLeft;
	NxWheelShape*	m_wheelFrontRight;
	NxWheelShape*	m_wheelRearLeft;
	NxWheelShape*	m_wheelRearRight;

	RendObj*		m_objChassis;
	RendObj*		m_objFrontLeftWheel;
	RendObj*		m_objFrontRightWheel;
	RendObj*		m_objRearLeftWheel;
	RendObj*		m_objRearRightWheel;

	NxActor*		m_actor;
	NxScene*		m_scene;

	bool			m_setCurrentSteerAngle;
	bool			m_setCurrentMotorTorque;
	bool			m_setCurrentBrakeTorque;
	NxF32			m_currentSteerAngle;
	NxF32			m_currentMotorTorque;
	int				m_currentBrakeTorque;

	bool			m_poseGiven;

	ITrailer*		m_joinedTrailer;
	NxJoint*		m_joint;
};
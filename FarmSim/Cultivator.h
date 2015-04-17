#pragma once
#include "Headers.h"


struct Cultivator : ITrailer
{
public:
	Cultivator(Dynamics *dyns, NxVec3 pos, string fname);
	~Cultivator();

	NxActor* getActor();
	void*	getVehiclePose();
	void*	getVehicleDimms();

	void attachRendObj(RendObj* chassis, RendObj* frontLeftWheel, RendObj* frontRightWheel, RendObj* rearLeftWheel, RendObj* rearRightWheel);
	void update();
	int getSpeed();

	Vec3 getForward() { return m_forward; }
	Vec3 getJointPoint();

	void joinVehicle();
	void unjoinVehicle();

	void action1();
	void action2();
	void action3();

	bool isJoined() { return m_joined; }
	bool isRigid() { return (bool)m_isRigid; }
protected:

	Vec3			m_oldPos;
	Vec3			m_forward;
	Vec3			m_jointPoint;
	Vec3			m_revoluteJointPoint;

	BoundingBox		m_actionBox;

	TrailerDimms*	m_vehicleDimms;
	TrailerPose*	m_vehiclePose;

	NxWheelShape*	m_wheelFrontLeft;
	NxWheelShape*	m_wheelFrontRight;
	NxWheelShape*	m_wheelRearLeft;
	NxWheelShape*	m_wheelRearRight;

	RendObj*		m_objChassis;
	RendObj*		m_objDumper;
	RendObj*		m_objFrontLeftWheel;
	RendObj*		m_objFrontRightWheel;
	RendObj*		m_objRearLeftWheel;
	RendObj*		m_objRearRightWheel;

	NxActor*		m_actor;
	NxActor*		m_dumperActor;
	NxRevoluteJoint* m_revJoint;

	NxScene*		m_scene;

	bool			m_poseGiven;

	bool			m_joined;
	int				m_isRigid;

	TrailerWheelNb		m_trailerType;
	TrailerActionType m_trailerActionType;
};
#pragma once
#include "Headers.h"


struct ITrailer
{
public:
	virtual void			update()							= 0;
	virtual void			updateInput()						= 0;

	virtual	TrailerPose*	getPose()							= 0;

	virtual void			attachToTractor(Tractor* tractor)	= 0;
	virtual void			detach()							= 0;
	virtual Tractor*		isAttached()						= 0;
	virtual WheelSet*		getWheelSet()						= 0;
	virtual ActionBox*		getDumperActionBox()				= 0;

	virtual int				getCapacity()						= 0;
	virtual int				getPayloadValue()					= 0;
	virtual bool			addPayload(int payloadToAdd,
										CropType* cropType)		= 0;
	virtual CropType*		getLoadedCropType()					= 0;
	virtual Vec3			getPayloadDumpPos()					= 0;
	virtual bool			isDumperUp()						= 0;
	/*virtual ~ITrailer()										{ }

	virtual NxActor*		getActor()						= 0;
	virtual void*			getPose()						= 0;

	virtual void			update()						= 0;
	virtual int				getSpeed()						= 0;

	virtual Vec3			getForward()					= 0;
	virtual Vec3			getJointPoint()					= 0;

	virtual void			joinVehicle()					= 0;
	virtual void			unjoinVehicle()					= 0;

	virtual bool			isJoined()						= 0;
	virtual TrailerType		getTrailerType()				= 0;
	virtual VehicleType		getVehicleTypeDestination()		= 0;
	//virtual bool			isRigid()						= 0;*/
protected:
/*
	Vec3			m_oldPos;
	Vec3			m_forward;
	Vec3			m_jointPoint;

	BoundingBox		m_actionBox;

	VehicleDimms*	m_vehicleDimms;
	VehiclePose*	m_vehiclePose;

	NxWheelShape*	m_wheelFrontLeft;
	NxWheelShape*	m_wheelFrontRight;
	NxWheelShape*	m_wheelRearLeft;
	NxWheelShape*	m_wheelRearRight;

	Surface*		m_objChassis;
	Surface*		m_objFrontLeftWheel;
	Surface*		m_objFrontRightWheel;
	Surface*		m_objRearLeftWheel;
	Surface*		m_objRearRightWheel;

	NxActor*		m_actor;
	NxScene*		m_scene;

	bool			m_poseGiven;

	bool			m_joined;
	int				m_isRigid;

	TrailerWheelNb		m_trailerType;
	TrailerActionType m_trailerActionType;*/
};
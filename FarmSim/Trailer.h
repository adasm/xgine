#pragma once
#include "Headers.h"


struct Trailer : ITrailer
{
public:
	Trailer(Vec3 pos, string fname);
	

	void			update();
	void			updateInput();

	TrailerPose*	getPose();
	
	void			attachToTractor(Tractor* tractor);
	void			detach();
	Tractor*		isAttached();
	WheelSet*		getWheelSet() { return m_wheelSet; }

	bool			addPayload(int payloadToAdd, CropType* cropType);
	int				getCapacity();
	int				getPayloadValue();
	CropType*		getLoadedCropType();

	ActionBox*		getDumperActionBox();
	Vec3			getPayloadDumpPos();
	bool			isDumperUp() { return m_dumperIsGoingUp; }

protected:
	ActionBox*		m_dumperActionBox;
	Tractor*		m_attachedTractor;
	TrailerParams*	m_params;
	WheelSet*		m_wheelSet;
	TrailerPose*	m_pose;
	bool			m_poseGiven;

	CropType*		m_dumperActualPayloadType;
	int				m_dumperActualPayload;
	float			m_dumperActualAngle;
	bool			m_dumperIsGoingUp;

	Surface*		m_objChassis;
	Surface*		m_objDumper;
	Surface*		m_objHarvest;

	Surface*		m_objWheelLeftFront;
	Surface*		m_objWheelRightFront;

	Surface*		m_objWheelLeftMiddle;
	Surface*		m_objWheelRightMiddle;

	Surface*		m_objWheelLeftRear;
	Surface*		m_objWheelRightRear;

};
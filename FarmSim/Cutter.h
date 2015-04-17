#pragma once
#include "Headers.h"

struct Cutter : IAgriDevice
{
public:
	Cutter(NxVec3 pos, string fname);
	void				update();
	void				updateInput();

	ActionBox*			getActionBox() { return m_actionBox; }
	TriPod*				isAttached();
	VehicleType			getVehicleTypeDestination() { return VT_COMBINE; } //returns type of vehicle to use with
	void				attach(TriPod *tripod);
	void				detach();
	AgriDeviceType		getDeviceType() { return ADT_CUTTER; }
	NxActor				*getActor() { return m_actor; }
	Vec3				getTriPodPos();
protected:
	ActionBox			*m_actionBox;
	CutterParams		*m_cutterParams;
	NxActor				*m_actor;

	Surface				*m_objChassis;
	Surface				*m_objCutter;

	float				m_cutterHeight;
	float				m_cutterRotateAngle;
	bool				m_cutterON;

	TriPod				*m_attachedTriPod;
};
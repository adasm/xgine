#pragma once
#include "Headers.h"

struct Plow : IAgriDevice
{
public:
	Plow(NxVec3 pos, string fname);
	void				update();
	void				updateInput();

	ActionBox*			getActionBox() { return m_actionBox; }
	TriPod*				isAttached();
	VehicleType			getVehicleTypeDestination() { return VT_TRACTOR; } //returns type of vehicle to use with
	void				attach(TriPod *tripod);
	void				detach();
	AgriDeviceType		getDeviceType() { return ADT_PLOW; }
	NxActor				*getActor() { return m_actor; }
	Vec3				getTriPodPos();
protected:
	ActionBox			*m_actionBox;
	PlowDimms			*m_plowDimms;
	NxActor				*m_actor;

	Surface				*m_objChassis;

	TriPod				*m_attachedTriPod;
};
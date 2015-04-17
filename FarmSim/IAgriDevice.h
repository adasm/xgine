#pragma once
#include "Headers.h"

enum AgriDeviceType
{
	ADT_CULTIVATOR,
	ADT_PLOW,
	ADT_CUTTER,
};

struct IAgriDevice
{
public:
	virtual		void				update() = 0;
	virtual		void				updateInput() = 0;

	virtual		ActionBox*			getActionBox() = 0;
	virtual		Vec3				getTriPodPos() = 0;
	virtual		NxActor*			getActor() = 0;
	virtual		void				detach() = 0;
	virtual		TriPod*				isAttached() = 0;
	virtual		VehicleType			getVehicleTypeDestination() = 0; //returns type of vehicle to use with
	virtual		void				attach(TriPod *tripod) = 0;
	virtual		AgriDeviceType		getDeviceType() = 0;
protected:
};
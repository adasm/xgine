#pragma once
#include "Headers.h"

enum VehicleType
{
	VT_TRACTOR,
	VT_COMBINE,
};

struct IVehicle
{
public:
	~IVehicle() { };

	virtual VehicleController	*getVehicleController() = 0;
	virtual VehicleType			getVehicleType() = 0;
	virtual TriPod				*getBackTriPod() = 0;
	virtual TriPod				*getFrontTriPod() = 0;

	virtual void				update() = 0;
	virtual void				updateInput() = 0;
	virtual Vec3				getPos() = 0;
protected:
};
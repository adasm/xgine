#pragma once
#include "Headers.h"

struct Tractor : IVehicle
{
public:
	Tractor(Vec3 pos, string fname);

	void				update();
	void				updateInput();

	VehicleController	*getVehicleController();

	VehicleType			getVehicleType() { return VT_TRACTOR; }
	TriPod				*getBackTriPod();
	TriPod				*getFrontTriPod();

	Vec3				getPos();
protected:
	bool				m_hasParkingBrake;
	bool				m_actualBraking;

	VehicleController	*m_vehicleController;
	VehicleEngine		*m_vehicleEngine;

	Surface				*m_objChassis;
	Surface				*m_objFrontLeftWheel;
	Surface				*m_objFrontRightWheel;
	Surface				*m_objRearLeftWheel;
	Surface				*m_objRearRightWheel;

	TriPod				*m_backTriPod;
	TriPod				*m_frontTriPod;
	Vec3				m_position;
};
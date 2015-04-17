#pragma once
#include "Headers.h"

struct Combine : IVehicle
{
public:
	Combine(Vec3 pos, string fname);

	void				update();
	void				updateInput();

	VehicleController	*getVehicleController();

	VehicleType			getVehicleType() { return VT_COMBINE; }
	TriPod				*getBackTriPod();
	TriPod				*getFrontTriPod();

	Vec3				getDumpPoint();
	void				unloadHarvestToTrailer();
	int					getCapacity();
	int					getPayloadValue();
	void				addPayload(int payloadToAdd, CropType* cropType);
	CropType*			getLoadedCropType();

	Vec3				getPos();
protected:
	bool				m_hasParkingBrake;
	bool				m_actualBraking;

	VehicleController	*m_vehicleController;
	VehicleEngine		*m_vehicleEngine;

	Surface				*m_objPipe;
	Surface				*m_objChassis;
	Surface				*m_objFrontLeftWheel;
	Surface				*m_objFrontRightWheel;
	Surface				*m_objRearLeftWheel;
	Surface				*m_objRearRightWheel;

	
	TriPod				*m_backTriPod;
	TriPod				*m_frontTriPod;

	Vec3				m_vecPipeRotation;
	Vec3				m_vecPipeTranslation;
	int					m_capacity;
	int					m_actualPayload;
	CropType			*m_actualPayloadType;
	Vec3				m_dumpPoint;

	bool				m_pipeOut;
	float				m_pipeAngle;

	Vec3				m_position;
};
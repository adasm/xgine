#pragma once
#include "Headers.h"

enum AttachType
{
	AT_TRAILER,
	AT_DEVICE,
	AT_NONE,
};

class TriPod
{
public:
	TriPod(IVehicle *vehicle, string fname, bool FrontTriPod = false);

	void				update();

	void				lift();
	void				low();

	bool				isLifted();

	AttachType			isAttached();
	void				*getAttach();
	void				attach(IAgriDevice* device);
	void				attach(ITrailer* trailer);
	void				detach();
	IVehicle			*getVehicle() { return m_vehicle; }
protected:
	void				reset();
	bool				m_frontPod;
	IVehicle			*m_vehicle;

	bool				m_tractor;
	bool				m_isLifted;

	Surface				*m_objTop;
	Surface				*m_objBottom;

	Mat					m_matTop;
	Mat					m_matBottom;

	NxActor				*m_actorTop;
	NxActor				*m_actorBottom;

	NxRevoluteJoint		*m_jointTop;
	NxRevoluteJoint		*m_jointBottomLeft;
	NxRevoluteJoint		*m_jointBottomRight;

	NxD6Joint			*m_jointTopPodDevice;
	NxD6Joint			*m_jointBottomLeftPodDevice;
	NxD6Joint			*m_jointBottomRightPodDevice;

	NxD6Joint			*m_jointTrailer;
	/*NxRevoluteJoint		*m_jointTopPodDevice;
	NxRevoluteJoint		*m_jointBottomLeftPodDevice;
	NxRevoluteJoint		*m_jointBottomRightPodDevice;*/

	NxMotorDesc			*m_jointMotorTop;
	NxMotorDesc			*m_jointMotorBottom;

	/*NxD6Joint			*m_jointTop;
	NxD6Joint			*m_jointBottom;*/

	TriPodDimms			*m_triPodDimms;

	IAgriDevice			*m_attachedDevice;
	ITrailer			*m_attachedTrailer;
};
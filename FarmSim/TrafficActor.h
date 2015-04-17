#pragma once
#include "Headers.h"

class TrafficActor
{
public:
	TrafficActor(IVehicle* vehicle);
	void			update();
	void			triggerIncrease();
	void			triggerDecrease();
	void			triggerInCollision();
protected:
	NxActor*		m_triggerActor;
	IVehicle*		m_vehicle;
	TrafficNode*	m_actualNode;
	Vec3			m_triggerPosition;
	int				m_triggerCollision;
	bool			m_triggerInCollision;
	int				m_upsideDownParameter;
};
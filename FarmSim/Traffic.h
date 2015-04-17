#pragma once
#include "Headers.h"

class TrafficManager
{
public:
	TrafficManager();
	TrafficActor*			addActor(IVehicle* vehicle);
	NavigationMesh*			getNavigationMesh() { return m_naviMesh; }

	void					update(ICamera *cam);
protected:
	NavigationMesh*			m_naviMesh;
	vector<TrafficActor*>	m_actors;
};
#include "Traffic.h"

TrafficManager::TrafficManager()
{
	m_naviMesh = new NavigationMesh;
}

TrafficActor* TrafficManager::addActor(IVehicle *vehicle)
{
	if(!vehicle)
		return NULL;
	TrafficActor* returnVal = new TrafficActor(vehicle);
	m_actors.push_back(returnVal);
	return returnVal;
}

void TrafficManager::update(ICamera *cam)
{
	for(int i = 0; i < m_actors.size(); i++)
	{
		m_actors[i]->update();
	}
}
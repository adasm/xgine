#include "FrustumCulling.h"

u32 FrustumCulling::check(ICamera* cam, vector<IEntity*> &entities, EntityList* entList)
{
	if(cam == 0) return 0;
	const BoundingSphere*	frustumSphere = cam->getFrustumSphere();
	const Plane*			frustumPlanes = cam->getFrustumPlanes();
	
	for(u32 i = 0; i < entities.size(); i++)
	{
			if(entities[i]->getPose()->boundingBox.BSphere.intersect(frustumSphere))
				if(entities[i]->getPose()->boundingBox.BSphere.intersect(frustumPlanes))
					entList->add(entities[i]);
	}
	return 1;
}
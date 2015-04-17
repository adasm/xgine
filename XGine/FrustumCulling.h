#pragma once
#include "XGine.h"

struct XGINE_API FrustumCulling
{
	template< class T > static u32 check(ICamera* cam, vector<T*> &entities, EntityList<T>* entList);
};

template< typename T > u32 FrustumCulling::check(ICamera* cam, vector<T*> &entities, EntityList<T>* entList)
{
	if(cam == 0) return 0;
	const BoundingSphere*	frustumSphere = cam->getFrustumSphere();
	const Plane*			frustumPlanes = cam->getFrustumPlanes();
	
	for(u32 i = 0; i < entities.size(); i++)
	{
			if(entities[i]->boundingBox.BSphere.intersect(frustumSphere))
				if(entities[i]->boundingBox.BSphere.intersect(frustumPlanes))
					entList->add(entities[i]);
	}
	return 1;
}
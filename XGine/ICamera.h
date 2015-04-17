#pragma once
#include "XGine.h"

class XGINE_API ICamera
{
private:
protected:
public:
	virtual					~ICamera() {}
	virtual void			update() {}
	virtual f32				getFOV() { return 0; }
	virtual Mat*			getViewProjectionMat() { return NULL; }
	virtual Mat*			getViewMat() { return NULL; }
	virtual Mat*			getProjectionMat() { return NULL; }
	virtual const Vec3*		getPosition() const { return NULL; }
	virtual const Vec4*		getPositionV4() const { return NULL; }
	virtual Vec3*			getViewDirection() { return NULL; }
	virtual Vec4*			getViewDirectionV4() { return NULL; }
	virtual const Vec4*		getViewUp() const { return NULL; }
	virtual const Vec4*		getViewLeft() const { return NULL; }
	virtual const BoundingSphere*	getFrustumSphere() const { return NULL; }
	virtual const BoundingCone*		getFrustumCone() const { return NULL; }
	virtual const Plane*			getFrustumPlanes() const { return NULL; } //near, far, left, right, top, bottom
	virtual const Vec3*				getFrustumVertices() const { return NULL; } //near-bottom-left, nbr, ntl, ntr, fbl, fbr, ftl, ftr
};
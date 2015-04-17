#pragma once
#include "XGine.h"

class XGINE_API PerspectiveCamera : public ICamera
{
private:
protected:
	const Vec3 c_up;
	Vec3	m_look;  //wersor widoku
	Vec3	m_left; //wersor lewej
	Vec3	m_up; //wersor gory
	Vec3	m_position;
	Vec4	m_lookV4;  //wersor widoku
	Vec4	m_leftV4; //all ...V4 should be set on update
	Vec4	m_upV4;
	Vec4	m_positionV4;
	f32		m_frustumCenterZ; //(ZFar + ZNear) / 2
	f32		m_zNear;
	f32		m_zFar;
	f32		m_nearWidth;
	f32		m_nearHeight;
	f32		m_farWidth;
	f32		m_farHeight;
	f32		m_FOV;
	Mat		m_projectionMat;
	Mat		m_viewMat;
	Mat		m_viewProjectionMat;
	bool	m_updateViewMat;
	Plane	m_frustum[6]; //plaszczyzny ograniczajace stozek widzenia
	Vec3	m_frustumVertex[8]; //wierzcholki frustumu
	BoundingSphere	m_frustumSphere;
	BoundingCone	m_frustumCone;

	void calculateViewMat();
	void calculateProjectionMat();
	void buildViewFrustum();
public:
	PerspectiveCamera();
	PerspectiveCamera(const Vec3& Position, const Vec3& LookAtPou32);
	void			rotateBy(const Vec3& angles);
	void			lookAt(const Vec3& pou32);
	void			setPosition(const Vec3& position);
	void			setFOV(const f32 &fov){ m_FOV = fov;}
	f32				getFOV() { return m_FOV; }
	void			update();
	Mat*			getViewProjectionMat();
	Mat*			getViewMat();
	Mat*			getProjectionMat() { return &m_projectionMat; }
	const Vec3*		getPosition() const;
	const Vec4*		getPositionV4() const;
	Vec3*			getViewDirection();
	Vec4*			getViewDirectionV4();
	const Vec4*		getViewUp() const;
	const Vec4*		getViewLeft() const;
	const BoundingSphere*	getFrustumSphere() const { return &m_frustumSphere; }
	const BoundingCone*		getFrustumCone() const { return &m_frustumCone; }
	const Plane*			getFrustumPlanes() const { return m_frustum; }
	const Vec3*				getFrustumVertices() const { return m_frustumVertex; }
};

/////////////////////////////////////////////////////////////////////////

class XGINE_API FreeCamera : public PerspectiveCamera
{
public:
	FreeCamera();
	FreeCamera(const Vec3& Position, const Vec3& LookAtPou32);

	void moveBy(const Vec3& vector);
	void moveByRel(const Vec3& vector);
};
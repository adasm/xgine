#include "Camera.h"

PerspectiveCamera::PerspectiveCamera() : m_updateViewMat(true), c_up(0, 1, 0), m_position(0, 0, 0), m_look(0, 0, 1), m_FOV(D3DX_PI/5.0f)
{
	calculateProjectionMat();
	calculateViewMat();
}

PerspectiveCamera::PerspectiveCamera(const Vec3& Position, const Vec3& LookAtPou32) : m_updateViewMat(true), c_up(0, 1, 0),
	m_position(Position), m_look(LookAtPou32 - Position), m_FOV(D3DX_PI/5.0f)
{
	D3DXVec3Normalize(&m_look, &m_look);
	calculateProjectionMat();
	calculateViewMat();
}

void PerspectiveCamera::rotateBy(const Vec3& angles)
{
	Mat mR;

	D3DXMatrixRotationAxis(&mR, &c_up, angles.y);
	D3DXVec3TransformCoord(&m_look, &m_look, &mR);
	D3DXVec3TransformCoord(&m_left, &m_left, &mR);

	D3DXMatrixRotationAxis(&mR, &m_left, angles.x);
	D3DXVec3TransformCoord(&m_look, &m_look, &mR);

	m_updateViewMat = true;
}

void PerspectiveCamera::lookAt(const Vec3& pou32)
{
	D3DXVec3Normalize(&m_look, &(pou32 - m_position));
	m_updateViewMat = true;
}

void PerspectiveCamera::setPosition(const Vec3& position)
{
	m_position = position;
	m_updateViewMat = true;
}

void PerspectiveCamera::update()
{
	calculateProjectionMat();
	calculateViewMat();
}

Mat* PerspectiveCamera::getViewProjectionMat()
{
	return &m_viewProjectionMat;
}

Mat* PerspectiveCamera::getViewMat()
{
	return &m_viewMat;
}

const Vec3* PerspectiveCamera::getPosition() const
{
	return &m_position;
}

const Vec4* PerspectiveCamera::getPositionV4() const
{
	return &m_positionV4;
}

Vec3* PerspectiveCamera::getViewDirection()
{
	return &m_look;
}

Vec4* PerspectiveCamera::getViewDirectionV4()
{
	return &m_lookV4;
}

const Vec4* PerspectiveCamera::getViewUp() const
{
	return &m_upV4;
}

const Vec4* PerspectiveCamera::getViewLeft() const
{
	return &m_leftV4;
}

void PerspectiveCamera::calculateViewMat()
{
	if(m_updateViewMat)
	{
		D3DXVec3Cross(&m_left, &c_up, &m_look);
		D3DXVec3Normalize(&m_left, &m_left);

		D3DXVec3Cross(&m_up, &m_look, &m_left);
		D3DXVec3Normalize(&m_left, &m_left);

		m_lookV4 = Vec4(m_look, 1);
		m_positionV4 = Vec4(m_position, 1);
		m_upV4 = Vec4(m_up, 0);
		m_leftV4 = Vec4(m_left, 0);

		Vec3 LookAt = m_position + m_look;
		D3DXMatrixLookAtRH(&m_viewMat, &m_position, &LookAt, &m_up);
		D3DXMatrixMultiply(&m_viewProjectionMat, &m_viewMat, &m_projectionMat);
		buildViewFrustum();
		m_updateViewMat = false;
	}
}

void PerspectiveCamera::calculateProjectionMat()
{
	f32 aspect = gEngine.device->getWidth() / (f32)gEngine.device->getHeight();
	m_zNear = r_cameraNearZ;
	m_zFar = r_cameraFarZ;
	D3DXMatrixPerspectiveFovRH(&m_projectionMat, m_FOV, aspect, m_zNear, m_zFar);

	//calculate sizes of frustum
	f32 t = (f32)tanf(m_FOV * 0.5f);
	m_farHeight = m_zFar * t;
	m_farWidth = aspect * m_farHeight;
	m_nearHeight = m_zNear * t;
	m_nearWidth = aspect * m_nearHeight;

	//calculate frustum sphere radius
	m_frustumCenterZ = (m_zFar + m_zNear) * 0.5f;
	m_frustumSphere.Radius = D3DXVec3Length(&Vec3(m_farWidth, m_farHeight, (m_zFar - m_zNear) * 0.5f));

	//frustum cone
	m_frustumCone.setAngle(m_FOV * (1 / aspect));
}

void PerspectiveCamera::buildViewFrustum()
{
	// Calculate frustum planes
	// Near plane
	m_frustum[0].a = m_viewProjectionMat._13;
	m_frustum[0].b = m_viewProjectionMat._23;
	m_frustum[0].c = m_viewProjectionMat._33;
	m_frustum[0].d = m_viewProjectionMat._43;
	D3DXPlaneNormalize(&(m_frustum[0]), &(m_frustum[0]));

	// Far plane
	m_frustum[1].a = m_viewProjectionMat._14 - m_viewProjectionMat._13;
	m_frustum[1].b = m_viewProjectionMat._24 - m_viewProjectionMat._23;
	m_frustum[1].c = m_viewProjectionMat._34 - m_viewProjectionMat._33;
	m_frustum[1].d = m_viewProjectionMat._44 - m_viewProjectionMat._43;
	D3DXPlaneNormalize(&(m_frustum[1]), &(m_frustum[1]));

	// Left plane
	m_frustum[2].a = m_viewProjectionMat._14 + m_viewProjectionMat._11;
	m_frustum[2].b = m_viewProjectionMat._24 + m_viewProjectionMat._21;
	m_frustum[2].c = m_viewProjectionMat._34 + m_viewProjectionMat._31;
	m_frustum[2].d = m_viewProjectionMat._44 + m_viewProjectionMat._41;
	D3DXPlaneNormalize(&(m_frustum[2]), &(m_frustum[2]));

	// Right plane
	m_frustum[3].a = m_viewProjectionMat._14 - m_viewProjectionMat._11;
	m_frustum[3].b = m_viewProjectionMat._24 - m_viewProjectionMat._21;
	m_frustum[3].c = m_viewProjectionMat._34 - m_viewProjectionMat._31;
	m_frustum[3].d = m_viewProjectionMat._44 - m_viewProjectionMat._41;
	D3DXPlaneNormalize(&(m_frustum[3]), &(m_frustum[3]));

	// Top plane
	m_frustum[4].a = m_viewProjectionMat._14 - m_viewProjectionMat._12;
	m_frustum[4].b = m_viewProjectionMat._24 - m_viewProjectionMat._22;
	m_frustum[4].c = m_viewProjectionMat._34 - m_viewProjectionMat._32;
	m_frustum[4].d = m_viewProjectionMat._44 - m_viewProjectionMat._42;
	D3DXPlaneNormalize(&(m_frustum[4]), &(m_frustum[4]));

	// Bottom plane
	m_frustum[5].a = m_viewProjectionMat._14 + m_viewProjectionMat._12;
	m_frustum[5].b = m_viewProjectionMat._24 + m_viewProjectionMat._22;
	m_frustum[5].c = m_viewProjectionMat._34 + m_viewProjectionMat._32;
	m_frustum[5].d = m_viewProjectionMat._44 + m_viewProjectionMat._42;
	D3DXPlaneNormalize(&(m_frustum[5]), &(m_frustum[5]));

	// Calculate frustum sphere center
	m_frustumSphere.Center = m_position + (m_look * m_frustumCenterZ);

	// Calculate frustum cone
	m_frustumCone.Axis = m_look;
	m_frustumCone.Vertex = m_position;

	// Calcluate frustum vertices
	m_frustumVertex[0] = m_position + (m_look * m_zNear) + (m_left * m_nearWidth) - (m_up * m_nearHeight);
	m_frustumVertex[1] = m_position + (m_look * m_zNear) - (m_left * m_nearWidth) - (m_up * m_nearHeight);
	m_frustumVertex[2] = m_position + (m_look * m_zNear) + (m_left * m_nearWidth) + (m_up * m_nearHeight);
	m_frustumVertex[3] = m_position + (m_look * m_zNear) - (m_left * m_nearWidth) + (m_up * m_nearHeight);
	m_frustumVertex[4] = m_position + (m_look * m_zFar) + (m_left * m_farWidth) - (m_up * m_farHeight);
	m_frustumVertex[5] = m_position + (m_look * m_zFar) - (m_left * m_farWidth) - (m_up * m_farHeight);
	m_frustumVertex[6] = m_position + (m_look * m_zFar) + (m_left * m_farWidth) + (m_up * m_farHeight);
	m_frustumVertex[7] = m_position + (m_look * m_zFar) - (m_left * m_farWidth) + (m_up * m_farHeight);
}

/////////////////////////////////////////////////////////////////////////

FreeCamera::FreeCamera() : PerspectiveCamera()
{ }

FreeCamera::FreeCamera(const Vec3& Position, const Vec3& LookAtPou32) : PerspectiveCamera(Position, LookAtPou32)
{ }

void FreeCamera::moveBy(const Vec3& vector)
{
	m_position += vector;
	m_updateViewMat = true;
}

void FreeCamera::moveByRel(const Vec3& vector)
{
	m_position += vector.x * m_left
		+ vector.y * m_up
		+ vector.z * m_look;
	m_updateViewMat = true;
}
#include "Light.h"



Light::Light(LIGHT_TYPE type, u32 castsShadows, const Vec4& color, const Vec3& position, f32 range, const Vec3& direction, f32 phi)
	:	m_type(type), m_needsUpdate(true), m_color(0), m_position(0), m_direction(0), m_range(range), m_invRange(0), m_phi(0), m_cosPhiHalf(0),
		m_viewProjection(0), m_cone(0), m_box(new BoundingBox()), m_coneCappingNum(0), m_castsShadows(castsShadows), lpVolumeMesh(0), m_depthMap(0),
		m_enabled(1)
{
	if(range == 0.0f)
		range = 0.1f;

	m_color = new Vec4(color.x, color.y, color.z, color.w);

	if(type == LIGHT_POINT)
	{
		m_position = new Vec3(position);
		m_positionV4 = new Vec4(position, 1);
		m_invRange = 1.0f / range;
		calculateBoundingBox(0);
		LPD3DXBUFFER buffer;
		if(FAILED(D3DXCreateSphere(gEngine.device->getDev(), m_range * 1.05f, 16, 16, &lpVolumeMesh, &buffer)))
			lpVolumeMesh = 0;
		buffer->Release();
	}
	else if(type == LIGHT_SPOT)
	{
		m_position = new Vec3(position);
		m_positionV4 = new Vec4(position, 1);
		m_direction = new Vec3();
		D3DXVec3Normalize(m_direction, &direction);
		m_invRange = 1.0f / range;
		m_phi = phi;
		m_cosPhiHalf = cosf(phi * 0.5f);
		m_viewProjection = new Mat();
		m_view = new Mat();
		m_cone = new BoundingCone();
		
		calculateBoundingBox(0);
		calculateBoundingCone();

		LPD3DXBUFFER buffer;
		if(FAILED(D3DXCreateCylinder(gEngine.device->getDev(), 0.0f, m_range * 1.1f * tan(m_phi/2), m_range * 1.05f, 10, 1, &lpVolumeMesh, &buffer)))
			lpVolumeMesh = 0;
		buffer->Release();

		if(m_castsShadows)
		{
			RenderTexture::CreationParams cp;
			cp.width = cp.height = r_shadowMapSpotSize;
			cp.colorFormat = D3DFMT_R32F;
			cp.depthFormat = D3DFMT_D24S8;

			m_depthMap = new RenderTexture();
			if(!m_depthMap->init(cp))
			{
				gEngine.kernel->log->prn(LT_ERROR, "Light", "Could not initialize ShadowDepth. Couldn not initialize RenderTexture.");
				delete m_depthMap;
				m_depthMap = 0;
				m_castsShadows = 0;
			}
			
			/*cp.colorFormat = D3DFMT_G32R32F;
			m_VdepthMap = new RenderTexture();
			if(!m_VdepthMap->init(cp))
			{
				gEngine.kernel->log->prn("Could not initialize VarianceShadowDepth. Couldn not initialize RenderTexture.");
				delete m_VdepthMap;
				m_VdepthMap = 0;
				m_castsShadows = 0;
			}*/
		}
	}
	else if(type == LIGHT_DIR)
	{
		m_position = new Vec3(0,0,0);
		m_positionV4 = new Vec4(0,0,0, 1);
		m_direction = new Vec3(direction);
		D3DXVec3Normalize(m_direction, &direction);
		m_viewProjection = new Mat();
		m_view = new Mat();

		if(m_castsShadows)
		{
			RenderTexture::CreationParams cp;
			cp.width = cp.height = r_shadowMapDirSize;
			cp.colorFormat = D3DFMT_R32F;
			cp.depthFormat = D3DFMT_D24S8;

			m_depthMap = new RenderTexture();
			if(!m_depthMap->init(cp))
			{
				gEngine.kernel->log->prn(LT_ERROR, "Light", "Could not initialize ShadowDepth. Couldn not initialize RenderTexture.");
				delete m_depthMap;
				m_depthMap = 0;
				m_castsShadows = 0;
			}
			
			/*cp.colorFormat = D3DFMT_G32R32F;
			m_VdepthMap = new RenderTexture();
			if(!m_VdepthMap->init(cp))
			{
				gEngine.kernel->log->prn("Could not initialize VarianceShadowDepth. Couldn not initialize RenderTexture.");
				delete m_VdepthMap;
				m_VdepthMap = 0;
				m_castsShadows = 0;
			}*/
		}
	}
}

Light::~Light()
{
	DXRELEASE(lpVolumeMesh);
	delete m_color;
	delete m_positionV4;
	delete m_position;
	delete m_direction;
	delete m_viewProjection;
	delete m_cone;
	delete m_box;
}

void Light::setPos(const Vec3* position)
{
	if(m_type == LIGHT_POINT || m_type == LIGHT_SPOT)
	{
		*m_position = *position;
		*m_positionV4 = Vec4(*position, 1);
		m_needsUpdate = true;
	}
}

void Light::setPos(const Vec4* position)
{
	if(m_type == LIGHT_POINT || m_type == LIGHT_SPOT)
	{
		*m_positionV4 = *position;
		*m_position = *((Vec3*)position);
		m_needsUpdate = true;
	}
}

void Light::setDir(const Vec3* direction)
{
	if(m_type == LIGHT_SPOT || m_type == LIGHT_DIR)
	{
		D3DXVec3Normalize(m_direction, direction);
		m_needsUpdate = true;
	}
}

void Light::update(const ICamera *camera)
{
	calculateConeCapping(camera);

	if(m_needsUpdate || m_type == LIGHT_DIR)
	{
		calculateBoundingBox(camera->getFrustumVertices());
	}

	if(m_needsUpdate)
	{
		m_needsUpdate = false;
		if(m_type == LIGHT_SPOT)
		{
			calculateViewProjectionMatrix();
			calculateBoundingCone();
		}
	}
}

void Light::calculateConeCapping(const ICamera* camera)
{
	m_coneCappingNum = 0;
	if(m_type != LIGHT_DIR) //POINT, SPOT
	{
		for(int i = 0; i < 6; i++)
			if(camera->getFrustumPlanes()[i].a * m_position->x + camera->getFrustumPlanes()[i].b * m_position->y
				+ camera->getFrustumPlanes()[i].c * m_position->z + camera->getFrustumPlanes()[i].d > 0)
				m_coneCapping[m_coneCappingNum++] = camera->getFrustumPlanes()[i];
	}
	else //DIRECTIONAL
	{
		Vec3 cmpPos(*camera->getPosition() - *m_direction);
		for(int i = 0; i < 6; i++)
			if(camera->getFrustumPlanes()[i].a * cmpPos.x + camera->getFrustumPlanes()[i].b * cmpPos.y
				+ camera->getFrustumPlanes()[i].c * cmpPos.z + camera->getFrustumPlanes()[i].d > 0)
				m_coneCapping[m_coneCappingNum++] = camera->getFrustumPlanes()[i];
	}
}

void Light::calculateViewProjectionMatrix()
{
	if(m_type == LIGHT_SPOT)
	{
		D3DXMATRIX lightProj;
		Vec3 lookAt = *m_position + (*m_direction)*m_range;
		D3DXMatrixLookAtRH(m_view, m_position, &lookAt, &Vec3(0, 1, 0));
		D3DXMatrixPerspectiveFovRH( &lightProj,  D3DX_PI/2, 1.0f, 1.0f, m_range + 5.0f);
		D3DXMatrixMultiply(m_viewProjection, m_view, &lightProj);
	}
}

void Light::calculateBoundingCone()
{
	if(m_type == LIGHT_SPOT)
	{
		m_cone->setVertex(m_position);
		m_cone->setAxis(m_direction);
		m_cone->setAngle(m_phi);
	}
}

void Light::calculateBoundingBox(const Vec3* frustumVerts)
{
	if(m_type == LIGHT_POINT || m_type == LIGHT_SPOT)
	{
		Vec3 v(m_range, m_range, m_range);
		m_box->reset(Vec3(*m_position - v), Vec3(*m_position + v));

	}
	else //DIRECTIONAL
	{
		Vec3 min = frustumVerts[0];
		Vec3 max = frustumVerts[0];
		for(int i = 1; i < 8; ++i)
		{
			if(frustumVerts[i].x < min.x)
				min.x = frustumVerts[i].x;
			else if(frustumVerts[i].x > max.x)
				max.x = frustumVerts[i].x;

			if(frustumVerts[i].y < min.y)
				min.y = frustumVerts[i].y;
			else if(frustumVerts[i].y > max.y)
				max.y = frustumVerts[i].y;

			if(frustumVerts[i].z < min.z)
				min.z = frustumVerts[i].z;
			else if(frustumVerts[i].z > max.z)
				max.z = frustumVerts[i].z;
		}
		//extrude bb toward against light dir
		if(m_direction->x > 0)
			min.x -= m_direction->x * m_range;
		else
			max.x -= m_direction->x * m_range;
		if(m_direction->y > 0)
			min.y -= m_direction->y * m_range;
		else
			max.y -= m_direction->y * m_range;
		if(m_direction->z > 0)
			min.z -= m_direction->z * m_range;
		else
			max.z -= m_direction->z * m_range;

		m_box->reset(min, max);
	}
}

void Light::renderVolume()
{
	if(lpVolumeMesh)
	{
		static Mat world;
		if(m_type == LIGHT_POINT)
		{
			worldMat(&world, *m_position);
		}
		else if(m_type == LIGHT_SPOT)
		{
			Vec3 axis;
			f32	 angle;
			Mat  rot;
			D3DXVec3Cross(&axis, &Vec3(0,0,1), m_direction);
			angle = acos(D3DXVec3Dot(&Vec3(0,0,1), m_direction));
			D3DXMatrixRotationAxis(&rot, &axis, angle);
			Vec3 pos = *m_position + *m_direction * (m_range/2);
			worldMat(&world, pos);
			world = rot * world;
		}
		gEngine.renderer->setWorld(&world);
		lpVolumeMesh->DrawSubset(0);
	}
}

void Light::renderDepth(RenderList* rl)
{
	if(m_type == LIGHT_DIR)
	{
		m_phi = D3DX_PI/4;

		ICamera *cam = gEngine.renderer->get().camera;
		Vec3 dir;
		D3DXVec3Normalize(&dir, &Vec3(cam->getViewDirection()->x, 0, cam->getViewDirection()->z));
		Vec3 pos = *cam->getPosition() + dir * (g_shadowMapDirViewVolume)/2;
		Vec3 lookAt = pos;
		pos = pos - (*m_direction) * g_shadowMapDirViewDist;

		/*pos /= 5.0f;
		pos = Vec3((i32)pos.x * (i32)5,(i32)pos.y * (i32)5,(i32)pos.z * (i32)5);

		lookAt = pos + (*m_direction) * g_shadowMapDirViewDist;*/

		*m_position = pos;
		*m_positionV4 = Vec4(*m_position, 1);
		D3DXMATRIX lightProj;
		D3DXMatrixLookAtRH(m_view, &pos, &lookAt, &Vec3(0, 1, 0));
		D3DXMatrixOrthoRH(&lightProj, g_shadowMapDirViewVolume, g_shadowMapDirViewVolume, g_shadowMapDirNearZ, g_shadowMapDirFarZ);
		//D3DXMatrixPerspectiveFovRH( &lightProj,  D3DX_PI/3, 1.0f, g_shadowMapDirNearZ, g_shadowMapDirFarZ);
		D3DXMatrixMultiply(m_viewProjection, m_view, &lightProj);
	}

	if(m_type == LIGHT_SPOT || m_type == LIGHT_DIR)
		gEngine.renderer->sm->renderDepth(getDepthMap(), rl, this);
}
#pragma once
#include "XGine.h"

struct XGINE_API HQWater
{
	HQWater(f32 H);
	~HQWater();

	void	update(ICamera* cam);
	u32		render();

	void	setSunLight(Light *light) { g_sunLight = light; }

protected:
	u32								m_updatedNormalMapType;
	u32								m_mustUpdateNormalMap;
	u32								m_mustUpdateDetailNormalMap;
	void							updateNormalMap();
	void							updateDetailNormalMap();
	RenderTexture*					m_normalMap;
	RenderTexture*					m_detailNormalMap;
	Texture*						m_hmap;
	Texture*						m_hmap2;
	Texture*						m_dhmap;
	Texture*						m_fresnel;
	Texture*						m_foam;
	Shader*							m_shader;
	f32								m_waterY;
	Light*							g_sunLight;
	Mesh*							mesh;
	Mesh*							meshMed;
	Mesh*							meshLow;
	Mat								meshWorld;
	Mat								rot;

	Mat								world;
	Mat								invWorld;
	Vec3							position;
	Vec3							rotation;
	BoundingBox						boundingBox;
};
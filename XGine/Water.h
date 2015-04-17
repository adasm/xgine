#pragma once
#include "XGine.h"

struct XGINE_API Water
{
	Water(f32 H);
	~Water();

	void	update(ICamera* cam);
	u32		render();

	void			setSunLight(Light *light) { g_sunLight = light; }

protected:
	LPDIRECT3DVERTEXBUFFER9			m_vb;
	LPDIRECT3DINDEXBUFFER9			m_ib;
	LPDIRECT3DTEXTURE9				m_backBuffer;
	u32								m_vbSize;
	u32								m_ibSize;
	Texture*						m_texture1;
	Texture*						m_texture2;
	Texture*						m_texture3;
	Shader*							m_shader;
	f32								m_waterY;
	Light*							g_sunLight;

	Mat					world;
	Mat					invWorld;
	Vec3				position;
	Vec3				rotation;
	BoundingBox			boundingBox;
};
#pragma once
#include "XGine.h"

struct XGINE_API Terrain : Node<Terrain>
{
	Terrain(TerrainVertex *vertices, LPDIRECT3DTEXTURE9 indTex, u32 secX, u32 secY, u32 sectorSize, u32 hmSize, f32 texHmGeoStep);
	~Terrain();

							void	update(ICamera* cam);
							u32		renderRaw();
							u32		renderForward(Light* light);
							u32		renderDeferred();

	u32								m_sectorSize;
	u32								m_lod;
	u32								m_lodGeo;
	Terrain*						m_top;
	Terrain*						m_left;
	Terrain*						m_right;
	Terrain*						m_bottom;

	LPDIRECT3DTEXTURE9				m_indexTexture;
	Texture*						m_colorTexture[4];

	LPDIRECT3DVERTEXBUFFER9			*m_vb;
	LPDIRECT3DINDEXBUFFER9			*m_ibCenter, *m_ibBorder;
	u32								*m_vbSize;
	u32								*m_ibCenterSize;
	u32								*m_ibBorderSize;
	u32								*m_borderTopLod;
	u32								*m_borderLeftLod;
	u32								*m_borderRightLod;
	u32								*m_borderBottomLod;
	u32								m_levels;
	void							updateBorderLod();
};
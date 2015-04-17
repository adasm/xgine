#pragma once
#include "XGine.h"

struct TerrainVertex
{
	Vec3	pos;
	Vec2	texCoord;
	Vec3	normal;
	Vec3	tangent;
	Vec3	binormal;
};

struct XGINE_API TerrainManager
{
	TerrainManager(Scene* scene = 0, u32 hmGeoStep = 16, u32 indTexExp = 8);
	~TerrainManager();

	void cleanup();

	u32 create(const string& texHmName,  const string& texIndName, u32 size, Vec3 pos = Vec3(0,0,0));

	u32 save(const string& hmPackName);
	u32 saveIndTex(const string& texIndName);
	u32 load(const string& hmPackName,  const string& texIndName);
	u32	loadHm(const string& hmPackName);
	u32	loadIndTex(const string& texIndName);
	u32 updateBinLoad(const string& texHmName,  const string& texIndName, const string& binHmName, const string& binIndName, u32 size, Vec3 pos = Vec3(0,0,0));

	u32 saveHmAsXMesh(const string& fileName);

	void update(ICamera *cam);

	vector< vector< Terrain* > >	m_terrains;
	u32								m_sectorSize;
	Vec3							m_position;
	u32								m_size;
	TerrainVertex*					m_vertices;
	u32*							m_indices;
	u32								m_numVertices;
	u32								m_numIndices;
	u32								m_hmGeoStep;
	u32								m_indTexExp;

	//slower (whole data copy)
	//inline void setIndexTextureDataTexel(u32 x, u32 y, Vec4* color) { if(!m_indexTextureData)return; m_indexTextureData[y*m_indexTextureSize+x] = D3DCOLOR_ARGB((u32)color->w*255, (u32)color->x*255, (u32)color->y*255, (u32)color->z*255); }
	u32 updateIndexTexture();

	//faster
	void lockIndexTexture(Vec4 rect);
	void unlockIndexTexture(Vec4 rect);
	void setIndexTextureTexel(f32 x, f32 y, Vec4* color);
	
protected:
	Scene*	m_scene;
	u32		m_hmInit;

	LPDIRECT3DTEXTURE9				*m_indexTextures;
	u32								*m_indexTexturesLocked;
	D3DLOCKED_RECT					*m_indexTexturesLockedRect;

	u32*							m_indexTextureData;
	u32								m_indexTextureSize;
};
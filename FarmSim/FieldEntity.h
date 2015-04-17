#pragma once
#include "Headers.h"
#include "TreeParams.h"

/*
struct FieldSector
{
	GrassMesh *field;
	int *terrainIndex;
	Vec3 position;
	bool untouched;
	bool *active;
	FieldSector()
	{
		untouched = true;
	}
	FieldSector(int *terrainindex, Vec3 pos, GrassMesh *Field)
	{
		field = Field;
		position = pos;
		terrainIndex = terrainindex;
		untouched = true;
	}
};*/

struct FieldEntity : IEntity
{
	FieldEntity(Vec3 position, int width, int height, FieldSectorPTR* sectors, int secNum);
	~FieldEntity();

	void	update(ICamera* cam);
	u32		renderDeferred();
	u32		renderForward(Light* light = 0);
	u32		renderRaw();

		   u32			rendEnabled();
	inline u32			selfRendering() { return 1; }
	inline Mesh*		getMesh()		{ return 0; }
	inline Material*	getMaterial()	{ return 0; }
		   Shader*		getShader();
		   Texture*		getTexture();
	inline u32			hasShadow()		{ return 0; }
		   REND_TYPE	getRendType();
		   
	void				setSunLight(Light *light) { g_sunLight = light; }
	void				sectorChangeNotification() { m_regenerateRenderList = true; }

protected:
	Light*			g_sunLight;

	FieldSectorPTR*	m_sectors;
	int				m_width;
	int				m_height;
	int				m_sectorNum;

	bool			m_isEmpty;
	bool			m_regenerateRenderList;
	vector<int>		m_renderList;
};
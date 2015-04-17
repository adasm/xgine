#pragma once
#include "Headers.h"


struct ColorG16R16
{
	D3DXFLOAT16 g;
	D3DXFLOAT16 r;
	ColorG16R16(float G, float R)
	{
		D3DXFloat32To16Array(&g, &G, 1);
		D3DXFloat32To16Array(&r, &R, 1);
	}
};
struct ColorG32R32F
{
	float g;
	float r;
	ColorG32R32F(float G, float R)
	{
		g = G;
		r = R;
	}
};

typedef class FieldSector
{
public:
	FieldSector(Vec3 WorldPos, int posX, int posY);

	D3DXFLOAT16*		getHeights()				{ return m_heights; }
	float				getHeight(int x, int y);
	LPDIRECT3DTEXTURE9	getHeightMap()				{ if(m_dumpedHM) { generateHeights(); generateHMap(); m_dumpedHM = false; } return m_heightMap; }
	LPDIRECT3DTEXTURE9	getGrassTypeMap()			{ if(m_dumpedGrass) { generateGrassMap(); m_dumpedGrass = false; } return m_grassTypeMap; }

	void				setTimeCounter(DWORD time)	{ m_timeCounter = time; }
	DWORD				getTimeCounter()			{ return m_timeCounter; }

	Vec3				getWorldPos()				{ return m_worldPos; }

	bool				isEmpty()					{ return m_isEmpty; }

	void				newTurn();

	void				dumpData();

	void				setField(int x, int y, Field field);
	void				getField(int x, int y, Field *field);

	void				setFieldEntity(FieldEntity* fieldEntity) { m_fieldEntity = fieldEntity; }

protected:
	void				generateFieldMap();
	void				cleanupFieldMap();
	void				generateHMap();
	void				generateHeights();
	void				generateGrassMap();
	bool				checkIsEmpty();

	bool				m_isEmpty;
	int					m_posX;
	int					m_posY;
	LPDIRECT3DTEXTURE9	m_heightMap;
	LPDIRECT3DTEXTURE9	m_grassTypeMap;
	Vec3				m_worldPos;
	D3DXFLOAT16*		m_heights;

	bool				m_dumpedGrass;
	bool				m_dumpedHM;
	DWORD				m_timeCounter;
	Field*				m_fieldMap;
	FieldEntity*		m_fieldEntity;
}*FieldSectorPTR;
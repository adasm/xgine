#pragma once
#include "Headers.h"

struct GrassVertex
{
	Vec3	pos;
	Vec2	uv;
	Vec2	speedModifier;
	Vec2	dispMapCoord;
	Vec3	normal;
};

#define GRASSVERT_FVF (D3DFVF_XYZ | D3DFVF_TEX2)

struct GrassMesh
{
public:
	
	void render();
	void renderSV();

	void reassign(Vec3 *pos, Vec4 uvs, Vec2 grassBladeDimms, int grassBladesNum, int arraySize);

	void deleteMesh();
	void recreate();

	inline Vec2 getGrassBladeDimm()		{ return m_grassBladeDimm; }
	inline int getGrassBladeNum()			{ return m_grassBladeNum; }


protected:
	friend ResMgr;
	friend GrassManager;
	friend FieldEntity;

	~GrassMesh();
	GrassMesh();
	GrassMesh(Vec3 *pos, Vec4 uvs, Vec2 grassBladeDimm, int grassBladesNum, int arraySize, bool generateMesh = true);
	void							generateStats(int grassBladesNum, int grassNum);
	void							release();
	u32								load();

	//LPD3DXMESH 						m_pMesh;
	//DWORD							m_FVF;
	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDeclaration;
	IDirect3DIndexBuffer9*			m_IB;
	IDirect3DVertexBuffer9*			m_VB;

	Vec2							m_grassBladeDimm;
	int								m_grassBladeNum;

	GrassVertex*					m_vertices;
	WORD*							m_indices;

	Vec3*							m_grassPositions;
	Vec4							m_uvs;
	int								m_grassNum;
	int								m_verticesNum;
	int								m_indicesNum;
	int								m_facesNum;
};
#include "GrassMesh.h"

GrassMesh::GrassMesh()
{
	m_vertices = NULL;
	m_indices = NULL;
	m_VB = NULL;
	m_IB = NULL;
}

GrassMesh::~GrassMesh()
{
	release();
}

void GrassMesh::release()
{
	delete(m_uvs);
	delete(m_uvs);
	m_VB->Release();
	m_IB->Release();
}

u32 GrassMesh::load()
{
	return false;
}

void GrassMesh::render()
{
	if(!m_grassNum)
	return;
	if(!m_VB || !m_IB)
	recreate();
	gEngine.device->getDev()->SetVertexDeclaration(m_pVertexDeclaration);
	gEngine.device->getDev()->SetStreamSource(0, m_VB, 0, sizeof(GrassVertex));
	gEngine.device->getDev()->SetIndices(m_IB);
	//gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_grassBladeNum * m_grassNum * 6, 0, (UINT)((m_grassNum * 2 * 3 * m_grassBladeNum)/3));
	gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_verticesNum, 0, m_facesNum);
	//gEngine.device->getDev()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_facesNum);
	gEngine.renderer->addRenderedCounts(m_verticesNum, m_facesNum);
}

void GrassMesh::renderSV()
{
}

GrassMesh::GrassMesh(Vec3 *positions, Vec4 uvs, Vec2 grassBladeDimm, int grassBladesNum, int arraySize, bool generateMesh)
{
	generateStats(grassBladesNum, arraySize);
	m_uvs = uvs;
	m_grassBladeDimm = grassBladeDimm;
	float angle = 0;
	m_grassPositions = positions;
	Mat rot;
	//arraySize = 3;

	m_vertices                                      = NULL;
	m_indices                                       = NULL;

	if(generateMesh && m_grassNum)
	{
		const D3DVERTEXELEMENT9 vertexDecl[] =
		{
			{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,  0 },
			{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
			{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
			{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
			D3DDECL_END()
		};
		gEngine.device->getDev()->CreateVertexDeclaration(vertexDecl, &m_pVertexDeclaration);
		//MessageBox(0, "SZAJSE", "D", MB_OK);
		if(gEngine.device->getDev()->CreateIndexBuffer(m_indicesNum * sizeof(WORD), D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_IB, NULL) != D3D_OK)
		{
			m_IB = NULL;
			return;
		}
		if(gEngine.device->getDev()->CreateVertexBuffer(m_verticesNum * sizeof(GrassVertex), D3DUSAGE_DYNAMIC, GRASSVERT_FVF, D3DPOOL_DEFAULT, &m_VB, NULL) != D3D_OK)
		{
			m_VB = NULL;
			return;
		}
		m_VB->Lock(0, 0, (void**)&m_vertices, D3DLOCK_DISCARD);
		m_IB->Lock(0, 0, (void**)&m_indices, D3DLOCK_DISCARD);

		int modulo = 3.14f*100;
		time_t t;
		srand((unsigned) time(&t));
		int x = 0;
		int y = 0;
		//gEngine.kernel->log->prnEx("UV: %f %f %f %f", m_uvs.x, m_uvs.y, m_uvs.z, m_uvs.w);
		bool minus = true;
		for(int i = 0; i < m_grassNum; i++)
		{
			Vec2 speedModifier = Vec2((rand()%100)/100.0f, (rand()%100)/100.0f);
			angle += rand()%modulo/100.0f;
			//gEngine.kernel->log->prnEx("SpeedModifier: %f %f", speedModifier.x, speedModifier.y);
			if(angle > 1000)
			angle = 0;
			for(int z = 0; z < m_grassBladeNum; z++)
			{
				Mat rot;
				D3DXMatrixRotationY(&rot, angle);
				Vec3 tpos = Vec3(-0.5f * grassBladeDimm.x, grassBladeDimm.y, 0.0f);
				D3DXVec3TransformCoord(&tpos, &tpos, &rot);
				m_vertices[i*m_grassBladeNum*4+z*4+0].pos = tpos + positions[i];
				m_vertices[i*m_grassBladeNum*4+z*4+0].uv = Vec2(m_uvs.x, m_uvs.y);
				m_vertices[i*m_grassBladeNum*4+z*4+0].normal = Vec3(0, 1, 0);

				m_vertices[i*m_grassBladeNum*4+z*4+0].speedModifier = speedModifier;
				if(minus)
				m_vertices[i*m_grassBladeNum*4+z*4+0].speedModifier = -speedModifier;
				m_vertices[i*m_grassBladeNum*4+z*4+0].dispMapCoord = Vec2(x, y);

				tpos = Vec3(0.5f * grassBladeDimm.x, grassBladeDimm.y, 0.0f);
				D3DXVec3TransformCoord(&tpos, &tpos, &rot);
				m_vertices[i*m_grassBladeNum*4+z*4+1].pos = tpos + positions[i];
				m_vertices[i*m_grassBladeNum*4+z*4+1].uv = Vec2(m_uvs.z, m_uvs.y);
				m_vertices[i*m_grassBladeNum*4+z*4+1].normal = Vec3(0, 1, 0);

				m_vertices[i*m_grassBladeNum*4+z*4+1].speedModifier = speedModifier;
				if(minus)
				m_vertices[i*m_grassBladeNum*4+z*4+1].speedModifier = -speedModifier;
				m_vertices[i*m_grassBladeNum*4+z*4+1].dispMapCoord = Vec2(x, y);

				tpos = Vec3(0.5f * grassBladeDimm.x, 0.0f, 0.0f);
				D3DXVec3TransformCoord(&tpos, &tpos, &rot);
				m_vertices[i*m_grassBladeNum*4+z*4+2].pos = tpos + positions[i];
				m_vertices[i*m_grassBladeNum*4+z*4+2].uv = Vec2(m_uvs.z, m_uvs.w);
				m_vertices[i*m_grassBladeNum*4+z*4+2].speedModifier = Vec2(0, 0);
				m_vertices[i*m_grassBladeNum*4+z*4+2].dispMapCoord = Vec2(x, y);
				m_vertices[i*m_grassBladeNum*4+z*4+0].normal = Vec3(0, 1, 0);

				tpos = Vec3(-0.5f * grassBladeDimm.x, 0.0f, 0.0f);
				D3DXVec3TransformCoord(&tpos, &tpos, &rot);
				m_vertices[i*m_grassBladeNum*4+z*4+3].pos = tpos + positions[i];
				m_vertices[i*m_grassBladeNum*4+z*4+3].uv = Vec2(m_uvs.x, m_uvs.w);
				m_vertices[i*m_grassBladeNum*4+z*4+3].speedModifier = Vec2(0, 0);
				m_vertices[i*m_grassBladeNum*4+z*4+3].dispMapCoord = Vec2(x, y);
				m_vertices[i*m_grassBladeNum*4+z*4+0].normal = Vec3(0, 1, 0);

				m_indices[i*m_grassBladeNum*2*3+z*2*3+0] = i*m_grassBladeNum*4+z*4+0;
				m_indices[i*m_grassBladeNum*2*3+z*2*3+1] = i*m_grassBladeNum*4+z*4+1;
				m_indices[i*m_grassBladeNum*2*3+z*2*3+2] = i*m_grassBladeNum*4+z*4+3;
				m_indices[i*m_grassBladeNum*2*3+z*2*3+3] = i*m_grassBladeNum*4+z*4+1;
				m_indices[i*m_grassBladeNum*2*3+z*2*3+4] = i*m_grassBladeNum*4+z*4+2;
				m_indices[i*m_grassBladeNum*2*3+z*2*3+5] = i*m_grassBladeNum*4+z*4+3;
				angle += D3DX_PI/m_grassBladeNum;
			}
			minus = !minus;
			x++;
			if(x >= g_grassSectorSize)
			{
				x = 0;
				y++;
			}
		}
		m_VB->Unlock();
		m_IB->Unlock();
	}
}

void GrassMesh::reassign(Vec3 *positions, Vec4 uvs, Vec2 grassBladeDimm, int grassBladesNum, int arraySize)
{
	generateStats(grassBladesNum, arraySize);
	m_grassBladeDimm = grassBladeDimm;
	m_uvs = uvs;
	float angle = 0;
	m_grassPositions = positions;
	Mat rot;

	if(m_VB)
		m_VB->Release();
	if(m_IB)
		m_IB->Release();

	if(!m_grassNum)
		return;

	const D3DVERTEXELEMENT9 vertexDecl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,  0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
		{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		D3DDECL_END()
	};
	gEngine.device->getDev()->CreateVertexDeclaration(vertexDecl, &m_pVertexDeclaration);
	if(gEngine.device->getDev()->CreateIndexBuffer(m_indicesNum * sizeof(WORD), D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_IB, NULL) != D3D_OK)
	{
		m_IB = NULL;
		return;
	}
	if(gEngine.device->getDev()->CreateVertexBuffer(m_verticesNum * sizeof(GrassVertex), D3DUSAGE_DYNAMIC, GRASSVERT_FVF, D3DPOOL_DEFAULT, &m_VB, NULL) != D3D_OK)
	{
		m_VB = NULL;
		return;
	}
	m_VB->Lock(0, 0, (void**)&m_vertices, D3DLOCK_DISCARD);
	m_IB->Lock(0, 0, (void**)&m_indices, D3DLOCK_DISCARD);

	int modulo = 3.14f*100;
	time_t t;
	srand((unsigned) time(&t));
	int x = 0;
	int y = 0;
	bool minus = true;
	for(int i = 0; i < m_grassNum; i++)
	{
		Vec2 speedModifier = Vec2((rand()%100)/100.0f, (rand()%100)/100.0f);
		angle += rand()%modulo/100.0f;
		//gEngine.kernel->log->prnEx("SpeedModifier: %f %f", speedModifier.x, speedModifier.y);
		if(angle > 1000)
			angle = 0;
		for(int z = 0; z < m_grassBladeNum; z++)
		{
			Mat rot;
			D3DXMatrixRotationY(&rot, angle);
			Vec3 tpos = Vec3(-0.5f * grassBladeDimm.x, grassBladeDimm.y, 0.0f);
			D3DXVec3TransformCoord(&tpos, &tpos, &rot);
			m_vertices[i*m_grassBladeNum*4+z*4+0].pos = tpos + positions[i];
			m_vertices[i*m_grassBladeNum*4+z*4+0].uv = Vec2(m_uvs.x, m_uvs.y);
			m_vertices[i*m_grassBladeNum*4+z*4+0].normal = Vec3(0, 1, 0);

			m_vertices[i*m_grassBladeNum*4+z*4+0].speedModifier = speedModifier;
			if(minus)
			m_vertices[i*m_grassBladeNum*4+z*4+0].speedModifier = -speedModifier;
			m_vertices[i*m_grassBladeNum*4+z*4+0].dispMapCoord = Vec2(x, y);

			tpos = Vec3(0.5f * grassBladeDimm.x, grassBladeDimm.y, 0.0f);
			D3DXVec3TransformCoord(&tpos, &tpos, &rot);
			m_vertices[i*m_grassBladeNum*4+z*4+1].pos = tpos + positions[i];
			m_vertices[i*m_grassBladeNum*4+z*4+1].uv = Vec2(m_uvs.z, m_uvs.y);
			m_vertices[i*m_grassBladeNum*4+z*4+0].normal = Vec3(0, 1, 0);

			m_vertices[i*m_grassBladeNum*4+z*4+1].speedModifier = speedModifier;
			if(minus)
			m_vertices[i*m_grassBladeNum*4+z*4+1].speedModifier = -speedModifier;
			m_vertices[i*m_grassBladeNum*4+z*4+1].dispMapCoord = Vec2(x, y);

			tpos = Vec3(0.5f * grassBladeDimm.x, 0.0f, 0.0f);
			D3DXVec3TransformCoord(&tpos, &tpos, &rot);
			m_vertices[i*m_grassBladeNum*4+z*4+2].pos = tpos + positions[i];
			m_vertices[i*m_grassBladeNum*4+z*4+2].uv = Vec2(m_uvs.z, m_uvs.w);
			m_vertices[i*m_grassBladeNum*4+z*4+2].speedModifier = Vec2(0, 0);
			m_vertices[i*m_grassBladeNum*4+z*4+2].dispMapCoord = Vec2(x, y);
			m_vertices[i*m_grassBladeNum*4+z*4+0].normal = Vec3(0, 1, 0);

			tpos = Vec3(-0.5f * grassBladeDimm.x, 0.0f, 0.0f);
			D3DXVec3TransformCoord(&tpos, &tpos, &rot);
			m_vertices[i*m_grassBladeNum*4+z*4+3].pos = tpos + positions[i];
			m_vertices[i*m_grassBladeNum*4+z*4+3].uv = Vec2(m_uvs.x, m_uvs.w);
			m_vertices[i*m_grassBladeNum*4+z*4+3].speedModifier = Vec2(0, 0);
			m_vertices[i*m_grassBladeNum*4+z*4+3].dispMapCoord = Vec2(x, y);
			m_vertices[i*m_grassBladeNum*4+z*4+0].normal = Vec3(0, 1, 0);

			m_indices[i*m_grassBladeNum*2*3+z*2*3+0] = i*m_grassBladeNum*4+z*4+0;
			m_indices[i*m_grassBladeNum*2*3+z*2*3+1] = i*m_grassBladeNum*4+z*4+1;
			m_indices[i*m_grassBladeNum*2*3+z*2*3+2] = i*m_grassBladeNum*4+z*4+3;
			m_indices[i*m_grassBladeNum*2*3+z*2*3+3] = i*m_grassBladeNum*4+z*4+1;
			m_indices[i*m_grassBladeNum*2*3+z*2*3+4] = i*m_grassBladeNum*4+z*4+2;
			m_indices[i*m_grassBladeNum*2*3+z*2*3+5] = i*m_grassBladeNum*4+z*4+3;
			angle += D3DX_PI/m_grassBladeNum;
		}
		x++;
		if(x > g_grassSectorSize)
		{
			x = 0;
			y++;
		}
	}

	m_VB->Unlock();
	m_IB->Unlock();
}

void GrassMesh::deleteMesh()
{
	if(m_VB)
	m_VB->Release();
	if(m_IB)
	m_IB->Release();
	m_VB = NULL;
	m_IB = NULL;
	//if(m_pMesh)
	//{
	//      m_pMesh->Release();
	//      m_pMesh = NULL;
	//      /*free(m_vertices);
	//      free(m_indices);
	//      m_vertices = 0;
	//      m_indices = 0;*/
	//}
}

void GrassMesh::recreate()
{
	reassign(m_grassPositions, m_uvs, m_grassBladeDimm, m_grassBladeNum, m_grassNum);
}

void GrassMesh::generateStats(int grassBladesNum, int grassNum)
{
	m_grassBladeNum = grassBladesNum;
	m_grassNum = grassNum;
	m_verticesNum = 4 * m_grassBladeNum * m_grassNum;
	m_indicesNum = m_grassBladeNum * m_grassNum * 2 * 3;
	m_facesNum = m_grassBladeNum * m_grassNum * 2;
}
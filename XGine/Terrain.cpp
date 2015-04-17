#include "Terrain.h"

Terrain::Terrain(TerrainVertex *vertices, LPDIRECT3DTEXTURE9 indTex, u32 secX, u32 secY, u32 sectorSize, u32 hmSize, f32 texHmGeoStep)
{
	shader = 0;
	m_indexTexture = indTex;
	m_colorTexture[0] = 0;
	m_colorTexture[1] = 0;
	m_colorTexture[2] = 0;
	m_colorTexture[3] = 0;
	m_vb = 0;
	m_ibCenter = 0;
	m_ibBorder = 0;
	m_vbSize = 0;
	m_ibCenterSize = 0;
	m_ibBorderSize = 0;
	m_indexTexture = 0;
	m_borderTopLod = 0;
	m_borderLeftLod = 0;
	m_borderTopLod = 0;
	m_borderBottomLod = 0;
	m_top = m_left = m_right = m_bottom = 0;
	m_sectorSize = sectorSize;
	mesh = 0;

	//DEFERRED MAT
	material = new Material();
	if(g_terrainTexColorMap1.size())material->m_colorMapSplatting[0] = gEngine.resMgr->load<Texture>(g_terrainTexColorMap1);
	if(g_terrainTexColorMap2.size())material->m_colorMapSplatting[1] = gEngine.resMgr->load<Texture>(g_terrainTexColorMap2);
	if(g_terrainTexColorMap3.size())material->m_colorMapSplatting[2] = gEngine.resMgr->load<Texture>(g_terrainTexColorMap3);
	if(g_terrainTexColorMap4.size())material->m_colorMapSplatting[3] = gEngine.resMgr->load<Texture>(g_terrainTexColorMap4);
	if(g_terrainTexNormalMap1.size())material->m_normalMapSplatting[0] = gEngine.resMgr->load<Texture>(g_terrainTexNormalMap1);
	if(g_terrainTexNormalMap2.size())material->m_normalMapSplatting[1] = gEngine.resMgr->load<Texture>(g_terrainTexNormalMap2);
	if(g_terrainTexNormalMap3.size())material->m_normalMapSplatting[2] = gEngine.resMgr->load<Texture>(g_terrainTexNormalMap3);
	if(g_terrainTexNormalMap4.size())material->m_normalMapSplatting[3] = gEngine.resMgr->load<Texture>(g_terrainTexNormalMap4);
	material->m_type = MT_TERRAIN;
	material->m_albedoMap = gEngine.resMgr->load<Texture>("noise.jpg");
	material->m_useSpecular = 1.0f;
	material->m_splatting = 1;
	material->update();

	shader = material->m_shader;

	Vec3 minbb = Vec3(200000,200000,200000), maxbb = Vec3(-200000,-200000,-200000);

	TerrainVertex	*v;
	WORD			*indices;
	
	m_levels = sqrt((f32)sectorSize+1)-1;
	
	if(m_levels>g_terrainMaxLODLevels)
		m_levels=g_terrainMaxLODLevels;

	m_vb			= new LPDIRECT3DVERTEXBUFFER9[m_levels];
	m_ibCenter		= new LPDIRECT3DINDEXBUFFER9[m_levels];
	m_ibBorder		= new LPDIRECT3DINDEXBUFFER9[m_levels];
	m_vbSize		= new u32[m_levels];
	m_ibCenterSize	= new u32[m_levels];
	m_ibBorderSize	= new u32[m_levels];

	for(u32 level = 0; level < m_levels; level++)
	{
		u32 lev = pow((f32)2, (i32)level);
		u32 lowWidth = (((f32)sectorSize)/(f32)lev)+1;
		m_vbSize[level] = (u32)(lowWidth * lowWidth);
		gEngine.kernel->addMemUsage("Terrain", m_vbSize[level] * sizeof(TerrainVertex));
		if(FAILED(gEngine.device->getDev()->CreateVertexBuffer(m_vbSize[level] * sizeof(TerrainVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_vb[level], NULL)))
		{ gEngine.kernel->log->prnEx(LT_ERROR, "Terrain", "Could not create vertex buffer for level %u", level); return; }
		m_vb[level]->Lock(0, 0, (void**)&v, 0);
		u32 count = 0;
		for (UINT y = 0; y <= sectorSize; y+=lev){
			for (UINT x = 0; x <= sectorSize; x+=lev){
				u32 vertIndex = ((secY*sectorSize + y)*(hmSize) + secX*sectorSize + x);
				Vec3 pos = vertices[vertIndex].pos;
				//if(( (x == 0 || x == (sectorSize)) || (y == 0 || y == (sectorSize))  ) && level > 0)pos.y -= pow(g_terrainLowVBExponentX, (f32)level) * g_terrainLowVBExponent;
				if(pos.x > maxbb.x)maxbb.x=pos.x;if(pos.y > maxbb.y)maxbb.y=pos.y;if(pos.z > maxbb.z)maxbb.z=pos.z;
				if(pos.x < minbb.x)minbb.x=pos.x;if(pos.y < minbb.y)minbb.y=pos.y;if(pos.z < minbb.z)minbb.z=pos.z;
				v[count] = vertices[vertIndex];
				//v[count].texCoord = Vec2((float)(pos.x)/(hmSize*texHmGeoStep), (float)(pos.z)/(hmSize*texHmGeoStep));
				v[count].texCoord = Vec2((f32)x/(f32)sectorSize,(f32)y/(f32)sectorSize);
				count++;
			}
		}
		u32 centerWidth = ((sectorSize/lev)-2)?((sectorSize/lev)-2):((sectorSize/lev)-1);
		m_ibCenterSize[level] = (centerWidth * centerWidth * 3 * 2);
		gEngine.kernel->addMemUsage("Terrain", m_ibCenterSize[level] * sizeof(WORD));
		if(FAILED(gEngine.device->getDev()->CreateIndexBuffer(m_ibCenterSize[level] * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ibCenter[level], NULL)))
		{ gEngine.kernel->log->prnEx(LT_ERROR, "Terrain", "Could not create center index buffer for level %u", level);  return; }
		m_ibBorderSize[level] = (sectorSize/lev)*5*3 + (sectorSize/lev-1)*4*3;
		gEngine.kernel->addMemUsage("Terrain", m_ibBorderSize[level] * sizeof(WORD));
		if(FAILED(gEngine.device->getDev()->CreateIndexBuffer(m_ibBorderSize[level] * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ibBorder[level], NULL)))
		{ gEngine.kernel->log->prnEx(LT_ERROR, "Terrain", "Could not create border index buffer for level %u", level);  return; }
		m_ibCenter[level]->Lock(0, 0, (void**)&indices, 0);
		count = 0;
		WORD vIndex = 0;
		for (UINT z = 1; z < (sectorSize)/lev - 1; z++){
			for (UINT x = 1; x < (sectorSize)/lev - 1; x++){
				vIndex = z * lowWidth + x;
				// first triangle
				indices[count++] = vIndex + 1; indices[count++] = vIndex; indices[count++] = vIndex + lowWidth;
				// second triangle
				indices[count++] = vIndex + 1; indices[count++] = vIndex + lowWidth; indices[count++] = vIndex + lowWidth + 1;
			}
		}
		m_vb[level]->Unlock();
		m_ibCenter[level]->Unlock();
	}

	position = Vec3(0,0,0);
	boundingBox = BoundingBox(minbb, maxbb);

	boundingBox.setTransform(position);
	worldMat(&world, position);

	m_borderTopLod = new u32[m_levels];
	m_borderLeftLod = new u32[m_levels];
	m_borderRightLod = new u32[m_levels];
	m_borderBottomLod = new u32[m_levels];

	for(u32 i = 0; i < m_levels; i++)
	{
		m_borderTopLod[i] = 999;
		m_borderLeftLod[i] = 999;
		m_borderRightLod[i] = 999;
		m_borderBottomLod[i] = 999;
	}

	m_lod = 0;
	m_lodGeo = 0;

	gEngine.kernel->addMemUsage("Terrain", sizeof(this));
}

Terrain::~Terrain()
{
	material->cleanup();
	delete(material);

	for(u32 i = 0; i < m_levels; i++)
	{
		DXRELEASE(m_vb[i]);
		DXRELEASE(m_ibCenter[i]);
		DXRELEASE(m_ibBorder[i]);
	}
	delete(m_vbSize);
	delete(m_ibCenterSize);
	delete(m_ibBorderSize);
	delete(m_borderTopLod);
	delete(m_borderLeftLod);
	delete(m_borderRightLod);
	delete(m_borderBottomLod);

	shader = 0;
	m_indexTexture = 0;
	m_colorTexture[0] = 0;
	m_colorTexture[1] = 0;
	m_colorTexture[2] = 0;
	m_colorTexture[3] = 0;
	m_vb = 0;
	m_ibCenter = 0;
	m_ibBorder = 0;
	m_vbSize = 0;
	m_ibCenterSize = 0;
	m_ibBorderSize = 0;
	m_indexTexture = 0;
	m_borderTopLod = 0;
	m_borderLeftLod = 0;
	m_borderTopLod = 0;
	m_borderBottomLod = 0;
	m_top = m_left = m_right = m_bottom = 0;
}

void Terrain::update(ICamera* cam)
{
	if(material)
		material->m_indexMap = m_indexTexture;

	Vec3 v = (boundingBox.BSphere.Center) - *cam->getPosition();
	f32 dist = D3DXVec3Length(&v) - boundingBox.BSphere.Radius;
	if(dist < 0) dist = 0;

	m_lod = 0;
	if(dist < g_terrainMaxDistHighTex)m_lod = 0;
	else if(dist < (g_terrainMaxDistHighTex+g_terrainMaxDistHighTexFade))m_lod = 1;
	else m_lod = 2;
	m_lod = 0;

	m_lodGeo = g_terrainLodStart;

	if(g_terrainLodEnabled)
	{
		u32 maxLodGeo = m_levels;
		(m_top)?( (m_top->m_lodGeo < maxLodGeo)?(maxLodGeo = m_top->m_lodGeo):(0)):(0);
		(m_left)?( (m_left->m_lodGeo < maxLodGeo)?(maxLodGeo = m_left->m_lodGeo):(0)):(0);
		(m_right)?( (m_right->m_lodGeo < maxLodGeo)?(maxLodGeo = m_right->m_lodGeo):(0)):(0);
		(m_bottom)?( (m_bottom->m_lodGeo < maxLodGeo)?(maxLodGeo = m_bottom->m_lodGeo):(0)):(0);
		maxLodGeo += 1;
		
		for(i32 i = m_lodGeo+1; i < m_levels && i <= maxLodGeo; i++)
		{
			if(dist > g_terrainMaxDistHighGeo + g_terrainMaxDistHighGeoStep*(i))
				m_lodGeo = i;
		}
	}
}

void Terrain::updateBorderLod()
{
	bool change = false;

	if( (m_top)?(m_top->m_lodGeo != m_borderTopLod[m_lodGeo]):(m_borderTopLod[m_lodGeo] != 0) )change = true;
	if( (m_left)?(m_left->m_lodGeo !=  m_borderLeftLod[m_lodGeo]):(m_borderLeftLod[m_lodGeo] != 0) )change = true;
	if( (m_right)?(m_right->m_lodGeo!= m_borderRightLod[m_lodGeo]):(m_borderRightLod[m_lodGeo] != 0) )change = true;
	if( (m_bottom)?(m_bottom->m_lodGeo !=  m_borderBottomLod[m_lodGeo]):(m_borderBottomLod[m_lodGeo] != 0) )change = true;

	if(change)
	{
		u32 top = (m_top)?(m_top->m_lodGeo > m_lodGeo):(0);
		u32 left = (m_left)?(m_left->m_lodGeo > m_lodGeo):(0);
		u32 right = (m_right)?(m_right->m_lodGeo > m_lodGeo):(0);
		u32 bottom = (m_bottom)?(m_bottom->m_lodGeo > m_lodGeo):(0);

		u32 lev = pow((f32)2, (i32)m_lodGeo);
		u32 size = (m_sectorSize)/lev + 1;
		WORD *indices;
		u32 count = 0;
		m_ibBorder[m_lodGeo]->Lock(0, 0, (void**)&indices, 0);

		if(top)
		{
			u32 dir = 1;
			for(UINT i = 1; i < size - 1; i++)
			{	
				if(dir)
				{
					indices[count++] = 0*size + i + 1;
					indices[count++] = 0*size + i - 1;
					indices[count++] = 1*size + i;
				}
				else
				{
					indices[count++] = 1*size + i + 1;
					indices[count++] = 0*size + i;
					indices[count++] = 1*size + i;
					indices[count++] = 1*size + i;
					indices[count++] = 0*size + i;
					indices[count++] = 1*size + i - 1;
				}
				dir = !dir;
			}
		}
		else
		{
			indices[count++] = 0*size + 1;
			indices[count++] = 0*size + 0;
			indices[count++] = 1*size + 1;
			for(UINT i = 1; i < size - 2; i++)
			{
				indices[count++] = 0*size + i + 1;
				indices[count++] = 0*size + i;
				indices[count++] = 1*size + i;
				indices[count++] = 1*size + i + 1;
				indices[count++] = 0*size + i + 1;
				indices[count++] = 1*size + i;
			}
			indices[count++] = 0*size + size - 1;
			indices[count++] = 0*size + size - 2;
			indices[count++] = 1*size + size - 2;
		}

		if(left)
		{
			u32 dir = 1;
			for(UINT i = 1; i < size - 1; i++)
			{	
				if(dir)
				{
					indices[count++] = (i-1)*size + 0;
					indices[count++] = (i+1)*size + 0;
					indices[count++] = i*size + 1;
				}
				else
				{
					indices[count++] = (i-1)*size + 1;
					indices[count++] = i*size + 0;
					indices[count++] = (i)*size + 1;
					indices[count++] = (i)*size + 1;
					indices[count++] = i*size + 0;
					indices[count++] = (i+1)*size + 1;
				}
				dir = !dir;
			}
		}
		else
		{
			indices[count++] = 1*size + 1;
			indices[count++] = 0*size + 0;
			indices[count++] = 1*size + 0;
			for(UINT i = 1; i < size - 2; i++)
			{
				indices[count++] = (i)*size + 1;
				indices[count++] = i*size + 0;
				indices[count++] = (i+1)*size + 0;
				indices[count++] = (i)*size + 1;
				indices[count++] = (i+1)*size + 0;
				indices[count++] = (i+1)*size + 1;
			}
			indices[count++] = (size-1)*size + 0;
			indices[count++] = (size-2)*size + 1;
			indices[count++] = (size-2)*size + 0;
		}

		if(right)
		{
			u32 dir = 1;
			for(UINT i = 1; i < size - 1; i++)
			{	
				if(dir)
				{
					indices[count++] = (i+1)*size + size - 1;
					indices[count++] = (i-1)*size + size - 1;
					indices[count++] = i*size + size - 2;
				}
				else
				{
					indices[count++] = (i)*size + size - 1;
					indices[count++] = (i-1)*size + size - 2;
					indices[count++] = (i)*size + size - 2;
					indices[count++] = (i+1)*size + size - 2;
					indices[count++] = (i)*size + size - 1;
					indices[count++] = (i)*size + size - 2;
				}
				dir = !dir;
			}
		}
		else
		{
			indices[count++] = 1*size + size - 1;
			indices[count++] = 0*size + size - 1;
			indices[count++] = 1*size + size - 2;
			for(UINT i = 1; i < size - 2; i++)
			{
				indices[count++] = (i+1)*size + size - 1;
				indices[count++] = i*size + size - 1;
				indices[count++] = (i+1)*size + size - 2;
				indices[count++] = i*size + size - 1;
				indices[count++] = i*size + size - 2;
				indices[count++] = (i+1)*size + size - 2;
			}

			indices[count++] = (size-1)*size + size - 1;
			indices[count++] = (size-2)*size + size - 1;
			indices[count++] = (size-2)*size + size - 2;
		}

		if(bottom)
		{
			u32 dir = 1;
			for(UINT i = 1; i < size - 1; i++)
			{	
				if(dir)
				{
					indices[count++] = (size - 1)*size + i + 1;
					indices[count++] = (size - 2)*size + i;
					indices[count++] = (size - 1)*size + i - 1;
				}
				else
				{
					indices[count++] = (size - 1)*size + i;
					indices[count++] = (size - 2)*size + i;
					indices[count++] = (size - 2)*size + i - 1;
					indices[count++] = (size - 1)*size + i;
					indices[count++] = (size - 2)*size + i + 1;
					indices[count++] = (size - 2)*size + i;
				}
				dir = !dir;
			}
		}
		else
		{
			indices[count++] = (size - 1)*size + 1;
			indices[count++] = (size - 2)*size + 1;
			indices[count++] = (size - 1)*size + 0;
			for(UINT i = 1; i < size - 2; i++)
			{
				indices[count++] = (size - 1)*size + i + 1;
				indices[count++] = (size - 2)*size + i + 1;
				indices[count++] = (size - 1)*size + i;
				indices[count++] = (size - 2)*size + i + 1;
				indices[count++] = (size - 2)*size + i;
				indices[count++] = (size - 1)*size + i;
			}
			indices[count++] = (size-1)*size + size - 1;
			indices[count++] = (size-2)*size + size - 2;
			indices[count++] = (size-1)*size + size - 2;
		}

		m_ibBorder[m_lodGeo]->Unlock();
		m_ibBorderSize[m_lodGeo] = count;

		m_borderTopLod[m_lodGeo] = (m_top)?(m_top->m_lodGeo):(0);
		m_borderLeftLod[m_lodGeo] = (m_left)?(m_left->m_lodGeo):(0);
		m_borderRightLod[m_lodGeo] = (m_right)?(m_right->m_lodGeo):(0);
		m_borderBottomLod[m_lodGeo] = (m_bottom)?(m_bottom->m_lodGeo):(0);
	}
}

u32 Terrain::renderForward(Light* light)
{
	if(m_vb && m_ibCenter && m_ibBorder && m_vbSize && m_ibCenterSize && m_ibBorderSize && shader && light)
	{
		updateBorderLod();

		if(gEngine.renderer->get().pass == REND_REFLECT)
		{
			gEngine.renderer->setShader(shader->setTech("RenderForward"));
			gEngine.renderer->setMaterial(material);
			gEngine.renderer->setupLight(light);
			gEngine.renderer->setWorld(&world);
			gEngine.renderer->commitChanges();
				gEngine.device->getDev()->SetVertexDeclaration(gEngine.renderer->m_vd[VD_TERRAIN]);
				gEngine.device->getDev()->SetStreamSource(0, m_vb[m_lodGeo], 0, sizeof(TerrainVertex));
				gEngine.device->getDev()->SetIndices(m_ibCenter[m_lodGeo]);
				gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vbSize[m_lodGeo], 0, m_ibCenterSize[m_lodGeo] / 3);
				gEngine.device->getDev()->SetIndices(m_ibBorder[m_lodGeo]);
				gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vbSize[m_lodGeo], 0, m_ibBorderSize[m_lodGeo] / 3);				
		}
		else
		{
			gEngine.renderer->setShader(shader->setTech("RenderForward"));
			gEngine.renderer->setMaterial(material);
			gEngine.renderer->setupLight(light);
			gEngine.renderer->setWorld(&world);
			gEngine.renderer->commitChanges();
				gEngine.device->getDev()->SetVertexDeclaration(gEngine.renderer->m_vd[VD_TERRAIN]);
				gEngine.device->getDev()->SetStreamSource(0, m_vb[m_lodGeo], 0, sizeof(TerrainVertex));
				gEngine.device->getDev()->SetIndices(m_ibCenter[m_lodGeo]);
				gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vbSize[m_lodGeo], 0, m_ibCenterSize[m_lodGeo] / 3);
				gEngine.device->getDev()->SetIndices(m_ibBorder[m_lodGeo]);
				gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vbSize[m_lodGeo], 0, m_ibBorderSize[m_lodGeo] / 3);
			

			if(g_terrainDebugBB)
				gEngine.renderer->drawBoundingBox(&boundingBox, Vec4(1,0,1,1));

		}

		gEngine.renderer->addRenderedCounts(m_vbSize[m_lodGeo], m_ibCenterSize[m_lodGeo] / 3 + m_ibBorderSize[m_lodGeo] / 3);

		return 1;
	}
	return 0;
}

u32 Terrain::renderRaw()
{
	if(m_vb && m_ibCenter && m_ibBorder && m_vbSize && m_ibCenterSize && m_ibBorderSize && shader)
	{
		if(gEngine.renderer->get().pass == REND_SM)
			return 0;
		updateBorderLod();
		gEngine.renderer->setShader(shader->setTech("RenderRaw"));
		gEngine.renderer->setWorld(&world);
		gEngine.renderer->commitChanges();
			gEngine.device->getDev()->SetVertexDeclaration(gEngine.renderer->m_vd[VD_TERRAIN]);
			gEngine.device->getDev()->SetStreamSource(0, m_vb[m_lodGeo], 0, sizeof(TerrainVertex));
			gEngine.device->getDev()->SetIndices(m_ibCenter[m_lodGeo]);
			gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vbSize[m_lodGeo], 0, m_ibCenterSize[m_lodGeo] / 3);
			gEngine.device->getDev()->SetIndices(m_ibBorder[m_lodGeo]);
			gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vbSize[m_lodGeo], 0, m_ibBorderSize[m_lodGeo] / 3);

			gEngine.renderer->addRenderedCounts(2 * m_vbSize[m_lodGeo], m_ibCenterSize[m_lodGeo] / 3 + m_ibBorderSize[m_lodGeo] / 3, 2);
		return 1;
	}
	return 0;
}

u32	Terrain::renderDeferred()
{
	if(m_vb && m_ibCenter && m_ibBorder && m_vbSize && m_ibCenterSize && m_ibBorderSize && shader)
	{
		updateBorderLod();
		gEngine.renderer->setShader(shader->setTech("RenderGBuffer"));
		gEngine.renderer->setWorld(&world);
		gEngine.renderer->setMaterial(material);
		gEngine.renderer->commitChanges();
			gEngine.device->getDev()->SetVertexDeclaration(gEngine.renderer->m_vd[VD_TERRAIN]);
			gEngine.device->getDev()->SetStreamSource(0, m_vb[m_lodGeo], 0, sizeof(TerrainVertex));
			gEngine.device->getDev()->SetIndices(m_ibCenter[m_lodGeo]);
			gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vbSize[m_lodGeo], 0, m_ibCenterSize[m_lodGeo] / 3);
			gEngine.device->getDev()->SetIndices(m_ibBorder[m_lodGeo]);
			gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vbSize[m_lodGeo], 0, m_ibBorderSize[m_lodGeo] / 3);

			gEngine.renderer->addRenderedCounts(2 * m_vbSize[m_lodGeo], m_ibCenterSize[m_lodGeo] / 3 + m_ibBorderSize[m_lodGeo] / 3, 2);
		return 1;
	}
	return 0;
}
#include "TerrainManager.h"

#include <sys/types.h> // dla _stat
#include <sys/stat.h> // dla _stat

TerrainManager::TerrainManager(Scene* scene, u32 hmGeoStep, u32 indTexExp)
	: m_hmInit(0), m_scene(scene), m_vertices(0), m_indices(0), m_sectorSize(0), m_size(0), m_indexTextureData(0), m_hmGeoStep(hmGeoStep), m_indTexExp(indTexExp), m_indexTextures(0)
{
	m_scene = scene;
}

TerrainManager::~TerrainManager()
{
	cleanup();
}

void TerrainManager::cleanup()
{
	if(m_scene && m_terrains.size())
	{
		for(u32 secX = 0; secX < ((m_size-1)/m_sectorSize); secX++)
		{
			for(u32 secY = 0; secY < ((m_size-1)/m_sectorSize); secY++)
			{
					//delete(m_terrains[secX][secY]);
			}
		}
	}

	if(m_vertices)			delete(m_vertices);				m_vertices = 0;
	if(m_indices)			delete(m_indices);				m_indices = 0;
	if(m_indexTextureData)	delete(m_indexTextureData);		m_indexTextureData = 0;
	m_position = Vec3(0,0,0);
	m_size = 0;
	m_sectorSize = 0;
	m_numVertices = 0;
	m_numIndices = 0;
}

u32 TerrainManager::create(const string& texHmName, const string& texIndName, u32 size, Vec3 pos)
{
	cleanup();
	m_sectorSize = g_terrainSectorSize;

	D3DLOCKED_RECT lockedRect, lockedRectHM;
	Texture *rhm;
	Texture *rind;

	/////////////////////////////////////////////////////////////////////////////////////////////////

	if(m_hmInit)
	{ gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "[TerrainManager] Terrain will be reloaded."); }

	rhm = gEngine.resMgr->load<Texture>(texHmName, 0);
	if(rhm == 0)
	{ gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "[TerrainManager] Could not load height map texture: '%s'", (g_path_textures+string(texHmName)).c_str()); return 0; }

	rind = gEngine.resMgr->load<Texture>(texIndName, 0);
	if(rind == 0)
	{ gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "[TerrainManager] Could not load index map texture: '%s'", (g_path_textures+string(texIndName)).c_str()); return 0; }

	/////////////////////////////////////////////////////////////////////////////////////////////////

	//rhm->lpTexture->GetSurfaceLevel(0, &surface);
	//surface->GetDC(&hdc);
	//surface->GetDesc(&desc);

	
	if(!rind->get() || FAILED(rind->get()->LockRect(0, &lockedRect, 0, D3DLOCK_READONLY)))return 0;
	if(!rhm->get() ||FAILED(rhm->get()->LockRect(0, &lockedRectHM, 0, D3DLOCK_READONLY)))return 0;
		
	/////////////////////////////////////////////////////////////////////////////////////////////////

	if( size != rhm->width || size != rhm->height )
	{ gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "[TerrainManager] Invalid size (wh) height map texture: '%s'(%ux%u)", (g_path_textures+string(texHmName)).c_str(), size, size); return 0; }
	
	if( (size*m_indTexExp) != rind->width || (size*m_indTexExp) != rind->height )
	{ gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "[TerrainManager] Invalid size (wh) index map texture: '%s'(%ux%u)", (g_path_textures+string(texIndName)).c_str(), size, size); return 0; }

	if( ((size)%m_sectorSize != 0) )
	{ gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "[TerrainManager] Invalid size (sector) height map texture: '%s'(%ux%u)", (g_path_textures+string(texHmName)).c_str(), size, size); return 0; }
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	m_size = size + 1;

	m_numVertices = m_size*m_size;
	m_vertices = new TerrainVertex[m_numVertices];
	f32	*heights = new f32[m_numVertices];

	m_numIndices = size*size*3*2;
	m_indices  = new u32[m_numIndices];


	for(u32 y = 0; y < m_size; y++)
	{
		for(u32 x = 0; x < m_size; x++)
		{
			m_vertices[y*m_size+x].pos		= Vec3((f32)pos.x+x*m_hmGeoStep, (f32)0, (f32)pos.z+y*m_hmGeoStep);
			m_vertices[y*m_size+x].normal	= Vec3(0,0,0);
			m_vertices[y*m_size+x].texCoord	= Vec2((f32)x/(f32)m_size,(f32)y/(f32)m_size);
		}
	}


	for(u32 y = 0; y < m_size; y++)
	{
		for(u32 x = 0; x < m_size; x++)
		{
			u32 c = ((u32*)lockedRectHM.pBits)[y * rhm->width + x];
			D3DXCOLOR cc = D3DXCOLOR(c);
			f32 h = (f32)((f32)cc.r) * g_terrainMaxHeight;
			m_vertices[y*m_size+x].pos.y = (f32)h;

			if(y == 0 || y == (m_size-1) || x == 0 || x == (m_size-1))
				m_vertices[y*m_size+x].pos.y = 0;
			else if(m_vertices[y*m_size+x].pos.y < 1)
				m_vertices[y*m_size+x].pos.y = 1;
		}
	}

	i32 tBA = 3;

	for(i32 y = 0; y < m_size; y++)
	{
		for(i32 x = 0; x < m_size; x++)
		{
			f32 sum = 0;
			for(i32 py = -tBA; py <= tBA; py++)
			{
				for(i32 px = -tBA; px <= tBA; px++)
				{	
					i32 yy = py+y;
					i32 xx = px+x;
					if(yy < m_size && yy >= 0 && xx < m_size && xx >= 0)
						sum += (f32)m_vertices[yy*m_size+xx].pos.y;
					else 
						sum += (f32)m_vertices[y*m_size+x].pos.y;
				}
			}
			sum/=( (2*tBA+1)*(2*tBA+1) );
			heights[y*m_size+x] = (f32)sum;
		}
	}

	for(u32 y = 0; y < m_size; y++)
	{
		for(u32 x = 0; x < m_size; x++)
		{
			u32 c = ((u32*)lockedRectHM.pBits)[y * rhm->width + x];
			D3DXCOLOR cc = D3DXCOLOR(c);
			f32 y0 = m_vertices[y*m_size+x].pos.y;
			f32 y1 = heights[y*m_size+x];
			f32 e = cc.g;
			m_vertices[y*m_size+x].pos.y= y0 + (y1-y0)*e;
		}
	}

	u32  i1, i2, i3;
	Vec3 normal;
	u32 count = 0;
	for (u32 y = 0; y < size; y++)
	{
		for (u32 x = 0; x < size; x++)
		{
			u32 index = y * m_size + x;
			// first triangle
			i1 = index + 1; i2 = index; i3 = index + m_size;
			m_indices[count++] = i1; m_indices[count++] = i2; m_indices[count++] = i3;
			D3DXVec3Cross(&normal, &(m_vertices[i1].pos - m_vertices[i2].pos), &(m_vertices[i3].pos - m_vertices[i2].pos));
			D3DXVec3Normalize(&normal, &normal);
			m_vertices[i3].normal -= normal;
			m_vertices[i2].normal -= normal;
			m_vertices[i1].normal -= normal;
			D3DXVec3Normalize(&m_vertices[i3].normal, &m_vertices[i3].normal);
			D3DXVec3Normalize(&m_vertices[i2].normal, &m_vertices[i2].normal);
			D3DXVec3Normalize(&m_vertices[i1].normal, &m_vertices[i1].normal);
			// second triangle
			i1 = index + 1; i2 = index + m_size; i3 = index + m_size + 1;
			m_indices[count++] = i1; m_indices[count++] = i2; m_indices[count++] = i3;
			D3DXVec3Cross(&normal, &(m_vertices[i1].pos - m_vertices[i2].pos), &(m_vertices[i3].pos - m_vertices[i2].pos));
			D3DXVec3Normalize(&normal, &normal);
			m_vertices[i3].normal -= normal;
			m_vertices[i2].normal -= normal;
			m_vertices[i1].normal -= normal;
			D3DXVec3Normalize(&m_vertices[i3].normal, &m_vertices[i3].normal);
			D3DXVec3Normalize(&m_vertices[i2].normal, &m_vertices[i2].normal);
			D3DXVec3Normalize(&m_vertices[i1].normal, &m_vertices[i1].normal);
		}
	}

	//Tangents binormals:
	Vec3 *tan1 = new Vec3[m_numVertices * 2];
	Vec3 *tan2 = tan1 + m_numVertices;
	ZeroMemory(tan1, m_numVertices * sizeof(Vec3) * 2);

	for (long a = 0; a < m_numIndices/3; a++)
	{
		long i1 = m_indices[3*a+0];
		long i2 = m_indices[3*a+1];
		long i3 = m_indices[3*a+2];

		Vec3 v1 = m_vertices[i1].pos;
		Vec3 v2 = m_vertices[i2].pos;
		Vec3 v3 = m_vertices[i3].pos;
	    
		Vec2 w1 = m_vertices[i1].texCoord;
		Vec2 w2 = m_vertices[i2].texCoord;
		Vec2 w3 = m_vertices[i3].texCoord;
	    
		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;
	    
		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;
	    
		float r = 1.0F / (s1 * t2 - s2 * t1);
		Vec3 sdir = Vec3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
		Vec3 tdir = Vec3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);
	    
		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for (long a = 0; a < m_numVertices; a++)
	{
		Vec3 n = m_vertices[a].normal;
		Vec3 t = tan1[a];
		// Gram-Schmidt orthogonalize
		Vec3 tangent = (t - n * D3DXVec3Dot(&n, &t));
		D3DXVec3Normalize(&tangent, &tangent);
	    
		// Calculate handedness
		Vec3 NcrossT;
		D3DXVec3Cross(&NcrossT, &n, &t);
		float handednes = (D3DXVec3Dot(&NcrossT, &tan2[a]) < 0.0F) ? -1.0F : 1.0F;
		Vec3 binormal = NcrossT * handednes;
		D3DXVec3Normalize(&binormal, &binormal);
		
		m_vertices[a].tangent	= tangent;
		m_vertices[a].binormal	= binormal;
	}

	delete[] tan1;

	/////////////////////////

	m_indexTextureSize = (size*m_indTexExp);
	m_indexTextureData = new u32[m_indexTextureSize*m_indexTextureSize];
	memcpy(m_indexTextureData, lockedRect.pBits, m_indexTextureSize*m_indexTextureSize*sizeof(u32));
	
	/* END PREPARING RAW DATA */
	/////////////////////////////////////////////////////////////////////////////////////////////////


	//hmInit
	m_position = pos;
	m_hmInit = 1;
	m_terrains.resize(((size)/m_sectorSize));
	for(u32 i = 0; i < ((size)/m_sectorSize); i++)
		m_terrains[i].resize(((size)/m_sectorSize));
	
	//Stats
	gEngine.kernel->log->prnEx(LT_INFO, "TerrainManager", "Generating terrain sectors...");
	gEngine.kernel->log->prnEx(LT_INFO, "TerrainManager", " HeightMap  : '%s'", texHmName.c_str());
	gEngine.kernel->log->prnEx(LT_INFO, "TerrainManager", " IndexMap   : '%s'", texIndName.c_str());
	gEngine.kernel->log->prnEx(LT_INFO, "TerrainManager", " HM Size    : %u (%u)", m_size, size);
	gEngine.kernel->log->prnEx(LT_INFO, "TerrainManager", " SectorSize : %u", m_sectorSize);
	gEngine.kernel->log->prnEx(LT_INFO, "TerrainManager", " Position   : Vec3(%f,%f,%f)", pos.x, pos.y, pos.z);

	//Create all sectors
	u32 sectors = 0;
	for(u32 secX = 0; secX < ((m_size-1)/m_sectorSize); secX++)
	{
		for(u32 secY = 0; secY < ((m_size-1)/m_sectorSize); secY++)
		{
			Terrain * t = new Terrain(m_vertices, 0, secX, secY, m_sectorSize, m_size, m_hmGeoStep);
			m_terrains[secX][secY] = t;
			sectors++;
			if(m_scene)
				m_scene->add(t);
		}
	}

	for(u32 secX = 0; secX < ((m_size-1)/m_sectorSize); secX++)
	{
		for(u32 secY = 0; secY < ((m_size-1)/m_sectorSize); secY++)
		{
			Terrain* top	= (secY>0)?(m_terrains[secX][secY-1]):(0);
			Terrain* left	= (secX>0)?(m_terrains[secX-1][secY]):(0);
			Terrain* right	= (secX<(((m_size-1)/m_sectorSize)-1))?(m_terrains[secX+1][secY]):(0);
			Terrain* bottom	= (secY<(((m_size-1)/m_sectorSize)-1))?(m_terrains[secX][secY+1]):(0);

			m_terrains[secX][secY]->m_top		= top;
			m_terrains[secX][secY]->m_left		= left;
			m_terrains[secX][secY]->m_right		= right;
			m_terrains[secX][secY]->m_bottom	= bottom;
		}
	}

	if(!updateIndexTexture())
	{ gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Couldn't update index texture."); return 0; }

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// CLEAR ALL
	
	rind->get()->UnlockRect(0);
	rhm->get()->UnlockRect(0);
	gEngine.resMgr->release(rhm);
	gEngine.resMgr->release(rind);

	gEngine.kernel->log->prnEx(LT_SUCCESS, "TerrainManager", "Generating terrain sectors finished. Created %u sectors.", (((size)/m_sectorSize)*((size)/m_sectorSize)));
	return 1;
}

void TerrainManager::update(ICamera *cam)
{
	if(!m_terrains.size())return;
	for(u32 secX = 0; secX < ((m_size-1)/m_sectorSize); secX++)
	{
		for(u32 secY = 0; secY < ((m_size-1)/m_sectorSize); secY++)
		{
			m_terrains[secX][secY]->update(cam);
		}
	}
}


#define TERHM_HEADER_SIGNATURE "XGINETHM"
#define TERHM_HEADER_VERSION	0x00200000

struct TERHM_HEADER
{
	c8		signature[8];
	u32		version;
	u32		size;
	u32		sectorSize;
	Vec3	position;
	u32		compBuffVecSize;
	u32		vec_compressed;
};

u32 TerrainManager::save(const string& hmPackName)
{
	TERHM_HEADER header;

	ofstream file;
	file.open(hmPackName.c_str(), ios::trunc | ios::binary);
	if(!file.is_open())
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Could not open file %s to save terrain.", hmPackName.c_str());
		return 0;
	}

	memcpy(header.signature, TERHM_HEADER_SIGNATURE, 8);
	header.version = TERHM_HEADER_VERSION;
	header.size = this->m_size;
	header.position = this->m_position;
	header.sectorSize = this->m_sectorSize;

	u32 maxsize = Compression::getMaxCompressedSize(COMP_ZLIB, m_numVertices * sizeof(TerrainVertex));
	u8* vertices = new u8[maxsize];

	header.compBuffVecSize = maxsize;

	if(Compression::comp(COMP_ZLIBS, (u8*)m_vertices, (u8*)vertices, m_numVertices * sizeof(TerrainVertex), &header.compBuffVecSize))
		header.vec_compressed = 1;
	else
		header.vec_compressed = 0;

	file.write((c8*)&header, sizeof(TERHM_HEADER));
	if(header.vec_compressed)
		file.write((c8*)vertices, header.compBuffVecSize);
	else
		file.write((c8*)m_vertices, m_numVertices * sizeof(TerrainVertex));

	delete(vertices);

	file.close();

	gEngine.kernel->log->prnEx(LT_SUCCESS, "TerrainManager", "Saved bin terrain to %s.", hmPackName.c_str());
	return 1;
}

u32 TerrainManager::loadHm(const string& hmPackName)
{
	cleanup();
	TERHM_HEADER header;

	ifstream file;
	file.open(hmPackName.c_str(), ios::binary);
	if(!file.is_open())
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Couldn't open terrain bin '%s'.", hmPackName.c_str());
	}

	file.seekg(0,std::ios::beg);
	file.read((c8*)&header, sizeof(TERHM_HEADER));

	if(memcmp(header.signature, TERHM_HEADER_SIGNATURE, 8) != 0)
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Wrong signature of terrain bin '%s'.", hmPackName.c_str());
		return 0;
	}

	if(header.version != TERHM_HEADER_VERSION)
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Wrong version of terrain bin '%s'.", hmPackName.c_str());
		return 0;
	}

	this->m_size = header.size;
	this->m_position = header.position;
	this->m_sectorSize = header.sectorSize;

	m_numVertices = m_size * m_size;
	m_vertices	= new TerrainVertex[m_numVertices];

	if(header.vec_compressed)
	{
		u8* vertices = new u8[header.compBuffVecSize];
		file.read((c8*)vertices, header.compBuffVecSize);
		Compression::decomp(COMP_ZLIBS, (u8*)vertices, (u8*)m_vertices, header.compBuffVecSize, m_numVertices * sizeof(TerrainVertex));
		delete(vertices);
	}
	else
		file.read((c8*)m_vertices, m_numVertices * sizeof(TerrainVertex));

	
	file.close();

	m_numIndices = (m_size-1)*(m_size-1)*3*2;
	m_indices  = new u32[m_numIndices];
	u32  i1, i2, i3;
	u32 count = 0;
	for (u32 y = 0; y < (m_size-1); y++)
	{
		for (u32 x = 0; x < (m_size-1); x++)
		{
			u32 index = y * m_size + x;
			i1 = index + 1; i2 = index; i3 = index + m_size;
			m_indices[count++] = i1; m_indices[count++] = i2; m_indices[count++] = i3;
			i1 = index + 1; i2 = index + m_size; i3 = index + m_size + 1;
			m_indices[count++] = i1; m_indices[count++] = i2; m_indices[count++] = i3;
		}
	}

	m_hmInit = 1;
	m_terrains.resize(((m_size-1)/m_sectorSize));
	for(u32 i = 0; i < ((m_size-1)/m_sectorSize); i++)
		m_terrains[i].resize(((m_size-1)/m_sectorSize));
	
	//Stats
	gEngine.kernel->log->prnEx(LT_INFO, "TerrainManager", "Loading terrain sectors...");
	gEngine.kernel->log->prnEx(LT_INFO, "TerrainManager", " HeightMap  : '%s'", hmPackName.c_str());
	gEngine.kernel->log->prnEx(LT_INFO, "TerrainManager", " HM Size    : %u", m_size);
	gEngine.kernel->log->prnEx(LT_INFO, "TerrainManager", " SectorSize : %u", m_sectorSize);
	gEngine.kernel->log->prnEx(LT_INFO, "TerrainManager", " Position   : Vec3(%f,%f,%f)", m_position.x, m_position.y, m_position.z);

	//Create all sectors
	u32 sectors = 0;
	for(u32 secX = 0; secX < ((m_size-1)/m_sectorSize); secX++)
	{
		for(u32 secY = 0; secY < ((m_size-1)/m_sectorSize); secY++)
		{
			Terrain * t = new Terrain(m_vertices, 0, secX, secY, m_sectorSize, m_size, m_hmGeoStep);
			m_terrains[secX][secY] = t;
			sectors++;
			if(m_scene && t)
				m_scene->add(t);
		}
	}

	for(u32 secX = 0; secX < ((m_size-1)/m_sectorSize); secX++)
	{
		for(u32 secY = 0; secY < ((m_size-1)/m_sectorSize); secY++)
		{
			Terrain* top	= (secY>0)?(m_terrains[secX][secY-1]):(0);
			Terrain* left	= (secX>0)?(m_terrains[secX-1][secY]):(0);
			Terrain* right	= (secX<(((m_size-1)/m_sectorSize)-1))?(m_terrains[secX+1][secY]):(0);
			Terrain* bottom	= (secY<(((m_size-1)/m_sectorSize)-1))?(m_terrains[secX][secY+1]):(0);

			m_terrains[secX][secY]->m_top		= top;
			m_terrains[secX][secY]->m_left		= left;
			m_terrains[secX][secY]->m_right		= right;
			m_terrains[secX][secY]->m_bottom	= bottom;
		}
	}

	gEngine.kernel->log->prnEx(LT_SUCCESS, "TerrainManager", "Loading terrain sectors finished. Created %u sectors.", (((m_size-1)/m_sectorSize)*((m_size-1)/m_sectorSize)));
	return 1;
}


#define TERIM_HEADER_SIGNATURE "XGINETIM"
#define TERIM_HEADER_VERSION	  0x00200000

struct TERIM_HEADER
{
	c8		signature[8];
	u32		version;
	u32		size;
	u32		compressed;
	u32		compBuffSize;
};

u32 TerrainManager::saveIndTex(const string& texIndName)
{
	TERIM_HEADER header;

	ofstream file;
	file.open(texIndName.c_str(), ios::trunc | ios::binary);
	if(!file.is_open())
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Could not open file %s to save terrain index texture.", texIndName.c_str());
		return 0;
	}

	memcpy(header.signature, TERIM_HEADER_SIGNATURE, 8);
	header.version = TERIM_HEADER_VERSION;
	header.size = this->m_indexTextureSize;

	header.compBuffSize = Compression::getMaxCompressedSize(COMP_ZLIB, m_indexTextureSize * m_indexTextureSize * sizeof(u32));
	u8* compData = new u8[header.compBuffSize];

	if(Compression::comp(COMP_ZLIBS, (u8*)m_indexTextureData, (u8*)compData, m_indexTextureSize * m_indexTextureSize * sizeof(u32), &header.compBuffSize))
		header.compressed = 1;
	else
		header.compressed = 0;

	file.write((c8*)&header, sizeof(TERIM_HEADER));

	if(header.compressed)
		file.write((c8*)compData, header.compBuffSize);
	else
		file.write((c8*)m_indexTextureData, m_indexTextureSize * m_indexTextureSize * sizeof(u32));

	file.close();

	delete(compData);

	gEngine.kernel->log->prnEx(LT_SUCCESS, "TerrainManager", "Saved bin terrain index texture to %s.", texIndName.c_str());
	return 1;
}

u32 TerrainManager::loadIndTex(const string& texIndName)
{
	if(!m_hmInit)
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Could not load index texture '%s' before loading terrain bin.", texIndName.c_str());
		return 0;
	}

	TERIM_HEADER header;

	ifstream file;
	file.open(texIndName.c_str(), ios::binary);
	if(!file.is_open())
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Couldn't open terrain index texture '%s'.", texIndName.c_str());
	}

	file.seekg(0,std::ios::beg);
	file.read((c8*)&header, sizeof(TERIM_HEADER));

	if(memcmp(header.signature, TERIM_HEADER_SIGNATURE, 8) != 0)
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Wrong signature of terrain index texture '%s'.", texIndName.c_str());
		return 0;
	}

	if(header.version != TERIM_HEADER_VERSION)
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Wrong version of terrain index texture '%s'.", texIndName.c_str());
		return 0;
	}

	this->m_indexTextureSize = header.size;

	m_indexTextureData = new u32[m_indexTextureSize * m_indexTextureSize];

	if(header.compressed)
	{
		u8* compData = new u8[header.compBuffSize];
		file.read((c8*)compData, header.compBuffSize);
		Compression::decomp(COMP_ZLIBS, (u8*)compData, (u8*)m_indexTextureData, header.compBuffSize, m_indexTextureSize * m_indexTextureSize * sizeof(u32));
		delete(compData);
	}
	else
		file.read((c8*)m_indexTextureData, m_indexTextureSize * m_indexTextureSize * sizeof(u32));

	if(!updateIndexTexture())
	{ gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Couldn't update index texture."); return 0; }

	gEngine.kernel->log->prnEx(LT_SUCCESS, "TerrainManager", "Loaded and updated terrain index texture '%s'.", texIndName.c_str());
	return 1;
}

u32 TerrainManager::load(const string& hmPackName, const string& texIndName)
{
	if(!loadHm(hmPackName)){ gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Terrain has not been loaded."); return 0; }
	if(!loadIndTex(texIndName)){ gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Terrain has not been loaded."); return 0; }
	return 1;
}

u32 TerrainManager::updateBinLoad(const string& texHmName,  const string& texIndName, const string& binHmName, const string& binIndName, u32 size, Vec3 pos)
{
	u32 updateHm	= 0;
	u32 updateInd	= 0;

	string texHm = g_path_textures + texHmName;
	string texInd = g_path_textures + texIndName;

	if(gEngine.kernel->fs->exists(texHm) && gEngine.kernel->fs->exists(binHmName))
	{
		if( (GetFileAttributes(texHm.c_str()) != INVALID_FILE_ATTRIBUTES) && (GetFileAttributes(binHmName.c_str()) != INVALID_FILE_ATTRIBUTES))
		{
			struct _stat tex;	_stat(texHm.c_str(),	&tex);
			struct _stat bin;	_stat(binHmName.c_str(),	&bin);

			if(tex.st_mtime > bin.st_mtime)
				updateHm = 1;
		}
	}
	else if(gEngine.kernel->fs->exists(texHm) && !gEngine.kernel->fs->exists(binHmName))updateHm = 1;

	if(gEngine.kernel->fs->exists(texInd) && gEngine.kernel->fs->exists(binIndName))
	{
		if( (GetFileAttributes(texInd.c_str()) != INVALID_FILE_ATTRIBUTES) && (GetFileAttributes(binIndName.c_str()) != INVALID_FILE_ATTRIBUTES))
		{
			struct _stat tex;	_stat(texInd.c_str(),	&tex);
			struct _stat bin;	_stat(binIndName.c_str(),	&bin);

			if(tex.st_mtime > bin.st_mtime)
				updateInd = 1;
		}
	}
	else if(gEngine.kernel->fs->exists(texInd) && !gEngine.kernel->fs->exists(binIndName))updateInd = 1;

	if(updateHm || updateInd)
	{
		create(texHmName, texIndName, size, pos);
		save(binHmName);
		saveIndTex(binIndName);
	}
	else
	{
		load(binHmName, binIndName);
	}
	
	return 1;
}

u32 TerrainManager::saveHmAsXMesh(const string& fileName)
{
	ofstream fout;
	fout.open(fileName.c_str(), ios::trunc);
	if(fout.is_open())
	{
		for(u32 i = 0; i < m_numVertices; i++)
			fout << "v " << m_vertices[i].pos.x << " " << m_vertices[i].pos.y << " " << m_vertices[i].pos.z << endl;
		for(u32 i = 0; i < m_numIndices/3; i++)
			fout << "f v" << m_indices[i] + 1 << " v" << m_indices[i+1] + 1 << " v"<< m_indices[i+2] + 1 << endl;

		fout.close();
		return 1;
	}
	return 0;
}

u32 TerrainManager::updateIndexTexture()
{
	u32 indTexCount = ( (m_size-1)/m_sectorSize );
	u32 indTexSize  = ( m_indexTextureSize ) / ( (m_size-1)/m_sectorSize );
	if(!m_indexTextures)
	{
		m_indexTextures				= new LPDIRECT3DTEXTURE9[indTexCount*indTexCount];
		m_indexTexturesLocked		= new u32[indTexCount*indTexCount];
		m_indexTexturesLockedRect	= new D3DLOCKED_RECT[indTexCount*indTexCount];

		for(u32 y = 0; y < indTexCount; y++)
		{
			for(u32 x = 0; x < indTexCount; x++)
			{
				m_indexTextures[y*indTexCount+x]		= 0;
				m_indexTexturesLocked[y*indTexCount+x]	= 0;
			}
		}
	}

	D3DLOCKED_RECT lr;
	for(u32 y = 0; y < indTexCount; y++)
	{
		for(u32 x = 0; x < indTexCount; x++)
		{
			if(m_indexTextures[y*indTexCount+x])m_indexTextures[y*indTexCount+x]->Release();m_indexTextures[y*indTexCount+x]=0;

			if(FAILED(D3DXCreateTexture(gEngine.device->getDev(), indTexSize, indTexSize, D3DX_DEFAULT, D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP ,D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_indexTextures[y*indTexCount+x])))
				{ gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Could not create index texture for terrain"); m_indexTextures[y*indTexCount+x] = 0; return 0; }
			else
			{
				if(!FAILED(m_indexTextures[y*indTexCount+x]->LockRect(0, &lr, 0, D3DLOCK_DISCARD)))
				{
					for(u32 n = 0; n < indTexSize; n++)
					{
						for(u32 m = 0; m < indTexSize; m++)
						{
							((u32*)lr.pBits)[n*indTexSize+m] = m_indexTextureData[(y*m_indexTextureSize/indTexCount+n)*m_indexTextureSize+(m+x*m_indexTextureSize/indTexCount)];
						}
					}
					m_indexTextures[y*indTexCount+x]->UnlockRect(0);
				}
				else { gEngine.kernel->log->prnEx(LT_ERROR, "TerrainManager", "Could not copy index texture for terrain"); return 0; }
			}
		}
	}

	for(u32 secX = 0; secX < ((m_size-1)/m_sectorSize); secX++)
	{
		for(u32 secY = 0; secY < ((m_size-1)/m_sectorSize); secY++)
		{
			m_terrains[secX][secY]->m_indexTexture = m_indexTextures[secY*indTexCount+secX];

			m_indexTextures[secY*indTexCount+secX]->GenerateMipSubLevels();
		}
	}

	return 1;
}

void TerrainManager::lockIndexTexture(Vec4 rect)
{
	u32 indTexCount = ( (m_size-1)/m_sectorSize );
	Vec2 start = Vec2(min(rect.x, rect.z), min(rect.y, rect.w));
	Vec2 end = Vec2(max(rect.x, rect.z), max(rect.y, rect.w));
	start = Vec2((u32)(start.x/(m_sectorSize*m_hmGeoStep)),(u32)(start.y/(m_sectorSize*m_hmGeoStep)));
	end = Vec2((u32)(end.x/(m_sectorSize*m_hmGeoStep)),(u32)(end.y/(m_sectorSize*m_hmGeoStep)));

	u32 locked = 0;

	for(u32 y = start.y; y <= end.y; y++)
	{
		for(u32 x = start.x; x <= end.x; x++)
		{
			if(!m_indexTexturesLocked[y*indTexCount+x])
			{
				if(m_indexTextures[y*indTexCount+x] && !FAILED(m_indexTextures[y*indTexCount+x]->LockRect(0, &m_indexTexturesLockedRect[y*indTexCount+x], 0, 0)))
				{
					m_indexTexturesLocked[y*indTexCount+x] = 1;
					locked++;
				}
				else
					m_indexTexturesLocked[y*indTexCount+x] = 0;
			}

			if(locked)return;
		}
	}

	gEngine.renderer->addTxt("Locked terrain: %u", locked);
}

void TerrainManager::unlockIndexTexture(Vec4 rect)
{
	u32 indTexCount = ( (m_size-1)/m_sectorSize );
	Vec2 start = Vec2(min(rect.x, rect.z), min(rect.y, rect.w));
	Vec2 end = Vec2(max(rect.x, rect.z), max(rect.y, rect.w));
	start = Vec2((u32)(start.x/(m_sectorSize*m_hmGeoStep)),(u32)(start.y/(m_sectorSize*m_hmGeoStep)));
	end = Vec2((u32)(end.x/(m_sectorSize*m_hmGeoStep)),(u32)(end.y/(m_sectorSize*m_hmGeoStep)));

	for(u32 y = start.y; y <= end.y; y++)
	{
		for(u32 x = start.x; x <= end.x; x++)
		{
			if(m_indexTexturesLocked[y*indTexCount+x] && m_indexTextures[y*indTexCount+x])
			{
				m_indexTextures[y*indTexCount+x]->UnlockRect(0);		
			}

			m_indexTexturesLocked[y*indTexCount+x] = 0;
		}
	}
}

void TerrainManager::setIndexTextureTexel(f32 x, f32 y, Vec4* color)
{
	u32 indTexCount = ( (m_size-1)/m_sectorSize );
	u32 indTexSize  = ( m_indexTextureSize ) / ( (m_size-1)/m_sectorSize );
	Vec2 point = Vec2((u32)((f32)x/(f32)(m_sectorSize*m_hmGeoStep)),(u32)((f32)y/(f32)(m_sectorSize*m_hmGeoStep)));
	Vec2 texCoord = 
		Vec2((f32)((f32)x-(f32)point.x*(f32)((f32)m_sectorSize*(f32)m_hmGeoStep))*((f32)m_indTexExp/(f32)m_hmGeoStep), 
		     (f32)((f32)y-(f32)point.y*(f32)((f32)m_sectorSize*(f32)m_hmGeoStep))*((f32)m_indTexExp/(f32)m_hmGeoStep));
	
	if(m_indexTextures[(u32)((u32)point.y*(u32)indTexCount+(u32)point.x)] && m_indexTexturesLocked[(u32)(point.y*(u32)indTexCount+(u32)point.x)])
	{
		((u32*)m_indexTexturesLockedRect[(u32)((u32)point.y*(u32)indTexCount+(u32)point.x)].pBits)[(u32)((u32)texCoord.y*indTexSize+(u32)texCoord.x)] = D3DCOLOR_ARGB((u32)color->w*255, (u32)color->x*255, (u32)color->y*255, (u32)color->z*255);
	}
}
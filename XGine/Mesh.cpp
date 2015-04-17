#include "Mesh.h"
#include <sys/types.h> // dla _stat
#include <sys/stat.h> // dla _stat

void RawMesh::release()
{
	DXRELEASE(ib);
	DXRELEASE(vb);
	primType		= D3DPT_TRIANGLELIST;
	vertexStride	= 0;
	fvf				= 0;
	numVertices		= 0;
	numFaces		= 0;
	if(nxVertices)	delete[] nxVertices; 
	nxVertices		= 0;
	if(nxIndices)	delete[] nxIndices; 
	nxIndices		= 0;
	if(vertices)	delete[] vertices; 
	vertices		= 0;
}

#define XBIN_HEADER_SIGNATURE "XBIN"
#define XBIN_HEADER_VERSION   0x00000010

struct XBIN_HEADER
{
	c8	signature[4];
	u32 version;
};

u32 RawMesh::loadBin(string fileName)
{
	Buffer buffer;
	if(!gEngine.kernel->fs->load(fileName, buffer))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "RawMesh", "Couldn't load mesh '%s' buffer.", fileName.c_str());
		return 0;
	}

	XBIN_HEADER header;
	u32 currentPos = 0;
	memget(buffer, currentPos, (c8*)&header,		sizeof(header));

	if(memcmp(header.signature, XBIN_HEADER_SIGNATURE, 4) != 0 || header.version != XBIN_HEADER_VERSION)
	{
		gEngine.kernel->mem->freeBuff(buffer);
		gEngine.kernel->log->prnEx(LT_ERROR, "RawMesh", "Wrong signature of cache mesh '%s'", fileName.c_str());
		return 0;
	}

	memget(buffer, currentPos, (c8*)&numVertices,	sizeof(u32));
	memget(buffer, currentPos, (c8*)&numFaces,		sizeof(u32));
	memget(buffer, currentPos, (c8*)&boundingBox,	sizeof(BoundingBox));

	vertices	= new VertexNTB[numVertices];
	indices		= new WORD[numFaces * 3];
	nxIndices	= new u32[numFaces * 3];
	nxVertices	= new Vec3[numVertices];
	
	memget(buffer, currentPos, (c8*)vertices,		sizeof(VertexNTB)	* numVertices);
	memget(buffer, currentPos, (c8*)indices,			sizeof(WORD)		* numFaces * 3);
	memget(buffer, currentPos, (c8*)nxVertices,		sizeof(Vec3)		* numVertices);
	memget(buffer, currentPos, (c8*)nxIndices,		sizeof(u32)			* numFaces * 3);

	gEngine.kernel->mem->freeBuff(buffer);

	gEngine.kernel->log->prnEx(LT_SUCCESS, "RawMesh", "Loaded raw mesh from cache '%s'", fileName.c_str());
	return 1;
}

u32 RawMesh::saveBin(string fileName, u64 modDate, string cacheName)
{
	if(vertices)delete [] vertices;
	vertices = new VertexNTB[numVertices];
	VertexNTB*	ptrVertices = 0;
	vb->Lock(0, 0, (void**)&ptrVertices, 0);
	memcpy(vertices, ptrVertices, sizeof(VertexNTB) * numVertices);
	vb->Unlock();

	if(indices)delete [] indices;
	indices = new WORD[numFaces * 3];
	WORD* ptrIndices = 0;
	ib->Lock(0, 0, (void**)&ptrIndices, 0);
	memcpy(indices, ptrIndices, sizeof(WORD) * numFaces * 3);
	ib->Unlock();

	XBIN_HEADER header;
	memcpy(header.signature, XBIN_HEADER_SIGNATURE, 4);
	header.version = XBIN_HEADER_VERSION;

	u32 bufferSize = sizeof(header) + 2 * sizeof(u32) + sizeof(BoundingBox) + sizeof(VertexNTB)	* numVertices + sizeof(WORD) * numFaces * 3 + sizeof(Vec3) * numVertices + sizeof(u32) * numFaces * 3;
	u32 currentPos = 0;
	Buffer buffer;
	gEngine.kernel->mem->allocBuff(buffer, bufferSize, FILELINE);
	memadd(buffer, currentPos, (c8*)&header,		sizeof(header));
	memadd(buffer, currentPos, (c8*)&numVertices,	sizeof(u32));
	memadd(buffer, currentPos, (c8*)&numFaces,		sizeof(u32));
	memadd(buffer, currentPos, (c8*)&boundingBox,	sizeof(BoundingBox));
	memadd(buffer, currentPos, (c8*)vertices,		sizeof(VertexNTB)	* numVertices);
	memadd(buffer, currentPos, (c8*)indices,		sizeof(WORD)		* numFaces * 3);
	memadd(buffer, currentPos, (c8*)nxVertices,		sizeof(Vec3)		* numVertices);
	memadd(buffer, currentPos, (c8*)nxIndices,		sizeof(u32)			* numFaces * 3);

	FSPakFileInfo file;
	file.compType = g_resourcesCacheCompression;
	file.packedFileName = file.fileName = fileName;
	file.buffer = new Buffer();
	file.buffer->data = buffer.data;
	file.buffer->size = buffer.size;
	file.mdate = modDate;
	gEngine.kernel->fs->addToArchive(cacheName, file);
	delete(file.buffer);
	gEngine.kernel->mem->freeBuff(buffer);
	
	delete [] vertices;
	vertices = 0;
	delete [] indices;
	indices = 0;

	gEngine.kernel->log->prnEx(LT_SUCCESS, "RawMesh", "Saved raw mesh to cache '%s'", fileName.c_str());
	return 1;
}

u32 RawMesh::saveBin(LPD3DXMESH mesh, string fileName, u64 modDate, string cacheName)
{
	//PREPARE
	mesh->GetIndexBuffer(&ib);
	mesh->GetVertexBuffer(&vb);
	primType		= D3DPT_TRIANGLELIST;
	vertexStride	= mesh->GetNumBytesPerVertex();
	numFaces		= mesh->GetNumFaces();
	numVertices		= mesh->GetNumVertices();
	char* buff = 0;
	mesh->LockVertexBuffer(0, (void**)&buff);
	D3DXVECTOR3 min, max;
	D3DXComputeBoundingBox((D3DXVECTOR3*)buff, numVertices, vertexStride, &min, &max);
	boundingBox.reset(min, max);
	mesh->UnlockVertexBuffer();
	struct Vertex
	{
		Vec3	pos;
		Vec2	texCoord;
		Vec3	normal;
		Vec3	tangent;
		Vec3	binormal;
	};
	Vertex*		Vertices;
	WORD*		Indices;
	vb->Lock(0, 0, (void**)&Vertices, D3DLOCK_DISCARD);
	ib->Lock(0, 0, (void**)&Indices, 0);
	nxVertices	= new Vec3[numVertices];
	nxIndices	= new u32[numFaces * 3];
	for (long a = 0; a < numVertices; a++)
		nxVertices[a] = Vertices[a].pos;
	for (long a = 0; a < numFaces; a++)
	{
		nxIndices[a*3]	= Indices[a*3];
		nxIndices[a*3+1]= Indices[a*3+1];
		nxIndices[a*3+2]= Indices[a*3+2];
	}
	vb->Unlock();
	ib->Unlock();

	if(vertices)delete [] vertices;
	vertices = new VertexNTB[numVertices];
	VertexNTB*	ptrVertices = 0;
	vb->Lock(0, 0, (void**)&ptrVertices, 0);
	memcpy(vertices, ptrVertices, sizeof(VertexNTB) * numVertices);
	vb->Unlock();

	if(indices)delete [] indices;
	indices = new WORD[numFaces * 3];
	WORD* ptrIndices = 0;
	ib->Lock(0, 0, (void**)&ptrIndices, 0);
	memcpy(indices, ptrIndices, sizeof(WORD) * numFaces * 3);
	ib->Unlock();

	XBIN_HEADER header;
	memcpy(header.signature, XBIN_HEADER_SIGNATURE, 4);
	header.version = XBIN_HEADER_VERSION;

	u32 bufferSize = sizeof(header) + 2 * sizeof(u32) + sizeof(BoundingBox) + sizeof(VertexNTB)	* numVertices + sizeof(WORD) * numFaces * 3 + sizeof(Vec3) * numVertices + sizeof(u32) * numFaces * 3;
	u32 currentPos = 0;
	Buffer buffer;
	gEngine.kernel->mem->allocBuff(buffer, bufferSize, FILELINE);
	memadd(buffer, currentPos, (c8*)&header,		sizeof(header));
	memadd(buffer, currentPos, (c8*)&numVertices,	sizeof(u32));
	memadd(buffer, currentPos, (c8*)&numFaces,		sizeof(u32));
	memadd(buffer, currentPos, (c8*)&boundingBox,	sizeof(BoundingBox));
	memadd(buffer, currentPos, (c8*)vertices,		sizeof(VertexNTB)	* numVertices);
	memadd(buffer, currentPos, (c8*)indices,		sizeof(WORD)		* numFaces * 3);
	memadd(buffer, currentPos, (c8*)nxVertices,		sizeof(Vec3)		* numVertices);
	memadd(buffer, currentPos, (c8*)nxIndices,		sizeof(u32)			* numFaces * 3);

	FSPakFileInfo file;
	file.compType = g_resourcesCacheCompression;
	file.packedFileName = file.fileName = fileName;
	file.buffer = new Buffer();
	file.buffer->data = buffer.data;
	file.buffer->size = buffer.size;
	file.mdate = modDate;
	gEngine.kernel->fs->addToArchive(cacheName, file);
	delete(file.buffer);
	gEngine.kernel->mem->freeBuff(buffer);

	delete [] vertices;
	vertices = 0;
	delete [] indices;
	indices = 0;

	gEngine.kernel->log->prnEx(LT_SUCCESS, "RawMesh", "Saved raw mesh to cache '%s'", fileName.c_str());
	return 1;
}

////////////////////////////////////////////////////////////

u32 g_numMeshes = 1;

Mesh::Mesh()
	: m_pMesh(0), m_instanceVB(0), m_instanceData(0), m_instanceVBSize(64), m_allocFailed(0), m_instanceMustResize(0), meshBaseCheck(0), meshBaseResult(0), lodGenerated(0)
{ 
	m_ID = g_numMeshes++;
	ZeroMemory(&m_rawMesh, sizeof(RawMesh));
	_start = 0;
	_frame = 0;
	_count = 0;
}

Mesh::~Mesh()
{
	if(buffer)gEngine.kernel->mem->freeBuff(*buffer); delete(buffer); buffer = 0;
	release();
}

void Mesh::release()
{
	DXRELEASE(m_instanceVB);
	if(m_instanceData) delete [] (m_instanceData); m_instanceData = 0;
	DXRELEASE(m_pMesh);
	m_rawMesh.release();
}

u32 Mesh::reallocInstancVB()
{
	DXRELEASE(m_instanceVB);
	if(m_instanceData) delete [] (m_instanceData); m_instanceData = 0;

	if(FAILED(gEngine.device->getDev()->CreateVertexBuffer(m_instanceVBSize * sizeof(Vec4), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_instanceVB, NULL )))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "Mesh", "Mesh::m_instanceVB: Unable to create instancing vertex buffer!");
		m_instanceVB = 0;
		m_allocFailed = 1;
		return 0;
	}

	m_instanceData = new InstanceData[m_instanceVBSize];

	if(!m_instanceData)
	{
		DXRELEASE(m_instanceVB);
		if(m_instanceData) delete [] (m_instanceData); m_instanceData = 0;
		gEngine.kernel->log->prnEx(LT_ERROR, "Mesh", "Mesh::m_instanceData: Unable to create instancing data buffer!");
		m_instanceVB = 0;
		m_allocFailed = 1;
		return 0;
	}

	gEngine.renderer->addTxtF("Reallocated instance buffer to %u for mesh id %u", m_instanceVBSize, m_ID);

	m_allocFailed = 0;
	return 1;
}

static const u32		g_lodCount		= 5;
static const f32		g_lodPercent[5]	= { 0.95f, 0.90f, 0.80f, 0.65f, 0.50f };
static const string		g_lodNames[5]	= { "_lod1", "_lod2", "_lod3", "_lod4", "_lod5" };

u32 Mesh::isBaseMesh()
{
	if(meshBaseCheck && meshBaseResult)return 1;
	else if(!meshBaseCheck)
	{
		meshBaseCheck = 1;
		meshBaseResult = 0;
		string name = getWithoutExt(strFilename);
		if(name.size() < 5){ meshBaseResult = 1; return 1; }
		for(u32 i = 0; i < g_lodCount; i++)
			if(name.substr( name.size()-5 ) == g_lodNames[i])return 0;
		meshBaseResult = 1;
		return 1;
	}
	return 0;
}	

u32 Mesh::getNumLODs()
{
	if(!generateLods(1))return 0;
	return lodGenerated;
}

string Mesh::getLODName(u32 lod)
{
	if(lod >= 1 && lod < g_lodCount)return getWithoutExt(strFilename) + g_lodNames[lod] + ".xbin";
	else return "";
}

Mesh* Mesh::loadLOD(u32 lod)
{
	if(!lodGenerated)return 0;
	return gEngine.resMgr->load<Mesh>(getLODName(lod));
}

u32 Mesh::generateLods(u32 canOnlyCheck)
{
	if(lodGenerated || !isBaseMesh())return 1;
	u32 mustGenerate = 0;
	string name = getWithoutExt(strFilename);
	if(!canOnlyCheck)mustGenerate = 1;
	else
	{
		for(u32 i = 0; i < g_lodCount; i++)
		{
			if(!gEngine.kernel->fs->exists("data/" + name + g_lodNames[i] + ".xbin"))mustGenerate = 1;
			else lodGenerated++;
		}
	}

	if(!mustGenerate)return 1;
	lodGenerated = 0;

	u32 numFaces = m_rawMesh.numFaces;
	LPD3DXMESH mesh;
	DWORD* rgdwAdjacency = NULL;
	DWORD* adj = NULL;
	rgdwAdjacency = new DWORD[ m_pMesh->GetNumFaces() * 3 ];
	if( rgdwAdjacency == NULL )return 0;
	m_pMesh->GenerateAdjacency( 1e-6f, rgdwAdjacency );

	u32 lastNumFaces = numFaces;
	
	for(u32 i = 0; i < g_lodCount; i++)
	{
		if(SUCCEEDED(D3DXSimplifyMesh(m_pMesh, rgdwAdjacency, NULL, NULL, numFaces * g_lodPercent[i] + 2, D3DXMESHSIMP_FACE, &mesh)))
		{
			RawMesh rm;
			rm.release();
			if(lastNumFaces > mesh->GetNumFaces() + 10)
			{
				lastNumFaces = mesh->GetNumFaces();
				adj = new DWORD[ mesh->GetNumFaces() * 3 ];
				mesh->GenerateAdjacency( 1e-6f, adj );
				mesh->OptimizeInplace( D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, adj, NULL, NULL, NULL );
				delete [] adj;
				rm.saveBin(mesh, "data/" + name + g_lodNames[i] + ".xbin", modDate, getCache());
				rm.release();
				lodGenerated++;
				gEngine.kernel->log->prnEx(LT_INFO, "Mesh", "Generated LOD%u (%u faces) form mesh '%s' (%u faces)", i+1, lastNumFaces, strFilename.c_str(), numFaces);
			}
			else
			{
				mesh->Release();
				mesh = 0;
				delete [] rgdwAdjacency;
				return 1;
			}
			mesh->Release();
			mesh = 0;
		}
	}

	delete [] rgdwAdjacency;

	return 1;
}

void Mesh::reGenerateTangents()
{	
	struct Vertex
	{
		Vec3	pos;
		Vec2	texCoord;
		Vec3	normal;
		Vec3	tangent;
		Vec3	binormal;
	};

	Vertex*		Vertices = 0;
	WORD*		Indices = 0;

	Vec3 *tan1 = new Vec3[m_rawMesh.numVertices * 2];
	Vec3 *tan2 = tan1 + m_rawMesh.numVertices;
	ZeroMemory(tan1, m_rawMesh.numVertices * sizeof(Vec3) * 2);

	m_rawMesh.vb->Lock(0, 0, (void**)&Vertices, 0);
	m_rawMesh.ib->Lock(0, 0, (void**)&Indices, 0);

	for (long a = 0; a < m_rawMesh.numFaces; a++)
	{
		long i1 = Indices[3*a+0];
		long i2 = Indices[3*a+1];
		long i3 = Indices[3*a+2];

		Vec3 v1 = Vertices[i1].pos;
		Vec3 v2 = Vertices[i2].pos;
		Vec3 v3 = Vertices[i3].pos;
	    
		Vec2 w1 = Vertices[i1].texCoord;
		Vec2 w2 = Vertices[i2].texCoord;
		Vec2 w3 = Vertices[i3].texCoord;
	    
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

	for (long a = 0; a < m_rawMesh.numVertices; a++)
	{
		Vec3 n = Vertices[a].normal;
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
		
		Vertices[a].tangent		= tangent;
		Vertices[a].binormal	= binormal;
	}

	delete[] tan1;

	/*std::vector< MeshMender::Vertex > theVerts;
	std::vector< unsigned int > theIndices;
	std::vector< unsigned int > mappingNewToOld;
	for(i = 0; i < m_pMesh->GetNumVertices(); ++i)
	{
		MeshMender::Vertex v;
		v.pos = Vertices[i].pos;
		v.s = Vertices[i].texCoord.x;
		v.t = Vertices[i].texCoord.y;
		v.normal = Vertices[i].normal;
		theVerts.push_back(v);
		numVert++;
	}

	for(DWORD ind= 0 ; ind< m_pMesh->GetNumFaces(); ++ind)
	{
		theIndices.push_back(Indices[3*ind]);
		theIndices.push_back(Indices[3*ind+1]);
		theIndices.push_back(Indices[3*ind+2]);
	}
	g_meshMender.Mend( theVerts,  theIndices, mappingNewToOld);
	for(i = 0; i < numVert; ++i)
	{
		Vertices[i].pos = theVerts[ mappingNewToOld[i] ].pos;
		Vertices[i].texCoord =  Vec2(theVerts[ mappingNewToOld[i] ].s, theVerts[ mappingNewToOld[i] ].t);
		Vertices[i].normal = theVerts[ mappingNewToOld[i] ].normal;
		Vertices[i].tangent = theVerts[ mappingNewToOld[i] ].tangent;
		Vertices[i].binormal = theVerts[ mappingNewToOld[i] ].binormal;
	}*/

	m_rawMesh.vb->Unlock();
	m_rawMesh.ib->Unlock();
	
}

void Mesh::generateVerticesIndicesLists()
{
	struct Vertex
	{
		Vec3	pos;
		Vec2	texCoord;
		Vec3	normal;
		Vec3	tangent;
		Vec3	binormal;
	};

	Vertex*		Vertices;
	WORD*		Indices;

	m_rawMesh.vb->Lock(0, 0, (void**)&Vertices, D3DLOCK_DISCARD);
	m_rawMesh.ib->Lock(0, 0, (void**)&Indices, 0);

	m_rawMesh.nxVertices	= new Vec3[m_rawMesh.numVertices];
	m_rawMesh.nxIndices		= new u32[m_rawMesh.numFaces * 3];

	for (long a = 0; a < m_rawMesh.numVertices; a++)
		m_rawMesh.nxVertices[a] = Vertices[a].pos;
	
	for (long a = 0; a < m_rawMesh.numFaces; a++)
	{
		m_rawMesh.nxIndices[a*3]	= Indices[a*3];
		m_rawMesh.nxIndices[a*3+1]	= Indices[a*3+1];
		m_rawMesh.nxIndices[a*3+2]	= Indices[a*3+2];
	}

	m_rawMesh.vb->Unlock();
	m_rawMesh.ib->Unlock();
	
	gEngine.kernel->log->prnEx(LT_SUCCESS, "Mesh", "Generated physics vert indices for %s", this->strFilename.c_str());

	gEngine.kernel->addMemUsage("MeshPh", m_rawMesh.numVertices * sizeof(Vec3));
	gEngine.kernel->addMemUsage("MeshPh", m_rawMesh.numFaces * 3 * sizeof(u32));
}

BoundingBox* Mesh::getBoundingBox(Mat world)
{
	Vec3 Min, Max;
	Vec3 pos;
	Vec4 p;

	struct Vertex
	{
		Vec3	pos;
		Vec2	texCoord;
		Vec3	normal;
		Vec3	tangent;
		Vec3	binormal;
	};

	Vertex*		Vertices;

	m_rawMesh.vb->Lock(0, 0, (void**)&Vertices, D3DLOCK_DISCARD);

	pos = Vertices[0].pos;
	D3DXVec3Transform(&p, &pos, &world);
	
	Min = Max = Vec3(p.x, p.y, p.z);

	for(long a = 1; a < m_rawMesh.numVertices; a++)
	{
		pos = Vertices[a].pos;
		D3DXVec3Transform(&p, &pos, &world);

		if(p.x < Min.x)Min.x = p.x;
		if(p.y < Min.y)Min.y = p.y;
		if(p.z < Min.z)Min.z = p.z;

		if(p.x > Max.x)Max.x = p.x;
		if(p.y > Max.y)Max.y = p.y;
		if(p.z > Max.z)Max.z = p.z;

	}

	m_rawMesh.vb->Unlock();

	return new BoundingBox(Min,Max);
}

u32 Mesh::load()
{
	release();
		
	LPD3DXBUFFER meshAdjacency;

	u32 genMem = (strFilename.substr(0, 3) == "%gm");

	if(genMem)
	{
		meshBaseCheck = 1;
		meshBaseResult = 0;
		lodGenerated = 0;
	}

	string cacheFileName;
	string sourceFileName = getFolder() + strFilename;

	if(g_allowMeshCache)
	{
		if(genMem)
		{
			cacheFileName = "models/"+strFilename.substr(4)+".xbin";
			if(gEngine.kernel->fs->exists(cacheFileName))
			{
				if(!m_rawMesh.loadBin(cacheFileName))
				{
					release();
				}
				else
				{
					modDate = gEngine.kernel->fs->getModDate(cacheFileName);
					return prepareMesh();
				}
			}
		}
		else
		{
			cacheFileName = "models/"+getWithoutExt(strFilename)+".xbin";

			if(gEngine.kernel->fs->exists(sourceFileName) && gEngine.kernel->fs->exists(cacheFileName))
			{
				if(gEngine.kernel->fs->getModDate(sourceFileName) <= gEngine.kernel->fs->getModDate(cacheFileName))
				{
					if(!m_rawMesh.loadBin(cacheFileName))
					{
						release();
					}
					else
					{
						modDate = gEngine.kernel->fs->getModDate(cacheFileName);
						return prepareMesh();
					}
				}
			}
			else if(gEngine.kernel->fs->exists(cacheFileName))
			{
				if(!m_rawMesh.loadBin(cacheFileName))
				{
					release();
					gEngine.kernel->log->prnEx(LT_ERROR, "Mesh", "Couldn't load mesh '%s' from cache.", cacheFileName.c_str());
					return 0;
				}
				else
				{
					modDate = gEngine.kernel->fs->getModDate(cacheFileName);
					return prepareMesh();
				}
			}
		}
	}

	if(genMem)
	{
		Tokenizer tok(strFilename);
		string geoName;
		string size;

		tok.nextToken(&geoName);
		tok.nextToken(&geoName);

		if(geoName == "polygon")
		{
			string strLength; f32 length;
			string strSides;  u32 sides;

			tok.nextToken(&strLength);
			tok.nextToken(&strSides);
			
			length = strConv<f32>(strLength);
			sides = strConv<u32>(strSides);

			if(FAILED(D3DXCreatePolygon(gEngine.device->getDev(), length, sides, &m_pMesh, &meshAdjacency)))
			{
				gEngine.kernel->log->prnEx(LT_ERROR, "Mesh", "Couldn't create polygon mesh (length %f, sides %u)", length, sides);
				return 0;
			}

			gEngine.kernel->log->prnEx(LT_SUCCESS, "Mesh", "Created polygon mesh (length %f, sides %u)", length, sides);
		}
		else if(geoName == "box")
		{
			string strW, strH, strD;
			f32 w, h, d;

			tok.nextToken(&strW);
			tok.nextToken(&strH);
			tok.nextToken(&strD);
			
			w = strConv<f32>(strW);
			h = strConv<f32>(strH);
			d = strConv<f32>(strD);

			if(FAILED(D3DXCreateBox(gEngine.device->getDev(), w, h, d, &m_pMesh, &meshAdjacency)))
			{
				gEngine.kernel->log->prnEx(LT_ERROR, "Mesh", "Couldn't create box mesh (w %f, h %f, d %f)", w, h, d);
				return 0;
			}

			gEngine.kernel->log->prnEx(LT_SUCCESS, "Mesh", "Created box mesh (w %f, h %f, d %f)", w, h, d);
		}
		else if(geoName == "cylinder")
		{
			string strR1, strR2, strLength, strSlices, strStacks;
			f32 r1, r2, length, slices, stacks;

			tok.nextToken(&strR1);
			tok.nextToken(&strR2);
			tok.nextToken(&strLength);
			tok.nextToken(&strSlices);
			tok.nextToken(&strStacks);
			
			
			r1 = strConv<f32>(strR1);
			r2 = strConv<f32>(strR2);
			length = strConv<f32>(strLength);
			slices = strConv<f32>(strSlices);
			stacks = strConv<f32>(strStacks);


			if(FAILED(D3DXCreateCylinder(gEngine.device->getDev(), r1, r2, length, slices, stacks, &m_pMesh, &meshAdjacency)))
			{
				gEngine.kernel->log->prnEx(LT_ERROR, "Mesh", "Couldn't create cylinder mesh (r1 %f, r2 %f, l %f, sl %f, st %f)", r1, r2, length, slices, stacks);
				return 0;
			}

			gEngine.kernel->log->prnEx(LT_INFO, "Mesh", "Created cylinder mesh (r1 %f, r2 %f, l %f, sl %f, st %f)", r1, r2, length, slices, stacks);
		}
		else if(geoName == "plane")
		{
			string strSize, strSides;
			f32 size, sides;

			tok.nextToken(&strSize);
			tok.nextToken(&strSides);
			
			size = strConv<f32>(strSize);
			sides = strConv<f32>(strSides);

			const D3DVERTEXELEMENT9 vertexDecl[] =
			{
				{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				{ 0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
				D3DDECL_END()
			};

			u32 vertices = (u32)(sides + 1);


			if(FAILED(D3DXCreateMesh(sides*sides*2, vertices*vertices, 0, vertexDecl, gEngine.device->getDev(), &m_pMesh)))
			{
				gEngine.kernel->log->prnEx(LT_ERROR, "Mesh", "Couldn't create plane mesh (size %f, sides %f)", size, sides);
				return 0;
			}

			struct Vertex
			{
				Vec3	pos;
				Vec2	texCoord;
				Vec3	normal;
			};

			Vertex*		Vertices;
			WORD*		Indices;

			m_pMesh->GetIndexBuffer(&m_rawMesh.ib);
			m_pMesh->GetVertexBuffer(&m_rawMesh.vb);

			m_rawMesh.vb->Lock(0, 0, (void**)&Vertices, 0);
			m_rawMesh.ib->Lock(0, 0, (void**)&Indices, 0);

			for(f32 y = 0; y < vertices; y++)
			{
				for(f32 x = 0; x < vertices; x++)
				{
					u32 index = (u32)(y*vertices+x);
					Vertices[index].pos = Vec3(x/sides * size, 0, y/sides * size);
					Vertices[index].normal = Vec3(0,1,0);
					Vertices[index].texCoord = Vec2(x/sides, y/sides);
				}
			}
			
			u32 count = 0, i1, i2, i3;
			for(f32 y = 0; y < sides; y++)
			{
				for(f32 x = 0; x < sides; x++)
				{
					u32 index = (u32)(y*vertices+x);
					i1 = index + 1; i2 = index; i3 = index + vertices;
					Indices[count++] = i1; Indices[count++] = i2; Indices[count++] = i3;
					i1 = index + 1; i2 = index + vertices; i3 = index + vertices + 1;
					Indices[count++] = i1; Indices[count++] = i2; Indices[count++] = i3;
				}
			}

			m_rawMesh.vb->Unlock();
			m_rawMesh.ib->Unlock();

			if( count != (u32)(sides*sides*2*3))
			{
				gEngine.kernel->log->prnEx(LT_ERROR, "Mesh", "Couldn't create plane mesh (size %f, sides %f). Generation error!", size, sides);
				return 0;
			}

			gEngine.kernel->log->prnEx(LT_SUCCESS, "Mesh", "Created plane mesh (size %f, sides %f)", size, sides);
		}
		else 
		{
			gEngine.kernel->log->prnEx(LT_ERROR, "Mesh", "Unrecognized %gm command ('%s') for mesh. Couldn't create geo mesh.", strFilename.c_str());
			return 0;
		}
	}
	else
	{
		if(!loadXMesh(getFolder()+strFilename, &meshAdjacency))
			return 0;

		modDate = gEngine.kernel->fs->getModDate(getFolder()+strFilename);
	}	

	if(m_pMesh)
	{
		const D3DVERTEXELEMENT9 vertexDecl[] =
		{
			{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
			{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
			{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
			D3DDECL_END()
		};

		LPD3DXMESH pTempMesh = NULL;
		if( FAILED( m_pMesh->CloneMesh( m_pMesh->GetOptions(), vertexDecl, gEngine.device->getDev(), &pTempMesh ) ) )
		{
			if(pTempMesh)pTempMesh->Release();
			return 0;
		}
		bool bHadNormal = false, bHadTangent = false, bHadBinormal = false;
		D3DVERTEXELEMENT9 vertexOldDecl[ MAX_FVF_DECL_SIZE ];
		if( m_pMesh && SUCCEEDED( m_pMesh->GetDeclaration( vertexOldDecl ) ) )
		{
			for( UINT iChannelIndex = 0; iChannelIndex < D3DXGetDeclLength( vertexOldDecl ); iChannelIndex++ )
			{
				if( vertexOldDecl[iChannelIndex].Usage == D3DDECLUSAGE_NORMAL )bHadNormal = true;
				if( vertexOldDecl[iChannelIndex].Usage == D3DDECLUSAGE_TANGENT )bHadTangent = true;
				if( vertexOldDecl[iChannelIndex].Usage == D3DDECLUSAGE_BINORMAL )bHadBinormal = true;
			}
		}
		if( pTempMesh == NULL && ( bHadNormal == false || bHadTangent == false || bHadBinormal == false ) )
		{
			return 0;
		}
		if(m_pMesh)m_pMesh->Release();
		m_pMesh = pTempMesh;

		if(!bHadNormal)D3DXComputeNormals( m_pMesh, NULL );
		DWORD* rgdwAdjacency = NULL;
		rgdwAdjacency = new DWORD[ m_pMesh->GetNumFaces() * 3 ];
		if( rgdwAdjacency == NULL )return 0;
		m_pMesh->GenerateAdjacency( 1e-6f, rgdwAdjacency );

		m_pMesh->OptimizeInplace( D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL );
		if( !bHadTangent || !bHadBinormal )
		{
			ID3DXMesh* pNewMesh;
			if( FAILED( D3DXComputeTangentFrameEx( m_pMesh, D3DDECLUSAGE_TEXCOORD, 0, D3DDECLUSAGE_TANGENT, 0,
												   D3DDECLUSAGE_BINORMAL, 0, D3DDECLUSAGE_NORMAL, 0, 0, 
												   rgdwAdjacency, -1.01f, -0.01f, -1.01f, &pNewMesh, NULL ) ) )
			{
				return 0;
			}

			if(m_pMesh)m_pMesh->Release();
			m_pMesh = pNewMesh;
		}
		
		if(rgdwAdjacency)delete [] rgdwAdjacency;

		////////////////////////////////////////
		m_pMesh->GetIndexBuffer(&m_rawMesh.ib);
		m_pMesh->GetVertexBuffer(&m_rawMesh.vb);
		
		gEngine.kernel->addMemUsage("Mesh", m_pMesh->GetNumVertices() * m_pMesh->GetNumBytesPerVertex());
		gEngine.kernel->addMemUsage("Mesh", m_pMesh->GetNumFaces() * 3 * sizeof(WORD));

		m_rawMesh.primType		= D3DPT_TRIANGLELIST;
		m_rawMesh.vertexStride	= m_pMesh->GetNumBytesPerVertex();
		m_rawMesh.numFaces		= m_pMesh->GetNumFaces();
		m_rawMesh.numVertices	= m_pMesh->GetNumVertices();
		
		reGenerateTangents();
		generateVerticesIndicesLists();
		
		if(!prepareMesh())
			return 0;

		if(g_allowMeshCache)
		{
			m_rawMesh.saveBin(cacheFileName.c_str(), modDate, getCache());
		}

		return 1;
	}

	return 1;
}

u32 Mesh::loadXMesh(string fileName, LPD3DXBUFFER *meshAdj)
{
	LPD3DXBUFFER	meshAdjacency;
	LPD3DXBUFFER	BufMaterials = 0; 
	DWORD			num_materials;

	buffer = new Buffer();
	if(!gEngine.kernel->fs->load(fileName, *buffer))
	{
		gEngine.kernel->mem->freeBuff(*buffer);
		buffer = 0;
		return 0;
	}

	if(!buffer->data || !buffer->size)return 0;

	if(FAILED(D3DXLoadMeshFromXInMemory(buffer->data, buffer->size, 0 , gEngine.device->getDev(), &meshAdjacency, &BufMaterials, 0, &num_materials, &m_pMesh)))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "Mesh", "Couldn't create mesh from file in memory ('%s')", strFilename.c_str());
		return 0;
	}

	gEngine.kernel->mem->freeBuff(*buffer);
	delete(buffer);
	buffer = 0;
	
	if(BufMaterials)BufMaterials->Release();

	if(meshAdj)
		*meshAdj = meshAdjacency;

	return 1;
}

u32 Mesh::prepareMesh()
{
	struct Vertex
	{
		Vec3	pos;
		Vec2	texCoord;
		Vec3	normal;
		Vec3	tangent;
		Vec3	binormal;
	};

	m_pVertexDeclarationSize						= sizeof(Vertex);
	m_pInstanceVertexDeclarationSizeStream0			= m_pVertexDeclarationSize;
	m_pInstanceVertexDeclarationSizeStream1			= sizeof(InstanceData);
	m_pInstanceVertexDeclarationSizeStream1Packed	= sizeof(InstanceDataPacked);

	if(m_pMesh)
	{
		m_pMesh->GetIndexBuffer(&m_rawMesh.ib);
		m_pMesh->GetVertexBuffer(&m_rawMesh.vb);
		m_rawMesh.primType		= D3DPT_TRIANGLELIST;
		m_rawMesh.vertexStride	= m_pMesh->GetNumBytesPerVertex();
		m_rawMesh.numFaces		= m_pMesh->GetNumFaces();
		m_rawMesh.numVertices	= m_pMesh->GetNumVertices();

		char* buff = 0;
		m_pMesh->LockVertexBuffer(0, (void**)&buff);
		D3DXVECTOR3 min, max;
		D3DXComputeBoundingBox((D3DXVECTOR3*)buff, m_rawMesh.numVertices, m_rawMesh.vertexStride, &min, &max);
		m_rawMesh.boundingBox.reset(min, max);
		m_pMesh->UnlockVertexBuffer();
	}
	else if(m_rawMesh.vertices && m_rawMesh.indices && m_rawMesh.numFaces && m_rawMesh.numVertices)
	{
		const D3DVERTEXELEMENT9 vertexDecl[] =
		{
			{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
			{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
			{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
			D3DDECL_END()
		};

		if(FAILED(D3DXCreateMesh(m_rawMesh.numFaces, m_rawMesh.numVertices, 0, vertexDecl, gEngine.device->getDev(), &m_pMesh)))
		{
			gEngine.kernel->log->prnEx(LT_ERROR, "Mesh", "Couldn't create mesh for cached model.");
			return 0;
		}

		Vertex*		Vertices;
		WORD*		Indices;

		m_pMesh->GetIndexBuffer(&m_rawMesh.ib);
		m_pMesh->GetVertexBuffer(&m_rawMesh.vb);

		m_rawMesh.vb->Lock(0, 0, (void**)&Vertices,	0);
		m_rawMesh.ib->Lock(0, 0, (void**)&Indices,	0);

		memcpy(Vertices, m_rawMesh.vertices, sizeof(Vertex) * m_rawMesh.numVertices);
		memcpy(Indices, m_rawMesh.indices, sizeof(WORD) * m_rawMesh.numFaces * 3);

		m_rawMesh.vb->Unlock();
		m_rawMesh.ib->Unlock();

		delete [] m_rawMesh.vertices;
		m_rawMesh.vertices = 0;
		delete [] m_rawMesh.indices;
		m_rawMesh.indices = 0;
	}
	else
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "Mesh", "Prepare mesh failed. No data.");
		return 0;
	}

	return 1;
}

void Mesh::render()
{
	get();

	gEngine.device->getDev()->SetVertexDeclaration(gEngine.renderer->m_vd[VD_DEFAULT]);
	gEngine.device->getDev()->SetStreamSource(0, m_rawMesh.vb, 0, m_pVertexDeclarationSize);
	gEngine.device->getDev()->SetIndices(m_rawMesh.ib);
	gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_rawMesh.numVertices, 0, (UINT)m_rawMesh.numFaces);

	gEngine.renderer->addRenderedCounts(m_rawMesh.numVertices, m_rawMesh.numFaces, 1);
}
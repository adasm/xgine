#pragma once
#include "XGine.h"

extern XGINE_API u32 g_numMeshes;

struct XGINE_API RawMesh
{
	RawMesh() : ib(0), vb(0), vertexStride(0), fvf(0), numVertices(0), numFaces(0), vertices(0), indices(0), nxVertices(0), nxIndices(0) { };

	struct VertexNTB
	{
		Vec3	pos;
		Vec2	texCoord;
		Vec3	normal;
		Vec3	tangent;
		Vec3	binormal;
	};

	void	release();

	u32		loadBin(string fileName);
	u32		saveBin(string fileName, u64 modDate, string cacheName);
	
	u32		saveBin(LPD3DXMESH mesh, string fileName, u64 modDate, string cacheName);

	IDirect3DIndexBuffer9*			ib;
	IDirect3DVertexBuffer9*			vb;
	D3DPRIMITIVETYPE				primType;
	u32								vertexStride;
	DWORD							fvf;

	u32								numVertices;
	u32								numFaces;	
	BoundingBox						boundingBox;
	VertexNTB*						vertices;
	WORD*							indices;
	Vec3*							nxVertices;
	u32*							nxIndices;
};

struct XGINE_API Mesh : public IRes
{
protected:
	friend ResMgr;

	~Mesh();
	Mesh();
	void	release();
	u32		load();

	string  getFolder() { return "models/"; }
	u32	    canBeLoadBackground() { return 0; }
	u32		mustLoadBuffer() { return 0; }

	void generateDegenerateQuads();
	void reGenerateTangents();
	void generateVerticesIndicesLists();
	u32  loadXMesh(string fileName, LPD3DXBUFFER *meshAdj = 0);
	u32  prepareMesh();

	u32			generateLods(u32 canOnlyCheck = 0);
	string		getLODName(u32 lod);

	u32			meshBaseCheck, meshBaseResult;
	u32			lodGenerated;

	LPD3DXMESH 						m_pMesh;
	
	RawMesh							m_rawMesh;

	u32								m_pVertexDeclarationSize;
	u32								m_pInstanceVertexDeclarationSizeStream0;
	u32								m_pInstanceVertexDeclarationSizeStream1;
	u32								m_pInstanceVertexDeclarationSizeStream1Packed;

	IDirect3DVertexBuffer9*			m_instanceVB;
	void*							m_instanceData;
	u32								m_instanceVBSize;
	u32								m_instanceMustResize;
	u32								m_allocFailed;
	u32								reallocInstancVB();

	u32								m_ID;
	u64								modDate;

	void*	_start;
	u32		_frame;
	u32		_count;
public:
	
	void render();
	template< typename T > inline void	renderInstanced(T *ent);

						u32			getNumLODs();
						Mesh*		loadLOD(u32 lod);
						u32			isBaseMesh();

	inline LPD3DXMESH				get()						{ if(buffer)load(); return m_pMesh; }
	inline BoundingBox&				getBoundingBox()			{ return m_rawMesh.boundingBox; }
		   BoundingBox*				getBoundingBox(Mat world);
	inline IDirect3DIndexBuffer9*	getIB()						{ return m_rawMesh.ib; }
	inline IDirect3DVertexBuffer9*	getVB()						{ return m_rawMesh.vb; }
	inline u32						getNumVertices()			{ return m_rawMesh.numVertices; }
	inline u32						getNumFaces()				{ return m_rawMesh.numFaces; }

	inline Vec3*					getVertices()				{ return m_rawMesh.nxVertices; }
	inline u32*						getIndices()				{ return m_rawMesh.nxIndices; }

	inline u32						getID()						{ return m_ID; }
};

template< typename T > inline void Mesh::renderInstanced(T *ent)
{
	static u32 mustResize;

	get();

	if(!m_instanceVB)
	{
		if(!reallocInstancVB())return;
		m_instanceMustResize = 0;
	}

	void * voidEnt = (void*)ent;
	if( _start != voidEnt || _frame != gEngine.kernel->tmr->getFrameID())
	{
		f32 start_time = gEngine.kernel->tmr->getCurrent();

		if(m_instanceMustResize)
		{
			m_instanceVBSize *= 2;
			if(!reallocInstancVB())return;
			m_instanceMustResize = 0;
		}

		if(r_packInstanceDataToFloat16)
		{
			InstanceDataPacked *ptr_data = (InstanceDataPacked*)m_instanceData;
			_count = 0;
			for(;ent; ent = ent->next)
			{
				ptr_data[_count++] = ent->instanceDataPacked;
				if(_count >= m_instanceVBSize){ m_instanceMustResize = 1; break; }
			}
			
			void *ptr_instances = 0;
			if(FAILED(m_instanceVB->Lock(0, 0, (void**)&ptr_instances, D3DLOCK_DISCARD)))return;
			memcpy(ptr_instances, m_instanceData, sizeof(InstanceDataPacked) * _count);
			m_instanceVB->Unlock();
		}
		else
		{
			InstanceData *ptr_data = (InstanceData*)m_instanceData;
			_count = 0;
			for(;ent; ent = ent->next)
			{
				ptr_data[_count++] = ent->instanceData;
				if(_count >= m_instanceVBSize){ m_instanceMustResize = 1; break; }
			}


			void *ptr_instances = 0;
			if(FAILED(m_instanceVB->Lock(0, 0, (void**)&ptr_instances, D3DLOCK_DISCARD)))return;
			memcpy(ptr_instances, m_instanceData, sizeof(InstanceData) * _count);
			m_instanceVB->Unlock();
		}

		_start = voidEnt;
		_frame = gEngine.kernel->tmr->getFrameID();

		f32 end_time = gEngine.kernel->tmr->getCurrent();

		gEngine.renderer->addTxt("Generated instance data (V4-P,S) buffer for %u meshes in %.2fms [Mesh ptr %u, id %u, ent %u, frame %u]", _count, (end_time-start_time), this, m_ID, (u32)voidEnt, _frame);
	}


	gEngine.device->getDev()->SetStreamSourceFreq(0, (D3DSTREAMSOURCE_INDEXEDDATA | _count));
	gEngine.device->getDev()->SetStreamSource(0, m_rawMesh.vb, 0, m_pInstanceVertexDeclarationSizeStream0);
	gEngine.device->getDev()->SetStreamSourceFreq(1, (D3DSTREAMSOURCE_INSTANCEDATA | 1ul));
	gEngine.device->getDev()->SetStreamSource(1, m_instanceVB, 0, (r_packInstanceDataToFloat16)?(m_pInstanceVertexDeclarationSizeStream1Packed):(m_pInstanceVertexDeclarationSizeStream1));

	gEngine.device->getDev()->SetVertexDeclaration(gEngine.renderer->m_vd[(r_packInstanceDataToFloat16)?(VD_DEFAULT_INSTANCED_PACKED):(VD_DEFAULT_INSTANCED)]);
	gEngine.device->getDev()->SetIndices(m_rawMesh.ib);
	gEngine.device->getDev()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_rawMesh.numVertices, 0, (UINT)m_rawMesh.numFaces);

	gEngine.renderer->addRenderedCounts(m_rawMesh.numVertices * _count, m_rawMesh.numFaces * _count, 1);

	gEngine.device->getDev()->SetStreamSourceFreq(0,1);
	gEngine.device->getDev()->SetStreamSourceFreq(1,1);

	gEngine.renderer->addTxt("Rendered instanced %u meshes", _count);
}
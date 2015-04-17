#include "RenderTextureCube.h"

RenderTextureCube::RenderTextureCube()
{
	m_pCubeTexture		=NULL;
	m_pDSSurface		=NULL;
	m_pOldDSSurface		=NULL;
	m_pOldRenderTarget	=NULL;
}

RenderTextureCube::~RenderTextureCube()
{
	close();
}

u32 RenderTextureCube::init(const CreationParams &cp)
{
	if(FAILED(gEngine.device->getDev()->CreateCubeTexture(cp.size, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &m_pCubeTexture, 0)))
	{
		gEngine.kernel->log->prn(LT_FATAL, "RenderTextureCube", "Creating DepthMap failed. Could not create cube texture.");
		return 0;
	}
	if(FAILED(gEngine.device->getDev()->CreateDepthStencilSurface(cp.size,cp.size,cp.depthFormat,D3DMULTISAMPLE_NONE,0,TRUE,&m_pDSSurface,NULL)))
	{
		gEngine.kernel->log->prn(LT_FATAL, "RenderTextureCube", "Creating DepthMap failed. CreateDepthStencilSurface failed.");
		return 0;
	}
	m_Viewport.X = 0;
	m_Viewport.Y = 0;
	m_Viewport.Width  = cp.size;
	m_Viewport.Height = cp.size;
	m_Viewport.MinZ = 0.0f;
	m_Viewport.MaxZ = 1.0f;
	float ZBias		= -0.001f;
	float fTexOffs	= 0.5f + (0.5f / (float)cp.size);
	m_texAdjMat		= Mat(0.5f,		0.0f,	0.0f,	0.0f,
						  0.0f,    -0.5f,	0.0f,	0.0f,
						  0.0f,		0.0f,	1.0f,	0.0f,
						  fTexOffs, fTexOffs,  ZBias, 1.0f );
	D3DXMatrixPerspectiveFovRH(&m_cubeFaceProjection, D3DX_PI * 0.5f, 1.0f, 0.02f, 50.0f);
	m_Params=cp;

	gEngine.device->getDev()->GetRenderTarget(0,&m_pOldRenderTarget);
	gEngine.device->getDev()->GetDepthStencilSurface(&m_pOldDSSurface);

	return 1;
}

void RenderTextureCube::close()
{
	DXRELEASE(m_pDSSurface);
	DXRELEASE(m_pCubeTexture);
	DXRELEASE(m_pOldDSSurface);
	DXRELEASE(m_pOldRenderTarget);
}

void RenderTextureCube::enableRendering()
{
	gEngine.device->getDev()->SetDepthStencilSurface(m_pDSSurface);
	//gEngine.device->getDev()->SetViewport(&m_Viewport);
}

void RenderTextureCube::setupCubeFace(u32 i)
{
	static LPDIRECT3DSURFACE9 tempSurf;
	m_pCubeTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)i, 0, &tempSurf);
	gEngine.device->getDev()->SetRenderTarget(0,tempSurf);
	tempSurf->Release();
}

void RenderTextureCube::disableRendering()
{
	gEngine.device->getDev()->SetRenderTarget(0,m_pOldRenderTarget);
	gEngine.device->getDev()->SetDepthStencilSurface(m_pOldDSSurface);
	//gEngine.device->getDev()->SetViewport(&m_OldViewport);
}

void RenderTextureCube::calculateCubeMapViewMat(u32 face, const D3DXVECTOR3* centerPos, D3DXMATRIX* outMatrix)
{
    static Vec3 lookDir;
    static Vec3 upDir;
    switch(face)
    {
        case D3DCUBEMAP_FACE_POSITIVE_X:
            lookDir = Vec3(-1.0f, 0.0f, 0.0f ); //LH:1,0,0
            upDir   = Vec3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_X:
            lookDir = Vec3( 1.0f, 0.0f, 0.0f ); //LH:-1,0,0
            upDir   = Vec3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Y:
            lookDir = Vec3( 0.0f, 1.0f, 0.0f );
            upDir   = Vec3( 0.0f, 0.0f,-1.0f ); //LH:0,0,-1
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Y:
            lookDir = Vec3( 0.0f,-1.0f, 0.0f );
            upDir   = Vec3( 0.0f, 0.0f, 1.0f ); //LH:0,0,1
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Z:
            lookDir = Vec3( 0.0f, 0.0f, 1.0f );
            upDir   = Vec3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Z:
            lookDir = Vec3( 0.0f, 0.0f,-1.0f );
            upDir   = Vec3( 0.0f, 1.0f, 0.0f );
            break;
    }
	lookDir += *centerPos;
    D3DXMatrixLookAtRH(outMatrix, centerPos, &lookDir, &upDir);
}
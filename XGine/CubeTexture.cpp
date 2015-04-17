#include "CubeTexture.h"

CubeTexture::~CubeTexture()
{
	release();
}

CubeTexture::CubeTexture() 
: lpTexture(0), width(0), height(0)
{
	if(buffer)gEngine.kernel->mem->freeBuff(*buffer); delete(buffer); buffer = 0;
}

void CubeTexture::release()
{
	gEngine.kernel->subMemUsage("Texture", width * height * sizeof(u32));
	DXRELEASE(lpTexture);
	width = height = 0;
}

u32 CubeTexture::load()
{
	release();
	
	if(!buffer->data || !buffer->size)return 0;

	if(SUCCEEDED( D3DXCreateCubeTextureFromFileInMemory(gEngine.device->getDev(), buffer->data, buffer->size ,&lpTexture) ))
	{
		D3DSURFACE_DESC surfaceDesc;
		lpTexture->GetLevelDesc(0, &surfaceDesc);
		width = surfaceDesc.Width;
		height = surfaceDesc.Height;
		gEngine.kernel->addMemUsage("Texture", width * height * sizeof(u32));
		gEngine.kernel->mem->freeBuff(*buffer);
		buffer = 0;
		gEngine.kernel->log->prnEx(LT_SUCCESS, "CubeTexture", "Created cube texture '%s'.", strFilename.c_str());
		return 1;
	}
	else
	{
		width = height = 0;
		lpTexture = 0;
		gEngine.kernel->mem->freeBuff(*buffer);
		buffer = 0;
		return 0;
	}
}
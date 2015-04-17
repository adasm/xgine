#include "VolumeTexture.h"

VolumeTexture::~VolumeTexture()
{
	if(buffer)gEngine.kernel->mem->freeBuff(*buffer); delete(buffer); buffer = 0;
	release();
}

VolumeTexture::VolumeTexture() 
: lpTexture(0), width(0), height(0), depth(0)
{ 
}

void VolumeTexture::release()
{
	gEngine.kernel->subMemUsage("Texture", width * height * depth * sizeof(u32));
	DXRELEASE(lpTexture);
	width = height = depth = 0;
}

u32 VolumeTexture::load()
{
	release();
	
	if(!buffer->data || !buffer->size)return 0;

	if(SUCCEEDED( D3DXCreateVolumeTextureFromFileInMemory(gEngine.device->getDev(), buffer->data, buffer->size ,&lpTexture) ))
	{
		D3DVOLUME_DESC surfaceDesc;
		lpTexture->GetLevelDesc(0, &surfaceDesc);
		width = surfaceDesc.Width;
		height = surfaceDesc.Height;
		depth = surfaceDesc.Depth;
		gEngine.kernel->addMemUsage("Texture", width * height * depth * sizeof(u32));
		gEngine.kernel->mem->freeBuff(*buffer);
		buffer = 0;
		gEngine.kernel->log->prnEx(LT_SUCCESS, "VolumeTexture", "Created volume texture '%s'.", strFilename.c_str());
		return 1;
	}
	else
	{
		width = height = depth = 0;
		lpTexture = 0;
		gEngine.kernel->mem->freeBuff(*buffer);
		buffer = 0;
		return 0;
	}
}
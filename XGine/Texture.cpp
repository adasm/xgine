#include "Texture.h"

Texture::~Texture()
{
	if(buffer)gEngine.kernel->mem->freeBuff(*buffer); delete(buffer); buffer = 0;
	release();
}

Texture::Texture() 
: lpTexture(0), width(0), height(0)
{ 
}

void Texture::release()
{
	gEngine.kernel->subMemUsage("Texture", width * height * sizeof(u32));
	DXRELEASE(lpTexture);
	width = height = 0;
}

u32 Texture::load()
{
	release();

	if(!buffer->data || !buffer->size)return 0;

	if(SUCCEEDED( D3DXCreateTextureFromFileInMemory(gEngine.device->getDev(), buffer->data, buffer->size, &lpTexture) ))
	//if(SUCCEEDED( D3DXCreateTextureFromFileInMemoryEx(gEngine.device->getDev(), buffer->data, buffer->size, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &lpTexture) ))
	{
		D3DSURFACE_DESC surfaceDesc;
		lpTexture->GetLevelDesc(0, &surfaceDesc);
		width = surfaceDesc.Width;
		height = surfaceDesc.Height;
		//lpTexture->GenerateMipSubLevels();
		gEngine.kernel->addMemUsage("Texture", width * height * sizeof(u32));


		gEngine.kernel->mem->freeBuff(*buffer);
		delete(buffer);
		buffer = 0;

		gEngine.kernel->log->prnEx(LT_SUCCESS, "Texture", "Created texture '%s'.", strFilename.c_str());
		return 1;
	}
	else
	{
		width = height = 0;
		lpTexture = 0;
		gEngine.kernel->mem->freeBuff(*buffer);
		delete(buffer);
		buffer = 0;
		return 0;
	}
}
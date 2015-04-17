#include "Effect.h"

Effect::Effect()
	: lpEffect(0)
{
}
Effect::~Effect()
{
	if(lpEffect)
		lpEffect->Release();
}

void Effect::release()
{
	if(lpEffect)
		lpEffect->Release();
}

u32 Effect::load()
{
	LPD3DXBUFFER pBufferErrors = NULL;
	release();

	Buffer buffer;
	string g_path_effects = "shaders/";

	if(!gK.fs.load(g_path_effects+strFilename, buffer))return 0;

	if(FAILED(D3DXCreateEffect(gD.getDev(),buffer.data,buffer.size, NULL, NULL, 0, NULL, &lpEffect, &pBufferErrors )))
	{
		if(pBufferErrors)
		{
			LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
			gK.log.prn((char*)pCompilErrors);
		}
		pBufferErrors->Release();
		return 0;
	}
	return 1;
}
D3DXHANDLE Effect::getParam(char *name)
{
	return lpEffect ? lpEffect->GetParameterByName(NULL,name) : NULL;
}

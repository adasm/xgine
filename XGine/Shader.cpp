#include "Shader.h"

Shader::Shader()
	: lpEffect(0), procBegin(0), passes(0), currTechProcStr(0)
{
}

Shader::~Shader()
{
	DXRELEASE(lpEffect);
}

D3DXHANDLE Shader::getParam(char *name)
{
	return lpEffect ? lpEffect->GetParameterByName(NULL,name) : NULL;
}

D3DXHANDLE Shader::getTech(char *name)
{
	return lpEffect->GetTechniqueByName(name);
}

Shader* Shader::setTech(D3DXHANDLE handle)
{
	if(	currTechProcStr != handle )
	{
		u32 mustBegin = procBegin;

		if(procBegin)end();

		gEngine.renderer->_addTxt(" -tech: %s", handle);
	

		lpEffect->SetTechnique(handle);
		currTechProcStr = handle;
		if(mustBegin){ gEngine.renderer->_rendererShaderChanges++; begin(); }

	}
	return this;
}

Shader* Shader::setTech(char *name)
{
	D3DXHANDLE handle = getTech(name);
	if(	currTechProcStr != handle )
	{
		u32 mustBegin = procBegin;

		if(procBegin)end();
		gEngine.renderer->_addTxt(" -tech: %s", handle);
	

		lpEffect->SetTechnique(handle);
		currTechProcStr = handle;
		if(mustBegin){ gEngine.renderer->_rendererShaderChanges++; begin(); }

	}
	return this;
}

void Shader::begin()
{
	if(procBegin)return;
	static DWORD flags;
	lpEffect->Begin(&passes, flags);
	lpEffect->BeginPass(0);
	procBegin = 1;
}

void Shader::end()
{
	if(!procBegin)return;
	lpEffect->EndPass();
	lpEffect->End();
	procBegin = 0;
}

void Shader::setVal(D3DXHANDLE h, void* v, u32 c)
{
	lpEffect->SetValue(h, v, c);
}

void Shader::setBool(D3DXHANDLE h, bool v)
{
	lpEffect->SetBool(h, v);
}

void Shader::setVec2(D3DXHANDLE h, const Vec2* v)
{
	lpEffect->SetFloatArray(h, (f32*)v, 2);
}

void Shader::setVec3(D3DXHANDLE h, const Vec3* v)
{
	lpEffect->SetFloatArray(h, (f32*)v, 3);
}

void Shader::setVec4(D3DXHANDLE h, const Vec4* v)
{
	lpEffect->SetVector(h, v);
}

void Shader::setF32(D3DXHANDLE h, const f32 f)
{
	lpEffect->SetFloat(h, f);
}

void Shader::setTex(D3DXHANDLE h, Texture* tex)
{
	lpEffect->SetTexture(h, (tex)?(tex->get()):(0));
}

void Shader::setTex(D3DXHANDLE h, CubeTexture* tex)
{
	lpEffect->SetTexture(h, (tex)?(tex->get()):(0));
}

void Shader::setTex(D3DXHANDLE h, VolumeTexture* tex)
{
	lpEffect->SetTexture(h, (tex)?(tex->get()):(0));
}

void Shader::setTex(D3DXHANDLE h, const LPDIRECT3DTEXTURE9 tex)
{
	lpEffect->SetTexture(h, tex);
}

void Shader::setMat(D3DXHANDLE h, const Mat* mat)
{
	lpEffect->SetMatrix(h, mat);
}

void Shader::commitChanges()
{
	lpEffect->CommitChanges();
}
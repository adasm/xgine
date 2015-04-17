#pragma once

struct IRenderTexture
{
	virtual ~IRenderTexture() { }
	virtual void					enableRendering() = 0;
	virtual void					disableRendering() = 0;
	virtual LPDIRECT3DTEXTURE9		getColorTexture() = 0;
};
#pragma once
#include "XGine.h"

struct XGINE_API Texture : public IRes
{
protected:
	friend ResMgr;

	~Texture();
	Texture();

	void	release();
	u32		load();
	string  getFolder() { return "textures/"; }
	u32	    canBeLoadBackground() { return 1; }
	u32		mustLoadBuffer() { return 1; }

	IDirect3DTexture9*	lpTexture;

public:
	inline IDirect3DTexture9*  get() { if(buffer)load(); return lpTexture; }
	u32					width, 
						height;
};
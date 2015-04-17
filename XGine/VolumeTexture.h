#pragma once
#include "XGine.h"

struct XGINE_API VolumeTexture : public IRes
{
protected:
	friend ResMgr;

	~VolumeTexture();
	VolumeTexture();

	void	release();
	u32		load();
	string  getFolder() { return "textures/"; }
	u32	    canBeLoadBackground() { return 1; }
	u32		mustLoadBuffer() { return 1; }

	IDirect3DVolumeTexture9*	lpTexture;

public:
	inline IDirect3DVolumeTexture9*  get() { if(buffer)load(); return lpTexture; }
	u32						width, 
							height,
							depth;
};
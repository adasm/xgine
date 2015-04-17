#pragma once
#include "XGine.h"

struct XGINE_API Effect : public IRes
{
protected:
	friend ResMgr;
	~Effect();
	void	release();
	u32		load();
	
public:
	Effect();
	
	LPD3DXEFFECT		lpEffect;
	D3DXHANDLE	getParam(char *name);
};
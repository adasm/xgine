#pragma once
#include "XGine.h"

struct XGINE_API Font : public IRes
{
protected:
	friend ResMgr;

	~Font();
	Font();

	void	release();
	u32		load();
	string  getFolder() { return ""; }
	u32	    canBeLoadBackground() { return 0; }
	u32		mustLoadBuffer() { return 0; }

public:
	LPD3DXFONT lpFont;

};
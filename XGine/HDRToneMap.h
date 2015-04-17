/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API HDRToneMap
{
	HDRToneMap();
	~HDRToneMap();

	u32 init();
	void close();

	void process(RenderTexture *lightTranportTexture);

	void setSunPos(Vec3 pos) { sunPos = pos; }

protected:
	u32									initialized, failure;
	GuassianBlur						*blur;
	Shader								*shader;
	RenderTexture						*downSampled;
	RenderTexture						*blured, *bluredTemp;
	RenderTexture						*luminance;
	RenderTexture						*avgLuminance;
	vector<RenderTexture*>				toneMapTextures;
	RenderTexture						*prevLuminance, *currentLuminance;
	vector<LPDIRECT3DVERTEXBUFFER9>		toneMapQuads;
	LPDIRECT3DVERTEXBUFFER9				m_vbHalf;
	Vec3							    sunPos;
};
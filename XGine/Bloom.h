/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API Bloom : IPostprocess
{
	Bloom();
	~Bloom();

	void process();

protected:
	LPDIRECT3DTEXTURE9		m_rt1;
	LPDIRECT3DTEXTURE9		m_rt2;
	LPDIRECT3DVERTEXBUFFER9 m_vbHalf;
	Shader					*m_fx;

	void	FillBlurParams(f32 dx, f32 dy);
	f32		Gaussian(f32 n);
};
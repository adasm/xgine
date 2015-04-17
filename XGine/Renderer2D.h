/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API Renderer2D
{
	Renderer2D();

	u32  init();
	void close();

	void begin();
	void end();

	void drawQuad(f32 _X0, f32 _Y0, f32 _X1, f32 _Y1, D3DCOLOR _Color00, D3DCOLOR _Color10, D3DCOLOR _Color01, D3DCOLOR _Color11);
	void drawQuad(Texture *_tex, f32 _X0, f32 _Y0, f32 _X1, f32 _Y1, D3DCOLOR _Color00, D3DCOLOR _Color10, D3DCOLOR _Color01, D3DCOLOR _Color11);
	void drawQuad(IDirect3DTexture9* _tex, f32 _X0, f32 _Y0, f32 _X1, f32 _Y1, D3DCOLOR _Color00, D3DCOLOR _Color10, D3DCOLOR _Color01, D3DCOLOR _Color11);
	void drawQuadTexMatch(f32 _X0, f32 _Y0, f32 _X1, f32 _Y1);
	void drawQuadTextured(u32 width, u32 height);
	
	void drawScreenQuad(u32 color, u32 textured);
	void drawScreenQuad(IDirect3DTexture9* _tex);
	void drawScreenQuadRaw();
	void drawScreenQuadNotRHW();

	void drawRect(f32 _X0, f32 _Y0, f32 _X1, f32 _Y1, D3DCOLOR _Color);
	void drawLine(Vec2 *ptr, u32 num, D3DCOLOR color);
	void drawCross(Vec2 pos, u32 r, D3DXCOLOR color);

	void drawText(f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...);
	void drawText(RECT rect, u32 color, D3DCOLOR flag, const c8 *str, ...);
	void drawTextCenter(f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...);
	void drawTextSmall(f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...);
	void drawTextSmall(RECT rect, u32 color, D3DCOLOR flag, const c8 *str, ...);
	void drawTextSmallCenter(f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...);
	void drawTextBig(f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...);
	void drawTextBig(RECT rect, u32 color, D3DCOLOR flag, const c8 *str, ...);
	void drawTextBigCenter(f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...);
	void drawTextEx(string fontName, f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...);

	inline Mat* getQuadMatNotRHW() { return &m_quadMat; }

protected:
	typedef map< string, Font* >			FontsMap;
	typedef map< string, Font* >::iterator	FontsMapIt;

	LPD3DXLINE				m_d3dLine;
	Font					*font24, *font14, *font10;
	FontsMap				fonts;
	LPDIRECT3DVERTEXBUFFER9 m_VBFullscreenQuad;
	LPDIRECT3DVERTEXBUFFER9 m_VBFullscreenQuadNotRHW;
	Mat						m_quadMat;
	u32						m_initialized;
};
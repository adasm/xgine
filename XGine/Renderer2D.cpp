#include "Renderer2D.h"

struct VertexTransformedTextured
{
	D3DXVECTOR4 p;
	float		u;
	float		v;
};
const DWORD VertexTransformedTextured_FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

struct VertexTextured
{
	Vec3 p;
	Vec2 uv;
};
const DWORD VertexTextured_FVF = D3DFVF_XYZ | D3DFVF_TEX1;

#define RENDER2D_ENABLED

Renderer2D::Renderer2D()
	: m_initialized(0), m_d3dLine(NULL), font14(NULL), font10(NULL), m_VBFullscreenQuad(0)
{
}

u32 Renderer2D::init()
{
	if(m_initialized)return 1;

	if(FAILED(D3DXCreateLine(gEngine.device->getDev(), &m_d3dLine))){ gEngine.kernel->log->prnEx(LT_ERROR, "Renderer2D", "Couldn't create d3dline."); return 0; }
	
	font10 = gEngine.resMgr->load<Font>("'Lucida Console' 10");
	font14 = gEngine.resMgr->load<Font>("'Lucida Console' 14");
	font24 = gEngine.resMgr->load<Font>("'Lucida Console' 24");
	
	// Create full screen quad VB
	float width = gEngine.device->getWidth();
	float height = gEngine.device->getHeight();
	VertexTransformedTextured* data;
	gEngine.device->getDev()->CreateVertexBuffer( 4*sizeof(VertexTransformedTextured), D3DUSAGE_WRITEONLY, VertexTransformedTextured_FVF,
		D3DPOOL_MANAGED, &m_VBFullscreenQuad, 0 );
	m_VBFullscreenQuad->Lock( 0, 0, (void**)&data, 0 );
	f32 rx = -0.5;
	f32 ry = -0.5;
	data[0].p = D3DXVECTOR4(rx, height+ry, 0.0f, 1.0f);
	data[0].u = 0.0f;
	data[0].v = 1.0f;
	data[1].p = D3DXVECTOR4(width+rx, height+ry, 0.0f, 1.0f);
	data[1].u = 1.0f;
	data[1].v = 1.0f;
	data[2].p = D3DXVECTOR4(rx, ry, 0.0f, 1.0f);
	data[2].u = 0.0f;
	data[2].v = 0.0f;
	data[3].p = D3DXVECTOR4(width+rx, ry, 0.0f, 1.0f);
	data[3].u = 1.0f;
	data[3].v = 0.0f;
	m_VBFullscreenQuad->Unlock();

    gEngine.device->getDev()->CreateVertexBuffer( 4 * sizeof(VertexTextured), D3DUSAGE_WRITEONLY, VertexTextured_FVF, D3DPOOL_DEFAULT, &m_VBFullscreenQuadNotRHW, NULL);
    VertexTextured *pBuff;
	f32 u_adjust = 0.5f / gEngine.device->getWidth();
    f32 v_adjust = 0.5f / gEngine.device->getHeight();
	m_VBFullscreenQuadNotRHW->Lock(0, 0,(void**)&pBuff, 0);
    for (u32 i = 0; i < 4; ++i)
    {
        pBuff->p = Vec3((i==0 || i==1) ? -1.0f : 1.0f,
                        (i==0 || i==3) ? -1.0f : 1.0f,
                        0.0f);
        pBuff->uv  = Vec2(((i==0 || i==1) ? 0.0f : 1.0f) + u_adjust, 
                          ((i==0 || i==3) ? 1.0f : 0.0f) + v_adjust);
        pBuff++; 
    }
    m_VBFullscreenQuadNotRHW->Unlock();
	
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
    D3DXMATRIX matProj;
    D3DXMATRIX matViewProj;
    D3DXVECTOR3 const vEyePt      = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 const vLookatPt   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 const   vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
    D3DXMatrixOrthoLH(&matProj, 4.0f, 4.0f, 0.2f, 20.0f);
    D3DXMatrixMultiply(&matViewProj, &matView, &matProj);
    D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);
    D3DXMatrixMultiply(&m_quadMat, &matWorld, &matViewProj);

	m_initialized = 1;
	return 1;
}

void Renderer2D::close()
{
	DXRELEASE(m_VBFullscreenQuad);
	DXRELEASE(m_d3dLine);
	gEngine.resMgr->release(font10);
	gEngine.resMgr->release(font14);
	gEngine.resMgr->release(font24);

	for(FontsMapIt it = fonts.begin(); it != fonts.end(); it++)
	{
		gEngine.resMgr->release(it->second);
	}
	fonts.clear();
}

void Renderer2D::begin()
{
#ifdef RENDER2D_ENABLED
	gEngine.renderer->state.setZEnable(FALSE);
	gEngine.renderer->state.setCullMode(D3DCULL_NONE);
	gEngine.renderer->state.setAlphaTestEnable(FALSE);
	gEngine.renderer->state.setAlphaBlendEnable(TRUE);
	gEngine.renderer->state.setBlendOp(D3DBLENDOP_ADD);
	gEngine.renderer->state.setSrcBlend(D3DBLEND_SRCALPHA);
	gEngine.renderer->state.setDestBlend(D3DBLEND_INVSRCALPHA);
	gEngine.renderer->state.setClipPlaneEnable(0);
	gEngine.renderer->state.setFillMode(D3DFILL_SOLID);
	gEngine.renderer->state.setFogEnable(FALSE);
	gEngine.renderer->state.setStencilEnable(FALSE);
	gEngine.renderer->state.setColorWriteEnable(0x0000000f);
	gEngine.renderer->state.setScissorTestEnable(FALSE);
	gEngine.renderer->state.setSeparateAlphaBlendEnable(FALSE);
	gEngine.renderer->state.setAntialiasedLineEnable(TRUE);
	gEngine.renderer->state.setLighting(FALSE);

	gEngine.renderer->setShader(0);

	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	gEngine.renderer->commitChanges();
#endif
}

void Renderer2D::end()
{
#ifdef RENDER2D_ENABLED
	gEngine.renderer->state.setAlphaBlendEnable(FALSE);
	gEngine.renderer->state.setCullMode(D3DCULL_CW);

	gEngine.device->getDev()->SetTexture(0, NULL);
#endif
}

void Renderer2D::drawQuad(f32 _X0, f32 _Y0, f32 _X1, f32 _Y1, D3DCOLOR _Color00, D3DCOLOR _Color10, D3DCOLOR _Color01, D3DCOLOR _Color11)
{
#ifdef RENDER2D_ENABLED
	if(_X0<_X1)
		++_X1;
	else if(_X0>_X1)
		++_X0;
	if(_Y0<_Y1)
		++_Y1;
	else if(_Y0>_Y1)
		++_Y0;

	struct CVtx
	{
		float m_Pos[4];
		DWORD m_Color;
	};
	CVtx p[4];

	p[0].m_Pos[0] = (float)_X1;
	p[0].m_Pos[1] = (float)_Y0;
	p[0].m_Pos[2] = 0;
	p[0].m_Pos[3] = 1;
	p[0].m_Color  = _Color10;

	p[1].m_Pos[0] = (float)_X0;
	p[1].m_Pos[1] = (float)_Y0;
	p[1].m_Pos[2] = 0;
	p[1].m_Pos[3] = 1;
	p[1].m_Color  = _Color00;

	p[2].m_Pos[0] = (float)_X1;
	p[2].m_Pos[1] = (float)_Y1;
	p[2].m_Pos[2] = 0;
	p[2].m_Pos[3] = 1;
	p[2].m_Color  = _Color11;

	p[3].m_Pos[0] = (float)_X0;
	p[3].m_Pos[1] = (float)_Y1;
	p[3].m_Pos[2] = 0;
	p[3].m_Pos[3] = 1;
	p[3].m_Color  = _Color01;

	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	gEngine.device->getDev()->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
	gEngine.device->getDev()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CVtx));
#endif
}

void Renderer2D::drawQuad(Texture *_tex, f32 _X0, f32 _Y0, f32 _X1, f32 _Y1, D3DCOLOR _Color00, D3DCOLOR _Color10, D3DCOLOR _Color01, D3DCOLOR _Color11)
{
#ifdef RENDER2D_ENABLED
	if(!_tex)return;

	if(_X0<_X1)
		++_X1;
	else if(_X0>_X1)
		++_X0;
	if(_Y0<_Y1)
		++_Y1;
	else if(_Y0>_Y1)
		++_Y0;

	struct CVtx
	{
		float m_Pos[4];
		DWORD m_Color;
		float m_Tex[2];
	};
	CVtx p[4];

	p[0].m_Pos[0] = (float)_X1;
	p[0].m_Pos[1] = (float)_Y0;
	p[0].m_Pos[2] = 0;
	p[0].m_Pos[3] = 1;
	p[0].m_Color  = _Color10;
	p[0].m_Tex[0] = 1.0f;
	p[0].m_Tex[1] = 0.0f;

	p[1].m_Pos[0] = (float)_X0;
	p[1].m_Pos[1] = (float)_Y0;
	p[1].m_Pos[2] = 0;
	p[1].m_Pos[3] = 1;
	p[1].m_Color  = _Color00;
	p[1].m_Tex[0] = 0.0f;
	p[1].m_Tex[1] = 0.0f;

	p[2].m_Pos[0] = (float)_X1;
	p[2].m_Pos[1] = (float)_Y1;
	p[2].m_Pos[2] = 0;
	p[2].m_Pos[3] = 1;
	p[2].m_Color  = _Color11;
	p[2].m_Tex[0] = 1.0f;
	p[2].m_Tex[1] = 1.0f;

	p[3].m_Pos[0] = (float)_X0;
	p[3].m_Pos[1] = (float)_Y1;
	p[3].m_Pos[2] = 0;
	p[3].m_Pos[3] = 1;
	p[3].m_Color  = _Color01;
	p[3].m_Tex[0] = 0.0f;
	p[3].m_Tex[1] = 1.0f;

	gEngine.device->getDev()->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_CURRENT);
	gEngine.device->getDev()->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	gEngine.device->getDev()->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	gEngine.device->getDev()->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	gEngine.device->getDev()->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	gEngine.device->getDev()->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_MODULATE); 

	gEngine.device->getDev()->SetTexture(0, _tex->get());

	gEngine.device->getDev()->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1);			
	gEngine.device->getDev()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CVtx));
#endif
}

void Renderer2D::drawQuad(IDirect3DTexture9* _tex, f32 _X0, f32 _Y0, f32 _X1, f32 _Y1, D3DCOLOR _Color00, D3DCOLOR _Color10, D3DCOLOR _Color01, D3DCOLOR _Color11)
{
#ifdef RENDER2D_ENABLED
	if(!_tex)return;

	if(_X0<_X1)
		++_X1;
	else if(_X0>_X1)
		++_X0;
	if(_Y0<_Y1)
		++_Y1;
	else if(_Y0>_Y1)
		++_Y0;

	struct CVtx
	{
		float m_Pos[4];
		float m_Tex[2];
	};
	CVtx p[4];

	p[0].m_Pos[0] = (float)_X1;
	p[0].m_Pos[1] = (float)_Y0;
	p[0].m_Pos[2] = 0;
	p[0].m_Pos[3] = 1;
	p[0].m_Tex[0] = 1.0f;
	p[0].m_Tex[1] = 0.0f;

	p[1].m_Pos[0] = (float)_X0;
	p[1].m_Pos[1] = (float)_Y0;
	p[1].m_Pos[2] = 0;
	p[1].m_Pos[3] = 1;
	p[1].m_Tex[0] = 0.0f;
	p[1].m_Tex[1] = 0.0f;

	p[2].m_Pos[0] = (float)_X1;
	p[2].m_Pos[1] = (float)_Y1;
	p[2].m_Pos[2] = 0;
	p[2].m_Pos[3] = 1;
	p[2].m_Tex[0] = 1.0f;
	p[2].m_Tex[1] = 1.0f;

	p[3].m_Pos[0] = (float)_X0;
	p[3].m_Pos[1] = (float)_Y1;
	p[3].m_Pos[2] = 0;
	p[3].m_Pos[3] = 1;
	p[3].m_Tex[0] = 0.0f;
	p[3].m_Tex[1] = 1.0f;

	gEngine.device->getDev()->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_CURRENT);
	gEngine.device->getDev()->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	gEngine.device->getDev()->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	gEngine.device->getDev()->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	gEngine.device->getDev()->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	gEngine.device->getDev()->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_MODULATE); 

	gEngine.device->getDev()->SetTexture(0, _tex);

	gEngine.device->getDev()->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX1);			
	gEngine.device->getDev()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CVtx));
#endif
}

void Renderer2D::drawQuadTexMatch(f32 _X0, f32 _Y0, f32 _X1, f32 _Y1)
{
#ifdef RENDER2D_ENABLED
	if(_X0<_X1)
		++_X1;
	else if(_X0>_X1)
		++_X0;
	if(_Y0<_Y1)
		++_Y1;
	else if(_Y0>_Y1)
		++_Y0;

	struct CVtx
	{
		float m_Pos[4];
		float m_Tex[2];
	};
	CVtx p[4];

	p[0].m_Pos[0] = (float)_X1;
	p[0].m_Pos[1] = (float)_Y0;
	p[0].m_Pos[2] = 0;
	p[0].m_Pos[3] = 1;


	p[1].m_Pos[0] = (float)_X0;
	p[1].m_Pos[1] = (float)_Y0;
	p[1].m_Pos[2] = 0;
	p[1].m_Pos[3] = 1;


	p[2].m_Pos[0] = (float)_X1;
	p[2].m_Pos[1] = (float)_Y1;
	p[2].m_Pos[2] = 0;
	p[2].m_Pos[3] = 1;


	p[3].m_Pos[0] = (float)_X0;
	p[3].m_Pos[1] = (float)_Y1;
	p[3].m_Pos[2] = 0;
	p[3].m_Pos[3] = 1;

	for(u32 i = 0; i < 4; i++)
	{
		p[i].m_Tex[0] = (f32) p[i].m_Pos[0] / (f32) gEngine.device->getWidth();
		p[i].m_Tex[1] = (f32) p[i].m_Pos[1] / (f32) gEngine.device->getHeight();
	}

	gEngine.device->getDev()->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX1);			
	gEngine.device->getDev()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CVtx));
#endif
}

void Renderer2D::drawQuadTextured(u32 width, u32 height)
{
#ifdef RENDER2D_ENABLED
	struct CVtx
	{
		float m_Pos[4];
		float m_Tex[2];
	};
	CVtx p[4];
	p[0].m_Pos[0] = (float)width;
	p[0].m_Pos[1] = (float)0;
	p[0].m_Pos[2] = 0;
	p[0].m_Pos[3] = 1;
	p[0].m_Tex[0] = 1.0f;
	p[0].m_Tex[1] = 0.0f;
	p[1].m_Pos[0] = (float)0;
	p[1].m_Pos[1] = (float)0;
	p[1].m_Pos[2] = 0;
	p[1].m_Pos[3] = 1;
	p[1].m_Tex[0] = 0.0f;
	p[1].m_Tex[1] = 0.0f;
	p[2].m_Pos[0] = (float)width;
	p[2].m_Pos[1] = (float)height;
	p[2].m_Pos[2] = 0;
	p[2].m_Pos[3] = 1;
	p[2].m_Tex[0] = 1.0f;
	p[2].m_Tex[1] = 1.0f;
	p[3].m_Pos[0] = (float)0;
	p[3].m_Pos[1] = (float)height;
	p[3].m_Pos[2] = 0;
	p[3].m_Pos[3] = 1;
	p[3].m_Tex[0] = 0.0f;
	p[3].m_Tex[1] = 1.0f;

	gEngine.device->getDev()->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX1);			
	gEngine.device->getDev()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CVtx));
#endif
}

void Renderer2D::drawScreenQuad(u32 color, u32 textured)
{
#ifdef RENDER2D_ENABLED
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLORARG1, (textured ? D3DTA_TEXTURE : D3DTA_CURRENT));
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CONSTANT);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_CONSTANT, color);
	gEngine.device->getDev()->SetFVF( VertexTransformedTextured_FVF );
	gEngine.device->getDev()->SetStreamSource(0, m_VBFullscreenQuad, 0, sizeof(VertexTransformedTextured));
	gEngine.device->getDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
#endif
}

void Renderer2D::drawScreenQuad(IDirect3DTexture9* _tex)
{
#ifdef RENDER2D_ENABLED
	gEngine.device->getDev()->SetTexture(0, _tex);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CONSTANT);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_CONSTANT, 0xffffffff);
	gEngine.device->getDev()->SetFVF( VertexTransformedTextured_FVF );
	gEngine.device->getDev()->SetStreamSource(0, m_VBFullscreenQuad, 0, sizeof(VertexTransformedTextured));
	gEngine.device->getDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	gEngine.device->getDev()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
#endif
}

void Renderer2D::drawScreenQuadRaw()
{
#ifdef RENDER2D_ENABLED
	gEngine.device->getDev()->SetFVF( VertexTransformedTextured_FVF );
	gEngine.device->getDev()->SetStreamSource(0, m_VBFullscreenQuad, 0, sizeof(VertexTransformedTextured));
	gEngine.device->getDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
#endif
}

void Renderer2D::drawScreenQuadNotRHW()
{
#ifdef RENDER2D_ENABLED
	gEngine.device->getDev()->SetFVF( VertexTextured_FVF );
	gEngine.device->getDev()->SetStreamSource(0, m_VBFullscreenQuadNotRHW, 0, sizeof(VertexTextured));
	gEngine.device->getDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
#endif
}

void Renderer2D::drawRect(f32 _X0, f32 _Y0, f32 _X1, f32 _Y1, D3DCOLOR _Color)
{
#ifdef RENDER2D_ENABLED
	Vec2 list[8] = { Vec2(_X0,_Y0), Vec2(_X1,_Y0),
					 Vec2(_X1,_Y0), Vec2(_X1,_Y1),
					 Vec2(_X1,_Y1), Vec2(_X0,_Y1),
					 Vec2(_X0,_Y1), Vec2(_X0,_Y0) };

	m_d3dLine->Draw(list,8,_Color);
#endif
}

void Renderer2D::drawLine(Vec2 *ptr, u32 num, D3DCOLOR color)
{
#ifdef RENDER2D_ENABLED
	m_d3dLine->Draw(ptr,num,color);
#endif
}

void Renderer2D::drawCross(Vec2 pos, u32 r, D3DXCOLOR color)
{
#ifdef RENDER2D_ENABLED
	Vec2 l[2] = {Vec2(pos.x-r,pos.y),Vec2(pos.x+r,pos.y)};
	Vec2 l2[2] = {Vec2(pos.x,pos.y-r),Vec2(pos.x,pos.y+r)};

	drawLine(l,2,color);
	drawLine(l2,2,color);
#endif
}

void Renderer2D::drawText(f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...)
{
#ifdef RENDER2D_ENABLED
	if(font14 && font14->lpFont)
	{
		static c8 szBuf[1024];
		va_list args; va_start(args,str);
		vsprintf_s(szBuf,1024,str,args);
		std::string strText = szBuf;
		RECT rect;
		rect.top = _Y0;
		rect.left = _X0;
		rect.right = gEngine.device->getWidth();
		rect.bottom = gEngine.device->getHeight();
		font14->lpFont->DrawText(NULL, strText.c_str(),strText.length(),&rect,DT_LEFT|DT_NOCLIP,color);
	}
#endif
}

void Renderer2D::drawText(RECT rect, u32 color, D3DCOLOR flag, const c8 *str, ...)
{
#ifdef RENDER2D_ENABLED
	if(font14 && font14->lpFont)
	{
		static c8 szBuf[1024];
		va_list args; va_start(args,str);
		vsprintf_s(szBuf,1024,str,args);
		std::string strText = szBuf;
		font14->lpFont->DrawText(NULL, strText.c_str(),strText.length(),&rect,flag,color);
	}
#endif
}

void Renderer2D::drawTextCenter(f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...)
{
#ifdef RENDER2D_ENABLED
	if(font14 && font14->lpFont)
	{
		static c8 szBuf[1024];
		va_list args; va_start(args,str);
		vsprintf_s(szBuf,1024,str,args);
		std::string strText = szBuf;
		RECT rect;
		rect.top = _Y0;
		rect.left = _X0;
		rect.right = gEngine.device->getWidth();
		rect.bottom = gEngine.device->getHeight();
		font14->lpFont->DrawText(NULL, strText.c_str(),strText.length(),&rect,DT_CENTER|DT_NOCLIP,color);
	}
#endif
}

void Renderer2D::drawTextSmall(f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...)
{
#ifdef RENDER2D_ENABLED
	if(font10 && font10->lpFont)
	{
		static c8 szBuf[4096];
		va_list args; va_start(args,str);
		vsprintf_s(szBuf,4096,str,args);
		std::string strText = szBuf;
		RECT rect;
		rect.top = _Y0;
		rect.left = _X0;
		rect.right = gEngine.device->getWidth();
		rect.bottom = gEngine.device->getHeight();
		font10->lpFont->DrawText(NULL, strText.c_str(),strText.length(),&rect,DT_LEFT|DT_NOCLIP,color);
	}
#endif
}

void Renderer2D::drawTextSmall(RECT rect, u32 color, D3DCOLOR flag, const c8 *str, ...)
{
#ifdef RENDER2D_ENABLED
	if(font10 && font10->lpFont)
	{
		static c8 szBuf[4096];
		va_list args; va_start(args,str);
		vsprintf_s(szBuf,4096,str,args);
		std::string strText = szBuf;
		font10->lpFont->DrawText(NULL, strText.c_str(),strText.length(),&rect,flag,color);
	}
#endif
}

void Renderer2D::drawTextSmallCenter(f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...)
{
#ifdef RENDER2D_ENABLED
	if(font10 && font10->lpFont)
	{
		static c8 szBuf[1024];
		va_list args; va_start(args,str);
		vsprintf_s(szBuf,1024,str,args);
		std::string strText = szBuf;
		RECT rect;
		rect.top = _Y0;
		rect.left = _X0;
		rect.right = gEngine.device->getWidth();
		rect.bottom = gEngine.device->getHeight();
		font10->lpFont->DrawText(NULL, strText.c_str(),strText.length(),&rect,DT_CENTER|DT_NOCLIP,color);
	}
#endif
}

void Renderer2D::drawTextBig(f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...)
{
#ifdef RENDER2D_ENABLED
	if(font24 && font24->lpFont)
	{
		static c8 szBuf[4096];
		va_list args; va_start(args,str);
		vsprintf_s(szBuf,4096,str,args);
		std::string strText = szBuf;
		RECT rect;
		rect.top = _Y0;
		rect.left = _X0;
		rect.right = gEngine.device->getWidth();
		rect.bottom = gEngine.device->getHeight();
		font24->lpFont->DrawText(NULL, strText.c_str(),strText.length(),&rect,DT_LEFT|DT_NOCLIP,color);
	}
#endif
}

void Renderer2D::drawTextBig(RECT rect, u32 color, D3DCOLOR flag, const c8 *str, ...)
{
#ifdef RENDER2D_ENABLED
	if(font24 && font24->lpFont)
	{
		static c8 szBuf[4096];
		va_list args; va_start(args,str);
		vsprintf_s(szBuf,4096,str,args);
		std::string strText = szBuf;
		font24->lpFont->DrawText(NULL, strText.c_str(),strText.length(),&rect,flag,color);
	}
#endif
}

void Renderer2D::drawTextBigCenter(f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...)
{
#ifdef RENDER2D_ENABLED
	if(font24 && font24->lpFont)
	{
		static c8 szBuf[1024];
		va_list args; va_start(args,str);
		vsprintf_s(szBuf,1024,str,args);
		std::string strText = szBuf;
		RECT rect;
		rect.top = _Y0;
		rect.left = _X0;
		rect.right = gEngine.device->getWidth();
		rect.bottom = gEngine.device->getHeight();
		font24->lpFont->DrawText(NULL, strText.c_str(),strText.length(),&rect,DT_CENTER|DT_NOCLIP,color);
	}
#endif
}

void Renderer2D::drawTextEx(string fontName, f32 _X0, f32 _Y0, D3DCOLOR color, const c8 *str, ...)
{
#ifdef RENDER2D_ENABLED
	Font *font = 0;

	FontsMapIt it = fonts.find(fontName);
	if(it != fonts.end())
	{
		font = it->second;
	}
	else
	{
		font = gEngine.resMgr->load<Font>(fontName);
		if(!font)return;
		fonts.insert(make_pair(fontName, font));
	}

	if(font && font->lpFont)
	{
		static c8 szBuf[4096];
		va_list args; va_start(args,str);
		vsprintf_s(szBuf,4096,str,args);
		std::string strText = szBuf;
		RECT rect;
		rect.top = _Y0;
		rect.left = _X0;
		rect.right = gEngine.device->getWidth();
		rect.bottom = gEngine.device->getHeight();
		font->lpFont->DrawText(NULL, strText.c_str(),strText.length(),&rect,DT_LEFT|DT_NOCLIP,color);
	}
#endif
}


#pragma once
#include "XGine.h"

struct  XGINE_API SkyDome
{
protected:
	LPDIRECT3DVERTEXBUFFER9 m_vb;
	LPDIRECT3DINDEXBUFFER9  m_ib;
	u32						m_indices;
	Shader*					m_shader;
	u32						NumOfVertices;
	u32						NumOfFaces;
	Texture*				m_textureFront;
	Texture*				m_textureBack;
	f32						m_textureFrontScale;
	f32						m_textureBackScale;
	f32						m_slideFactorFront;
	f32						m_slideFactorBack;
	Vec2					m_slideDir;
	Vec4					m_colorFront;
	Vec4					m_colorBack;
	Light*					g_sunLight;
	Mat						world;
	Mesh*					plane;

public:
	SkyDome(Texture* textureFront, Texture* textureBack, f32 textureFrontScale, f32 textureBackScale, Vec4 colorFront, Vec4 colorBack, f32 slideFront, f32 slideBack, Vec2 slideDir, u32 hori_res, u32 vert_res, float image_percentage, float half_sphere_fraction);
	~SkyDome();

	void			update(ICamera* cam = 0);
	u32				render();
	u32				renderSun();
	void			setSunLight(Light *light) { g_sunLight = light; }

	f32				getTextureFrontScale() { return m_textureFrontScale; }
	void			setTextureFrontScale(f32 scale) { m_textureFrontScale = scale; }
	f32				getTextureBackScale() { return m_textureBackScale; }
	void			setTextureBackScale(f32 scale) { m_textureBackScale = scale; }
	f32				getSlideFactorFront() { return m_slideFactorFront; }
	void			setSlideFactorFront(f32 factor) { m_slideFactorFront = factor; }
	f32				getSlideFactorBack() { return m_slideFactorBack; }
	void			setSlideFactorBack(f32 factor) { m_slideFactorBack = factor; }
	Vec4			getColorFront() { return m_colorFront; }
	void			setColorFront(Vec4 color) { m_colorFront = color; }
	Vec4			getColorBack() { return m_colorBack; }
	void			setColorBack(Vec4 color) { m_colorBack = color; }
	Vec2			getSlideDir() { return m_slideDir; }
	void			setSlideDir(Vec2 slideDir) { m_slideDir = slideDir; }	
};
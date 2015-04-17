#pragma once
#include "XGine.h"

struct XGINE_API Vegetation : IEntity
{
protected:
	LPDIRECT3DVERTEXBUFFER9			m_vb;
	LPDIRECT3DINDEXBUFFER9			m_ib;
	i32								m_num;
	Texture*						m_texture;
	BillboardRotation				m_rot;
	
public:
	Vegetation(const char* textureName, Vec3 *positions, u32 number, u32 size, BillboardRotation rot);
	~Vegetation();

	void	update(ICamera* cam = 0);
	u32		renderDeferred();
	u32		renderForward(Light* light = 0);
	u32		renderRaw();
	void	setSunLight(Light *light) { }
	void	updateOrigin();	
	void	setPosition(Vec3 &pos)	{ position = (pos); }

	inline u32			rendEnabled()	{ return 1; }
	inline u32			selfRendering() { return 1; }
	inline Mesh*		getMesh()		{ return 0; }
	inline Material*	getMaterial()	{ return 0; }
	inline Shader*		getShader()		{ return shader; }
	inline Texture*		getTexture()	{ return m_texture; }
	inline u32			hasShadow()		{ return 0; }
		   REND_TYPE	getRendType();
};
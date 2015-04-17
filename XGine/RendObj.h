#pragma once
#include "XGine.h"

struct XGINE_API RendObj : IEntity
{
	RendObj(string meshName, Material* material, Vec3 position, u32 hasShadow = 1);
	~RendObj();

	void	update(ICamera* cam);
	u32		renderDeferred();
	u32		renderForward(Light* light = 0);
	u32		renderRaw();
	void	setSunLight(Light *light) { }
	void	updRotBB(Vec3 rot);

	inline u32			selfRendering() { return 1; }
	inline Mesh*		getMesh()		{ return (currentNode)?(currentNode->mesh):(0); }
	inline Material*	getMaterial()	{ return (currentNode)?(currentNode->material):(0); }
		   Shader*		getShader();
		   Texture*		getTexture();
	inline u32			hasShadow()		{ return m_hasShadow; }
		   REND_TYPE	getRendType();

private:
	u32				m_hasShadow;
	vector<Light*>  m_lights;
};
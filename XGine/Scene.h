#pragma once
#include "XGine.h"

struct XGINE_API Scene
{
	Scene(ICamera* camera, u32 skybox = 1);
	~Scene();

	void add(Surface* entity, u32 dynamic = 1);
	void add(Terrain* entity);
	void addWater(f32 H);
	void add(IEntity* entity, u32 dynamic = 1);
	void add(Light* light);

	void update();
	void preRender();
	void render();

	void load();

	void		setSun(Vec3 dir, Vec4 color) { g_sunDir = dir; g_sunColor = color; }
	ICamera*	getCamera()	{ return m_camera; }
	RenderList* getRendList() { return m_renderList; }
	SkyDome*	getSky() { return m_sky; }
	f32			getSunBright() { return m_bright; }

protected:
	ICamera*	m_camera;
	RenderList*	m_renderList;
	SkyDome*	m_sky;
	Light*		g_skyLight;
	Vec3		g_sunDir;
	Vec4		g_sunColor;
	Vec4		g_skyFront, g_skyBack;
	f32			m_bright;
};


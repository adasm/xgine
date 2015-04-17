#include "HeatHaze.h"

HeatHaze::HeatHaze()
{
	shader = 0;
}

HeatHaze::~HeatHaze()
{
}

u32 HeatHaze::init()
{
	if(shader)return 1;
	shader = gEngine.shaderMgr->load("HeatHaze.fx");
	return (shader != 0);
}

void HeatHaze::close()
{
	shader = 0;
}

void HeatHaze::process()
{
	if(!shader)return;

	Mat g_matInvViewProj;
	D3DXMatrixInverse(&g_matInvViewProj, 0, gEngine.renderer->get().camera->getViewProjectionMat());
	
	gEngine.renderer->state.setAlphaBlendEnable(FALSE);

	gEngine.renderer->setShader(shader->setTech("HeatHaze"));
	shader->setTex("DepthBuffer", gEngine.renderer->getMRT()->getRT(2));
	shader->setTex("ScreenTexture", gEngine.renderer->getBackBuffer(0)->getColorTexture());
	shader->setMat("g_matInvViewProj", &g_matInvViewProj);
	shader->setVec4("g_viewPos", gEngine.renderer->get().camera->getPositionV4());
	Vec2 hazeStrength = Vec2((f32)r_heatHazeStrength/(f32)gEngine.device->getWidth(), (f32)r_heatHazeStrength/(f32)gEngine.device->getHeight());
	shader->setVec2("g_heatHazeStrength", &hazeStrength);
	shader->setF32("g_time", (f32)gEngine.kernel->tmr->getFrameTime() * (f32)r_heatHazeTimeScale);
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawScreenQuadRaw();
}
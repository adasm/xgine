#include "VolumetricLightScattering.h"

u32 VolumetricLightScattering::init()
{
	if(initialized)return 1;

	shader = gEngine.shaderMgr->load("VLS.fx");
	if(!shader)
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "VolumetricLightScattering", "Couldn't initialize VolumetricLightScattering");
		close();
		return 0;
	}

	RenderTexture::CreationParams cp;
	texture = new RenderTexture();
	cp.width = gEngine.device->getWidth();
	cp.height = gEngine.device->getHeight();
	cp.colorFormat = D3DFMT_A8R8G8B8;
	cp.depthFormat = D3DFMT_D24S8;
	if(!texture->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "VolumetricLightScattering", "Couldn't initialize VolumetricLightScattering");
		close();
		return 0;
	}

	tempEffect = new RenderTexture();
	cp.width = gEngine.device->getWidth();
	cp.height = gEngine.device->getHeight();
	cp.colorFormat = D3DFMT_A8R8G8B8;
	cp.depthFormat = D3DFMT_D24S8;
	if(!tempEffect->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "VolumetricLightScattering", "Couldn't initialize VolumetricLightScattering");
		close();
		return 0;
	}

	blured = new RenderTexture();
	cp.width = gEngine.device->getWidth();
	cp.height = gEngine.device->getHeight();
	cp.colorFormat = D3DFMT_A8R8G8B8;
	cp.depthFormat = D3DFMT_D24S8;
	if(!blured->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "VolumetricLightScattering", "Couldn't initialize VolumetricLightScattering");
		close();
		return 0;
	}

	finalEffect = new RenderTexture();
	cp.width = gEngine.device->getWidth();
	cp.height = gEngine.device->getHeight();
	cp.colorFormat = D3DFMT_A8R8G8B8;
	cp.depthFormat = D3DFMT_D24S8;
	if(!finalEffect->init(cp))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "VolumetricLightScattering", "Couldn't initialize VolumetricLightScattering");
		close();
		return 0;
	}

	guassianBlur = new GuassianBlur();
	guassianBlur->init(7);
	
	initialized = 1;

	return 1;
}

void VolumetricLightScattering::close()
{
	initialized = 0;
	shader = 0;
	if(finalEffect)finalEffect->close();
	delete(finalEffect); finalEffect = 0;
	if(blured)blured->close();
	delete(blured); blured = 0;
	if(tempEffect)tempEffect->close();
	delete(tempEffect); tempEffect = 0;
	if(texture)texture->close();
	delete(texture); texture = 0;
}


void VolumetricLightScattering::prepare()
{
	if(!initialized)return;

	//ScreenSpace light // r_vlsEnabled
	Vec3 sunPosScreen;
	Vec2 vecSunToCenter;
	Mat world;
	Vec2 screenCenter;
	D3DVIEWPORT9 viewport;
	gEngine.device->getDev()->GetViewport(&viewport);
	D3DXMatrixTranslation(&world, 0, 0, 0);
	D3DXVec3Project(&sunPosScreen, &sunPos, &viewport, gEngine.renderer->get().camera->getProjectionMat(), gEngine.renderer->get().camera->getViewMat(), &world);
    Vec2 half_texel = Vec2(1.0f/(f32)gEngine.device->getWidth(), 1.0f/(f32)gEngine.device->getHeight());
	Vec2 sunPosSS = Vec2(sunPosScreen);
		 sunPosSS.x *= half_texel.x;
		 sunPosSS.y *= half_texel.y;
		 sunPosSS += 0.5*half_texel;

	finalEffect->enableRendering();
	gEngine.renderer->setShader(shader->setTech("VLS"));
	shader->setVec2("g_sunPosSS", &sunPosSS);
	shader->setTex("Tex0", texture->getColorTexture());
	gEngine.renderer->state.setAlphaBlendEnable(FALSE);
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawScreenQuadRaw();
	finalEffect->disableRendering();

	//guassianBlur->process(tempEffect, blured, finalEffect);
}

void VolumetricLightScattering::draw()
{
	gEngine.renderer->setShader(0);
	gEngine.renderer->state.setAlphaBlendEnable(TRUE);
	gEngine.renderer->state.setBlendOp(D3DBLENDOP_ADD);
	gEngine.renderer->state.setSrcBlend(D3DBLEND_ONE);
	gEngine.renderer->state.setDestBlend(D3DBLEND_ONE);
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawQuad(finalEffect->getColorTexture(), 0, 0, gEngine.device->getWidth(), gEngine.device->getHeight(), 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
	gEngine.renderer->state.setAlphaBlendEnable(FALSE);

	gEngine.renderer->addTxt("Performed VLS");
}
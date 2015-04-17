#include "DepthOfField.h"

DepthOfField::DepthOfField()
{
	shader = 0;
	prevDepth = 0;
	currentDepth = 0;
}

DepthOfField::~DepthOfField()
{
}

u32 DepthOfField::init()
{
	if(shader)return 1;

	shader = gEngine.shaderMgr->load("DOF.fx");

	if(!shader)
	{ shader = 0; gEngine.kernel->log->prnEx(LT_ERROR, "DepthOfField", "Couldn't initialize DepthOfField!"); return 0; }

	RenderTexture::CreationParams cp;
	cp.width = 1;
	cp.height = 1;
	cp.colorFormat = D3DFMT_A16B16G16R16F;
	cp.depthFormat = D3DFMT_D24S8;
	prevDepth = new RenderTexture();
	if(!prevDepth->init(cp))
	{
		delete(prevDepth);
		prevDepth = 0;
		{ shader = 0; gEngine.kernel->log->prnEx(LT_ERROR, "DepthOfField", "Couldn't initialize DepthOfField - Unable to create prevDepth!"); return 0; }
	}

	currentDepth = new RenderTexture();
	if(!currentDepth->init(cp))
	{
		delete(currentDepth);
		currentDepth = 0;
		{ shader = 0; gEngine.kernel->log->prnEx(LT_ERROR, "DepthOfField", "Couldn't initialize DepthOfField - Unable to create currentDepth!"); return 0; }
	}

	return 1;
}

void DepthOfField::close()
{
	shader = 0;

	if(prevDepth)
	{
		prevDepth->close();
		delete(prevDepth);
		prevDepth = 0;
	}

	if(currentDepth)
	{
		currentDepth->close();
		delete(currentDepth);
		currentDepth = 0;
	}
}

void DepthOfField::process()
{
	if(!shader)return;
	
	gEngine.renderer->state.setAlphaBlendEnable(FALSE);

	Mat g_matInvViewProj;
	D3DXMatrixInverse(&g_matInvViewProj, 0, gEngine.renderer->get().camera->getViewProjectionMat());

	if(r_dofEyeAdaptation && prevDepth && currentDepth)
	{
		static RenderTexture* temp;
		temp = prevDepth;
		prevDepth = currentDepth;
		currentDepth = temp;

		currentDepth->enableRendering();
		gEngine.renderer->clear(1,0,0);
		gEngine.renderer->setShader(shader->setTech("DOFAdaptation"));
		shader->setTex("PrevDepth", prevDepth->getColorTexture());
		shader->setTex("DepthBuffer", gEngine.renderer->getMRT()->getRT(2));
		shader->setMat("g_matInvViewProj", &g_matInvViewProj);
		shader->setVec4("g_viewPos", gEngine.renderer->get().camera->getPositionV4());
		shader->setF32("g_time", gEngine.kernel->tmr->get() * 0.001f * r_dofEyeAdaptationTimeScale);
		gEngine.renderer->commitChanges();
		gEngine.renderer->r2d->drawQuadTextured(1,1);
		currentDepth->disableRendering();

		gEngine.renderer->setShader(shader->setTech("DOFAdapted"));
		shader->setTex("DepthBuffer", gEngine.renderer->getMRT()->getRT(2));
		shader->setTex("BlurTexture", gEngine.renderer->getBackBuffer(1)->getColorTexture());
		shader->setTex("CurrentDepth", currentDepth->getColorTexture());
		shader->setTex("ScreenTexture", gEngine.renderer->getBackBuffer(0)->getColorTexture());
		shader->setMat("g_matInvViewProj", &g_matInvViewProj);
		shader->setVec4("g_viewPos", gEngine.renderer->get().camera->getPositionV4());
		shader->setF32("g_dofNearFocalPlane", r_dofNearFocalPlane);
		shader->setF32("g_dofFarFocalPlane", r_dofFarFocalPlane);
		shader->setF32("g_dofBlurExponent", r_dofBlurExponent);
		gEngine.renderer->commitChanges();
		gEngine.renderer->r2d->drawScreenQuadRaw();
	}
	else
	{
		gEngine.renderer->setShader(shader->setTech("DOF"));
		shader->setTex("DepthBuffer", gEngine.renderer->getMRT()->getRT(2));
		shader->setTex("BlurTexture", gEngine.renderer->getBackBuffer(1)->getColorTexture());
		shader->setTex("ScreenTexture", gEngine.renderer->getBackBuffer(0)->getColorTexture());
		shader->setMat("g_matInvViewProj", &g_matInvViewProj);
		shader->setVec4("g_viewPos", gEngine.renderer->get().camera->getPositionV4());
		shader->setF32("g_dofFocalPlane", r_dofFocalPlane);
		shader->setF32("g_dofNearFocalPlane", r_dofNearFocalPlane);
		shader->setF32("g_dofFarFocalPlane", r_dofFarFocalPlane);
		shader->setF32("g_dofBlurExponent", r_dofBlurExponent);
		gEngine.renderer->commitChanges();
		gEngine.renderer->r2d->drawScreenQuadRaw();
	}
}
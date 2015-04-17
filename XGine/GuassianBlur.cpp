#include "GuassianBlur.h"

static Vec2			g_vSampleOffsets[32];
static f32			g_fSampleWeights[32];
static Vec2			g_vSampleOffsetsV[32];
static f32			g_fSampleWeightsV[32];

f32 getGaussianDistribution( f32 n, f32 rho )
{
    return (1.0f / sqrtf(2 * D3DX_PI * rho)) * (f32)exp(-(n*n) / (2 * rho * rho));
}

static f32 root_bluramount = 1.0f;
static f32 bluramount = 1.0f;

void setupGaussianOffsetsH(u32 sampleCount, Vec2 vViewportTexelSize) 
{
    g_fSampleWeights[0] = getGaussianDistribution( 0, floor(sampleCount/2.0f) );
    g_vSampleOffsets[0] = Vec2( 0.0f, 0.0f );
	f32 totalWeights = g_fSampleWeightsV[0];
	for( int i = 1; i < sampleCount; i += 2 ) {
		g_vSampleOffsets[i + 0] = Vec2(  i * vViewportTexelSize.x, 0.0f );
		g_vSampleOffsets[i + 1] = Vec2( -i * vViewportTexelSize.x, 0.0f );
		g_fSampleWeights[i + 0] = getGaussianDistribution( f32(i + 0), floor(sampleCount/2.0f) );
		g_fSampleWeights[i + 1] = getGaussianDistribution( f32(i + 0), floor(sampleCount/2.0f) );
		totalWeights += g_fSampleWeightsV[i + 0] + g_fSampleWeightsV[i + 1];
	}
	for(int i = 0; i < sampleCount; i++)
		g_fSampleWeightsV[i] /= totalWeights;
}
void setupGaussianOffsetsV(u32 sampleCount, Vec2 vViewportTexelSize) 
{
	g_fSampleWeightsV[0] = getGaussianDistribution( 0, floor(sampleCount/2.0f) );
    g_vSampleOffsetsV[0] = Vec2( 0.0f, 0.0f );
	f32 totalWeights = g_fSampleWeightsV[0];
	for( int i = 1; i < sampleCount; i += 2 ) {
		g_vSampleOffsetsV[i + 0] = Vec2( 0.0f,  i * vViewportTexelSize.y );
		g_vSampleOffsetsV[i + 1] = Vec2( 0.0f, -i * vViewportTexelSize.y );
		g_fSampleWeightsV[i + 0] = getGaussianDistribution( f32(i + 0), floor(sampleCount/2.0f) );
		g_fSampleWeightsV[i + 1] = getGaussianDistribution( f32(i + 0), floor(sampleCount/2.0f) );
		totalWeights += g_fSampleWeightsV[i + 0] + g_fSampleWeightsV[i + 1];
	}
	for(int i = 0; i < sampleCount; i++)
		g_fSampleWeightsV[i] /= totalWeights;
}

GuassianBlur::GuassianBlur()
	:	m_initialized(0), m_shader(0), m_uSampleCount(0)
{
}

GuassianBlur::~GuassianBlur()
{
}

u32 GuassianBlur::init(u32 sampleCount)
{
	if(sampleCount < 1 || sampleCount > 15 || (sampleCount%2 == 0))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "GuassianBlur", "Could not initialize GuasssianBlur. Incorrect value of sampleCount(%u).", sampleCount);
		return 0;
	}

	if(!m_initialized || m_uSampleCount != sampleCount)
	{
		m_uSampleCount = sampleCount;
		const c8 *MacroNames[] = { "SAMPLE_COUNT" };
		const u16 MacroBits[] = { 0 };
		Multishader* ms = gEngine.shaderMgr->load("GuassianBlur.fx", "GuassianBlur_%X.fxo", 1, MacroNames, MacroBits);
		u16 MacroValues[] = { m_uSampleCount };
		m_shader = ms->getShader(MacroValues);
	}

	m_initialized = 1;
	return 1;
}

void GuassianBlur::process(RenderTexture* input, RenderTexture* tempBlured, RenderTexture* output)
{
	if(!m_initialized)return;
	processH(input, tempBlured);
	processV(tempBlured, output);
}

void GuassianBlur::processCC(RenderTexture* input, RenderTexture* tempBlured, RenderTexture* output)
{
	if(!m_initialized)return;
	processHC(input, tempBlured);
	processVC(tempBlured, output);
}

void GuassianBlur::processLog(RenderTexture* input, RenderTexture* tempBlured, RenderTexture* output)
{
	if(!m_initialized)return;
	processHL(input, tempBlured);
	processVL(tempBlured, output);
}

void GuassianBlur::processD(LPDIRECT3DTEXTURE9 depthTexture, RenderTexture* input, RenderTexture* tempBlured, RenderTexture* output)
{
	if(!m_initialized)return;
	//m_shader->setTex("DEPTHMAP", depthTexture);
	
	setupGaussianOffsetsH(m_uSampleCount, Vec2(1.0f/output->getParams().width,1.0f/output->getParams().height));

	tempBlured->enableRendering();
	gEngine.device->getDev()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
	gEngine.renderer->setShader(m_shader->setTech("techBlurD"));
	m_shader->setVal("g_vSampleOffsets", g_vSampleOffsets, m_uSampleCount * sizeof(Vec2));
	m_shader->setVal("g_fSampleWeights", g_fSampleWeights, m_uSampleCount * sizeof(f32));
	m_shader->setTex("InputTexture",  input->getColorTexture());
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawQuadTextured(output->getParams().width,output->getParams().height);
	//tempBlured->disableRendering();

	output->enableRendering();
	gEngine.device->getDev()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
	gEngine.renderer->setShader(m_shader->setTech("techBlurD"));
	m_shader->setVal("g_vSampleOffsets", g_vSampleOffsetsV, m_uSampleCount * sizeof(Vec2));
	m_shader->setVal("g_fSampleWeights", g_fSampleWeightsV, m_uSampleCount * sizeof(f32));
	m_shader->setTex("InputTexture",  input->getColorTexture());
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawQuadTextured(output->getParams().width,output->getParams().height);
	output->disableRendering();
	
}

void GuassianBlur::processH(RenderTexture* input, RenderTexture* output)
{
	if(!m_initialized)return;
	setupGaussianOffsetsH(m_uSampleCount, Vec2(1.0f/output->getParams().width,1.0f/output->getParams().height));

	output->enableRendering();
	//gEngine.device->getDev()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
	gEngine.renderer->setShader(m_shader->setTech("techBlur"));
	m_shader->setVal("g_vSampleOffsets", g_vSampleOffsets, m_uSampleCount * sizeof(Vec2));
	m_shader->setVal("g_fSampleWeights", g_fSampleWeights, m_uSampleCount * sizeof(f32));
	m_shader->setTex("InputTexture",  input->getColorTexture());
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawQuadTextured(output->getParams().width,output->getParams().height);
	//output->disableRendering();
}

void GuassianBlur::processV(RenderTexture* input, RenderTexture* output)
{
	if(!m_initialized)return;
	setupGaussianOffsetsV(m_uSampleCount, Vec2(1.0f/output->getParams().width,1.0f/output->getParams().height));

	output->enableRendering();
	//gEngine.device->getDev()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
	gEngine.renderer->setShader(m_shader->setTech("techBlur"));
	m_shader->setVal("g_vSampleOffsets", g_vSampleOffsetsV, m_uSampleCount * sizeof(Vec2));
	m_shader->setVal("g_fSampleWeights", g_fSampleWeightsV, m_uSampleCount * sizeof(f32));
	m_shader->setTex("InputTexture",  input->getColorTexture());
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawQuadTextured(output->getParams().width,output->getParams().height);
	output->disableRendering();
}

void GuassianBlur::processHC(RenderTexture* input, RenderTexture* output)
{
	if(!m_initialized)return;
	setupGaussianOffsetsH(m_uSampleCount, Vec2(1.0f/output->getParams().width,1.0f/output->getParams().height));

	output->enableRendering();
	gEngine.device->getDev()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
	gEngine.renderer->setShader(m_shader->setTech("techBlurC"));
	m_shader->setVal("g_vSampleOffsets", g_vSampleOffsets, m_uSampleCount * sizeof(Vec2));
	m_shader->setVal("g_fSampleWeights", g_fSampleWeights, m_uSampleCount * sizeof(f32));
	m_shader->setTex("InputTexture",  input->getColorTexture());
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawQuadTextured(output->getParams().width,output->getParams().height);
	output->disableRendering();
}

void GuassianBlur::processVC(RenderTexture* input, RenderTexture* output)
{
	if(!m_initialized)return;
	setupGaussianOffsetsV(m_uSampleCount, Vec2(1.0f/output->getParams().width,1.0f/output->getParams().height));

	output->enableRendering();
	gEngine.device->getDev()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
	gEngine.renderer->setShader(m_shader->setTech("techBlurC"));
	m_shader->setVal("g_vSampleOffsets", g_vSampleOffsetsV, m_uSampleCount * sizeof(Vec2));
	m_shader->setVal("g_fSampleWeights", g_fSampleWeightsV, m_uSampleCount * sizeof(f32));
	m_shader->setTex("InputTexture",  input->getColorTexture());
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawQuadTextured(output->getParams().width,output->getParams().height);
	output->disableRendering();
}

void GuassianBlur::processHL(RenderTexture* input, RenderTexture* output)
{
	if(!m_initialized)return;
	setupGaussianOffsetsH(m_uSampleCount, Vec2(1.0f/output->getParams().width,1.0f/output->getParams().height));

	output->enableRendering();
	gEngine.device->getDev()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
	gEngine.renderer->setShader(m_shader->setTech("techBlurL"));
	m_shader->setVal("g_vSampleOffsets", g_vSampleOffsets, m_uSampleCount * sizeof(Vec2));
	m_shader->setVal("g_fSampleWeights", g_fSampleWeights, m_uSampleCount * sizeof(f32));
	m_shader->setTex("InputTexture",  input->getColorTexture());
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawQuadTextured(output->getParams().width,output->getParams().height);
	output->disableRendering();
}

void GuassianBlur::processVL(RenderTexture* input, RenderTexture* output)
{
	if(!m_initialized)return;
	setupGaussianOffsetsV(m_uSampleCount, Vec2(1.0f/output->getParams().width,1.0f/output->getParams().height));

	output->enableRendering();
	gEngine.device->getDev()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
	gEngine.renderer->setShader(m_shader->setTech("techBlurL"));
	m_shader->setVal("g_vSampleOffsets", g_vSampleOffsetsV, m_uSampleCount * sizeof(Vec2));
	m_shader->setVal("g_fSampleWeights", g_fSampleWeightsV, m_uSampleCount * sizeof(f32));
	m_shader->setTex("InputTexture",  input->getColorTexture());
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawQuadTextured(output->getParams().width,output->getParams().height);
	output->disableRendering();
}

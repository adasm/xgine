#include "Bloom.h"

struct VertexTransformedTextured
{
	D3DXVECTOR4 p;
	f32		u;
	f32		v;
};
const DWORD VertexTransformedTextured_FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

Bloom::Bloom()
: m_rt1(0), m_rt2(0), m_fx(0)
{
	f32 width = gEngine.device->getWidth();
	f32 height = gEngine.device->getHeight();

	if(FAILED(D3DXCreateTexture(gEngine.device->getDev(), width/2, height/2, 1,	D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_rt1))
	|| FAILED(D3DXCreateTexture(gEngine.device->getDev(), width/2, height/2, 1,	D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_rt2)))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "Bloom", "Creating textures failed");
		return;
	}

	VertexTransformedTextured* data;

	gEngine.device->getDev()->CreateVertexBuffer(4 * sizeof(VertexTransformedTextured), D3DUSAGE_WRITEONLY, VertexTransformedTextured_FVF,
		D3DPOOL_MANAGED, &m_vbHalf, 0);
	m_vbHalf->Lock(0, 0, (void**)&data, 0);
	data[0].p = D3DXVECTOR4(-0.5f, height/2-0.5f, 0.0f, 1.0f);
	data[0].u = 0.0f;
	data[0].v = 1.0f;
	data[1].p = D3DXVECTOR4(width/2-0.5f, height/2-0.5f, 0.0f, 1.0f);
	data[1].u = 1.0f;
	data[1].v = 1.0f;
	data[2].p = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
	data[2].u = 0.0f;
	data[2].v = 0.0f;
	data[3].p = D3DXVECTOR4(width/2-0.5f, -0.5f, 0.0f, 1.0f);
	data[3].u = 1.0f;
	data[3].v = 0.0f;
	m_vbHalf->Unlock();

	m_fx = gEngine.shaderMgr->load("Bloom.fx");
}

Bloom::~Bloom()
{
	DXRELEASE(m_vbHalf);
	DXRELEASE(m_rt1);
	DXRELEASE(m_rt2);
}

void Bloom::process()
{
	if(!m_fx)return;

	LPDIRECT3DSURFACE9 rt1Surf, rt2Surf;
	m_rt1->GetSurfaceLevel(0, &rt1Surf);
	m_rt2->GetSurfaceLevel(0, &rt2Surf);

	gEngine.device->getDev()->SetFVF(VertexTransformedTextured_FVF);

	//r_bloomAmount
	m_fx->setF32("g_threshold", r_bloomBrightThreshold);
	m_fx->setF32("bloomIntensity", r_bloomIntensity);
	m_fx->setF32("baseIntensity", r_bloomBaseIntensity);
	m_fx->setF32("bloomSaturation", r_bloomSaturation);
	m_fx->setF32("baseSaturation", r_bloomBaseSaturation);
	
	//pass 1 - extract brighter areas
	gEngine.renderer->setRenderTarget(rt1Surf);
	gEngine.renderer->setShader(m_fx->setTech("BloomStep1"));
	m_fx->setTex("Tex0", gEngine.renderer->getBackBuffer(0)->getColorTexture());
	gEngine.renderer->commitChanges();
	gEngine.device->getDev()->SetStreamSource(0, m_vbHalf, 0, sizeof(VertexTransformedTextured));
	gEngine.device->getDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//pass2 - horizontal blur
	gEngine.renderer->setRenderTarget(rt2Surf);
	gEngine.renderer->setShader(m_fx->setTech("BloomStep2"));
	FillBlurParams(1.0f / gEngine.device->getWidth()/2, 0);
	m_fx->setTex("Tex0", m_rt1);
	gEngine.renderer->commitChanges();
	gEngine.device->getDev()->SetStreamSource(0, m_vbHalf, 0, sizeof(VertexTransformedTextured));
	gEngine.device->getDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//pass3 - verical blur
	gEngine.renderer->setRenderTarget(rt1Surf);
	gEngine.renderer->setShader(m_fx->setTech("BloomStep2"));
	FillBlurParams(0, 1.0f / gEngine.device->getHeight()/2);
	m_fx->setTex("Tex0", m_rt2);
	gEngine.renderer->commitChanges();
	gEngine.device->getDev()->SetStreamSource(0, m_vbHalf, 0, sizeof(VertexTransformedTextured));
	gEngine.device->getDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	gEngine.renderer->setRenderTarget(0);
	rt1Surf->Release();
	rt2Surf->Release();

	//draw the final effect
	gEngine.renderer->setShader(m_fx->setTech("BloomStep3"));
	m_fx->setTex("Tex0", m_rt1);
	m_fx->setTex("Tex1", gEngine.renderer->getBackBuffer(0)->getColorTexture());
	gEngine.renderer->commitChanges();
	gEngine.renderer->r2d->drawScreenQuad(0xffffffff, true);
}

void Bloom::FillBlurParams(float dx, float dy)
{
	const int sampleCount = 15;
	float sampleWeights[sampleCount];
	D3DXVECTOR2 sampleOffsets[sampleCount];

	sampleWeights[0] = Gaussian(0);
	sampleOffsets[0].x = sampleOffsets[0].y = 0;

	float totalWeights = sampleWeights[0];

	for(int i = 0; i < sampleCount / 2; i++)
	{
		float weight = Gaussian(i + 1);
		sampleWeights[i * 2 + 1] = sampleWeights[i * 2 + 2] = weight;
		totalWeights += weight * 2;
		float sampleOffset = i * 2 + 1.5f;
		D3DXVECTOR2 delta(dx * sampleOffset, dy * sampleOffset);
		sampleOffsets[i * 2 + 1] = delta;
		sampleOffsets[i * 2 + 2] = -delta;
	}

	for(int i = 0; i < sampleCount; i++)
		sampleWeights[i] /= totalWeights;

	m_fx->setVal("sampleOffsets", (f32*)sampleOffsets, 2*sampleCount*sizeof(f32));
	m_fx->setVal("sampleWeights", (f32*)sampleWeights, sampleCount*sizeof(f32));
}

float Bloom::Gaussian(float n)
{
	return (1.0f / sqrtf(2 * D3DX_PI * r_bloomAmount)) * (float)exp(-(n*n) / (2 * r_bloomAmount * r_bloomAmount));
}

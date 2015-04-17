/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

enum XGINE_API REND_TYPE
{
	REND_DEFERRED,
	REND_FORWARD,
	REND_FORWARD_ONCE,
	REND_FORWARD_TRANS,	
	REND_LIGHTPREPASS
};

enum XGINE_API REND_PASS
{
	REND_GFILL,
	REND_SM,
	REND_SV,
	REND_UNLIT,
	REND_ZFILL,
	REND_SOLID,
	REND_SOLID_SHADOWED,
	REND_TRANS,
	REND_REFLECT,
	REND_LIGHTVOLUMES
};

enum XGINE_API REND_SHADER_TYPE
{
	SHADER_DEFAULT = 0,
	SHADER_CUSTOM
};

enum XGINE_API CULL_TYPE
{
	CULL_NONE,
	CULL_FRUSTUM,
	CULL_OCCLUSION,
	CULL_ALL
};

struct XGINE_API RendSet
{
	REND_TYPE		type;
	REND_PASS		pass;
	ICamera*		camera;
	Light*			light;
	Shader*			shader;
	Material*		material;
	Mat				matWorld;
	Mat				matWorldView;
	Mat				matWorldViewProj;
	RenderTexture*	screenMap;
};

enum XGINE_API VertexDeclaration
{
	VD_DEFAULT = 0,
	VD_DEFAULT_INSTANCED = 1,
	VD_DEFAULT_INSTANCED_PACKED = 2,
	VD_PARTICLE = 3,
	VD_SKYDOME = 4,
	VD_TERRAIN = 5,
	VD_VEGETATION = 6,
	VD_SIZE = 7
};

struct XGINE_API StateManager
{
protected:

	struct State 
	{
		u32 currentValue;
		u32	mustUpdate;
	};

	State	zEnable;
	State	zWriteEnable;
	State	zFunc;

	State	stencilEnable;
	State	stencilRef;
	State	stencilFunc;
	State	stencilFail;
	State	stencilPass;
	State	stencilZFail;

	State	alphaTestEnable;
	State	alphaBlendEnable;
	State	blendOp;
	State	srcBlend;
	State	destBlend;
	State	separateAlphaBlendEnable;
		
	State	antialiasedLineEnable;
	State	clipPlaneEnable;
	State	colorWriteEnable;
	State	cullMode;
	State	depthBias;
	State	fillMode;
	State	fogEnable;
	State	lastPixel;
	State	lighting;
	State	multiSampleAntialias;
	State	scissorTestEnable;
	State	shadeMode;

	u32		anyChanges;

public:

	StateManager() : anyChanges(1) { reset(); };

	inline void setZEnable(u32 value){ if(zEnable.currentValue != value){ zEnable.currentValue = value; zEnable.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ zEnable.mustUpdate = 1; anyChanges = 1; } }
	inline void setZWriteEnable(u32 value){ if(zWriteEnable.currentValue != value){ zWriteEnable.currentValue = value; zWriteEnable.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ zWriteEnable.mustUpdate = 1; anyChanges = 1; } }
	inline void setZFunc(u32 value){ if(zFunc.currentValue != value){ zFunc.currentValue = value; zFunc.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ zFunc.mustUpdate = 1; anyChanges = 1; } }

	inline void setStencilEnable(u32 value){ if(stencilEnable.currentValue != value){ stencilEnable.currentValue = value; stencilEnable.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ stencilEnable.mustUpdate = 1; anyChanges = 1; } }
	inline void setStencilRef(u32 value){ if(stencilRef.currentValue != value){ stencilRef.currentValue = value; stencilRef.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ stencilRef.mustUpdate = 1; anyChanges = 1; } }
	inline void setStencilFunc(u32 value){ if(stencilFunc.currentValue != value){ stencilFunc.currentValue = value; stencilFunc.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ stencilFunc.mustUpdate = 1; anyChanges = 1; } }
	inline void setStencilFail(u32 value){ if(stencilFail.currentValue != value){ stencilFail.currentValue = value; stencilFail.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ stencilFail.mustUpdate = 1; anyChanges = 1; } }
	inline void setStencilPass(u32 value){ if(stencilPass.currentValue != value){ stencilPass.currentValue = value; stencilPass.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ stencilPass.mustUpdate = 1; anyChanges = 1; } }
	inline void setStencilZFail(u32 value){ if(stencilZFail.currentValue != value){ stencilZFail.currentValue = value; stencilZFail.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ stencilZFail.mustUpdate = 1; anyChanges = 1; } }

	inline void setAlphaTestEnable(u32 value){ if(alphaTestEnable.currentValue != value){ alphaTestEnable.currentValue = value; alphaTestEnable.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ alphaTestEnable.mustUpdate = 1; anyChanges = 1; } }
	inline void setAlphaBlendEnable(u32 value){ if(alphaBlendEnable.currentValue != value){ alphaBlendEnable.currentValue = value; alphaBlendEnable.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ alphaBlendEnable.mustUpdate = 1; anyChanges = 1; } }
	inline void setBlendOp(u32 value){ if(blendOp.currentValue != value){ blendOp.currentValue = value; blendOp.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ blendOp.mustUpdate = 1; anyChanges = 1; } }
	inline void setSrcBlend(u32 value){ if(srcBlend.currentValue != value){ srcBlend.currentValue = value; srcBlend.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ srcBlend.mustUpdate = 1; anyChanges = 1; } }
	inline void setDestBlend(u32 value){ if(destBlend.currentValue != value){ destBlend.currentValue = value; destBlend.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ destBlend.mustUpdate = 1; anyChanges = 1; } }
	inline void setSeparateAlphaBlendEnable(u32 value){ if(separateAlphaBlendEnable.currentValue != value){ separateAlphaBlendEnable.currentValue = value; separateAlphaBlendEnable.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ separateAlphaBlendEnable.mustUpdate = 1; anyChanges = 1; } }

	inline void setAntialiasedLineEnable(u32 value){ if(antialiasedLineEnable.currentValue != value){ antialiasedLineEnable.currentValue = value; antialiasedLineEnable.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ antialiasedLineEnable.mustUpdate = 1; anyChanges = 1; } }
	inline void setClipPlaneEnable(u32 value){ if(clipPlaneEnable.currentValue != value){ clipPlaneEnable.currentValue = value; clipPlaneEnable.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ clipPlaneEnable.mustUpdate = 1; anyChanges = 1; } }
	inline void setColorWriteEnable(u32 value){ if(colorWriteEnable.currentValue != value){ colorWriteEnable.currentValue = value; colorWriteEnable.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ colorWriteEnable.mustUpdate = 1; anyChanges = 1; } }
	inline void setCullMode(u32 value){ if(cullMode.currentValue != value){ cullMode.currentValue = value; cullMode.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ cullMode.mustUpdate = 1; anyChanges = 1; } }
	inline void setDepthBias(u32 value){ if(depthBias.currentValue != value){ depthBias.currentValue = value; depthBias.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ depthBias.mustUpdate = 1; anyChanges = 1; } }
	inline void setFillMode(u32 value){ if(fillMode.currentValue != value){ fillMode.currentValue = value; fillMode.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ fillMode.mustUpdate = 1; anyChanges = 1; } }
	inline void setFogEnable(u32 value){ if(fogEnable.currentValue != value){ fogEnable.currentValue = value; fogEnable.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ fogEnable.mustUpdate = 1; anyChanges = 1; } }
	inline void setLastPixel(u32 value){ if(lastPixel.currentValue != value){ lastPixel.currentValue = value; lastPixel.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ lastPixel.mustUpdate = 1; anyChanges = 1; } }
	inline void setLighting(u32 value){ if(lighting.currentValue != value){ lighting.currentValue = value; lighting.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ lighting.mustUpdate = 1; anyChanges = 1; } }
	inline void setMultiSampleAntialias(u32 value){ if(multiSampleAntialias.currentValue != value){ multiSampleAntialias.currentValue = value; multiSampleAntialias.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ multiSampleAntialias.mustUpdate = 1; anyChanges = 1; } }
	inline void setScissorTestEnable(u32 value){ if(scissorTestEnable.currentValue != value){ scissorTestEnable.currentValue = value; scissorTestEnable.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ scissorTestEnable.mustUpdate = 1; anyChanges = 1; } }
	inline void setShadeMode(u32 value){ if(shadeMode.currentValue != value){ shadeMode.currentValue = value; shadeMode.mustUpdate = 1; anyChanges = 1; } else if(!r_optimizeRSChanges){ shadeMode.mustUpdate = 1; anyChanges = 1; } }

	inline void reset()
	{
		anyChanges = 1;
		zEnable.mustUpdate = 1;						zEnable.currentValue = TRUE;
		zWriteEnable.mustUpdate = 1;				zWriteEnable.currentValue = TRUE;
		zFunc.mustUpdate = 1;						zFunc.currentValue = D3DCMP_LESSEQUAL;
		stencilEnable.mustUpdate = 1;				stencilEnable.currentValue = FALSE;
		stencilRef.mustUpdate = 1;					stencilRef.currentValue = 0;
		stencilFunc.mustUpdate = 1;					stencilFunc.currentValue = D3DCMP_ALWAYS;
		stencilFail.mustUpdate = 1; 				stencilFail.currentValue = D3DSTENCILOP_KEEP;
		stencilPass.mustUpdate = 1; 				stencilPass.currentValue = D3DSTENCILOP_KEEP;
		stencilZFail.mustUpdate = 1; 				stencilZFail.currentValue = D3DSTENCILOP_KEEP;
		alphaTestEnable.mustUpdate = 1; 			alphaTestEnable.currentValue = FALSE;
		alphaBlendEnable.mustUpdate = 1; 			alphaBlendEnable.currentValue = FALSE;
		blendOp.mustUpdate = 1; 					blendOp.currentValue = D3DBLENDOP_ADD;
		srcBlend.mustUpdate = 1; 					srcBlend.currentValue = D3DBLEND_ONE;
		destBlend.mustUpdate = 1; 					destBlend.currentValue = D3DBLEND_ONE;
		separateAlphaBlendEnable.mustUpdate = 1;	separateAlphaBlendEnable.currentValue = FALSE;
		antialiasedLineEnable.mustUpdate = 1;		antialiasedLineEnable.currentValue = FALSE;
		clipPlaneEnable.mustUpdate = 1;				clipPlaneEnable.currentValue = FALSE;
		colorWriteEnable.mustUpdate = 1;			colorWriteEnable.currentValue = 0x0000000f;
		cullMode.mustUpdate = 1; 					cullMode.currentValue = D3DCULL_CW;
		depthBias.mustUpdate = 1; 					depthBias.currentValue = 0x00000000;
		fillMode.mustUpdate = 1; 					fillMode.currentValue = D3DFILL_SOLID;
		fogEnable.mustUpdate = 1; 					fogEnable.currentValue = FALSE;
		lastPixel.mustUpdate = 1; 					lastPixel.currentValue = FALSE;
		lighting.mustUpdate = 1; 					lighting.currentValue = FALSE;
		multiSampleAntialias.mustUpdate = 1;		multiSampleAntialias.currentValue = FALSE;
		scissorTestEnable.mustUpdate = 1;			scissorTestEnable.currentValue = FALSE;
		shadeMode.mustUpdate = 1;					shadeMode.currentValue = D3DSHADE_GOURAUD;
	}

	inline u32 commitChanges(LPDIRECT3DDEVICE9 dev)
	{
		if(!anyChanges)return 0;
		static u32 numChanges;
		numChanges = 0;
		if(zEnable.mustUpdate){						dev->SetRenderState(D3DRS_ZENABLE, zEnable.currentValue); numChanges++; }										zEnable.mustUpdate = 0;
		if(zWriteEnable.mustUpdate){				dev->SetRenderState(D3DRS_ZWRITEENABLE, zWriteEnable.currentValue); numChanges++; }								zWriteEnable.mustUpdate = 0;
		if(zFunc.mustUpdate){						dev->SetRenderState(D3DRS_ZFUNC, zFunc.currentValue); numChanges++; }											zFunc.mustUpdate = 0;
		if(stencilEnable.mustUpdate){				dev->SetRenderState(D3DRS_STENCILENABLE, stencilEnable.currentValue); numChanges++; }							stencilEnable.mustUpdate = 0;
		if(stencilRef.mustUpdate){					dev->SetRenderState(D3DRS_STENCILREF, stencilRef.currentValue); numChanges++; }									stencilRef.mustUpdate = 0;
		if(stencilFunc.mustUpdate){					dev->SetRenderState(D3DRS_STENCILFUNC, stencilFunc.currentValue); numChanges++; }								stencilFunc.mustUpdate = 0;
		if(stencilFail.mustUpdate){					dev->SetRenderState(D3DRS_STENCILFAIL, stencilFail.currentValue); numChanges++; }								stencilFail.mustUpdate = 0;
		if(stencilPass.mustUpdate){					dev->SetRenderState(D3DRS_STENCILPASS, stencilPass.currentValue); numChanges++; }								stencilPass.mustUpdate = 0;
		if(stencilZFail.mustUpdate){				dev->SetRenderState(D3DRS_STENCILZFAIL, stencilZFail.currentValue); numChanges++; }								stencilZFail.mustUpdate = 0;
		if(alphaTestEnable.mustUpdate){				dev->SetRenderState(D3DRS_ALPHATESTENABLE, alphaTestEnable.currentValue); numChanges++; }						alphaTestEnable.mustUpdate = 0;
		if(alphaBlendEnable.mustUpdate){			dev->SetRenderState(D3DRS_ALPHABLENDENABLE, alphaBlendEnable.currentValue); numChanges++; }						alphaBlendEnable.mustUpdate = 0;
		if(blendOp.mustUpdate){						dev->SetRenderState(D3DRS_BLENDOP, blendOp.currentValue); numChanges++; }										blendOp.mustUpdate = 0;
		if(srcBlend.mustUpdate){					dev->SetRenderState(D3DRS_SRCBLEND, srcBlend.currentValue); numChanges++; }										srcBlend.mustUpdate = 0;
		if(destBlend.mustUpdate){					dev->SetRenderState(D3DRS_DESTBLEND, destBlend.currentValue); numChanges++; }									destBlend.mustUpdate = 0;
		if(separateAlphaBlendEnable.mustUpdate){	dev->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, separateAlphaBlendEnable.currentValue); numChanges++; }		separateAlphaBlendEnable.mustUpdate = 0;
		if(antialiasedLineEnable.mustUpdate){		dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, antialiasedLineEnable.currentValue); numChanges++; }			antialiasedLineEnable.mustUpdate = 0;
		if(clipPlaneEnable.mustUpdate){				dev->SetRenderState(D3DRS_CLIPPLANEENABLE, clipPlaneEnable.currentValue); numChanges++; }						clipPlaneEnable.mustUpdate = 0;
		if(colorWriteEnable.mustUpdate){			dev->SetRenderState(D3DRS_COLORWRITEENABLE, colorWriteEnable.currentValue); numChanges++; }						colorWriteEnable.mustUpdate = 0;
		if(cullMode.mustUpdate){					dev->SetRenderState(D3DRS_CULLMODE, cullMode.currentValue); numChanges++; }										cullMode.mustUpdate = 0;
		if(depthBias.mustUpdate){					dev->SetRenderState(D3DRS_DEPTHBIAS, depthBias.currentValue); numChanges++; }									depthBias.mustUpdate = 0;
		if(fillMode.mustUpdate){					dev->SetRenderState(D3DRS_FILLMODE, fillMode.currentValue); numChanges++; }										fillMode.mustUpdate = 0;
		if(fogEnable.mustUpdate){					dev->SetRenderState(D3DRS_FOGENABLE, fogEnable.currentValue); numChanges++; }									fogEnable.mustUpdate = 0;
		if(lastPixel.mustUpdate){					dev->SetRenderState(D3DRS_LASTPIXEL, lastPixel.currentValue); numChanges++; }									lastPixel.mustUpdate = 0;
		if(lighting.mustUpdate){					dev->SetRenderState(D3DRS_LIGHTING, lighting.currentValue); numChanges++; }										lighting.mustUpdate = 0;
		if(multiSampleAntialias.mustUpdate){		dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, multiSampleAntialias.currentValue); numChanges++; }				multiSampleAntialias.mustUpdate = 0;
		if(scissorTestEnable.mustUpdate){			dev->SetRenderState(D3DRS_SCISSORTESTENABLE, scissorTestEnable.currentValue); numChanges++; }					scissorTestEnable.mustUpdate = 0;
		if(shadeMode.mustUpdate){					dev->SetRenderState(D3DRS_SHADEMODE, shadeMode.currentValue); numChanges++; }									shadeMode.mustUpdate = 0;
		anyChanges = 0;
		return numChanges;
	}
};

struct XGINE_API Renderer
{
	Renderer();

	u32	 init();
	void close();

	void clear(i32 clearScreen = 1, i32 clearDepth = 1, i32 clearStencil = 1, u32 color = 0x000000);
	bool begin();
	void end(u32 rendStats = 1, u32 rendGui = 1);
	void stats();
		
	void beginSceneRender();
	void setCamera(ICamera* cam);
	void setLight(Light* light);
	void setupLight(Light* light);
	void setShader(Shader* shader);
	void setDefaultShader() { setShader(m_defaultShader); }
	void setWorld(Mat* world);
	void setMaterial(Material* mat);
	void setTexture(u32 sampler, Texture* textureRes);
	void setTexture(u32 sampler, IDirect3DBaseTexture9* texture);
	void commitChanges();
	
	void cull(RenderList *in, CULL_TYPE cullType = CULL_ALL);
	void render(RenderList *rl);
	void renderReflection(RenderList *rl);
	void drawBoundingBox(BoundingBox* boundingBox, Vec4 color);

	RenderTexture* getBackBuffer(u32 blur);
	void copyBackBuffer(LPDIRECT3DTEXTURE9 dest);
	void setRenderTarget(LPDIRECT3DSURFACE9 rt);

	void addTxt(const c8 *str, ...);
	void _addTxt(const c8 *str, ...);
	void addTxtF(const c8 *str, ...);
	void takeScreenshot();
	void hideStats(i32 hide);

	inline RendSet&				get()									{ return m_rend; }

		   void					computeFps();
	inline u32					getFps()								{ return m_fps; }
	inline f32					getFrameTime()							{ return m_frameTime; }

	u32 renderListRaw(EntityList<Surface> *in);
	u32 renderListForward(EntityList<Surface> *in, Light* light);
	u32 renderListDeferred(EntityList<Surface> *in);
	u32 renderListLightPrePass(EntityList<Surface> *in);

	u32 renderListRaw(EntityList<Terrain> *in);
	u32 renderListForward(EntityList<Terrain> *in, Light* light);
	u32 renderListDeferred(EntityList<Terrain> *in);
	u32 renderListLightPrePass(EntityList<Terrain> *in);

	u32 renderListRaw(EntityList<IEntity> *in);
	u32 renderListForward(EntityList<IEntity> *in, Light* light);
	u32 renderListDeferred(EntityList<IEntity> *in);
	u32 renderListLightPrePass(EntityList<IEntity> *in);

	Renderer2D*					r2d;
	OcclusionCulling*			oc;
	ShadowMapping*				sm;
	VolumetricLightScattering*	vls;

	inline RenderTexture   *getReflectionTexture() { return m_reflection; }

	void addRenderedCounts(u32 verts, u32 primitivs, u32 dip = 0) { _renderedVerticesCount += verts; _renderedPrimitivesCount += primitivs; _renderedDrawIndexedPrimitive += dip; }
	u32 _renderedPrimitivesCount;
	u32 _renderedVerticesCount;
	u32 _renderedDrawIndexedPrimitive;
	u32	_rendererShaderChanges;
	u32 _rendererRSChanges;
	u32	_rendererTextureChanges;


	Shader* beginGuiShader();
	void	endGuiShader();
	
	MRT* getMRT() { return m_mrt; }

	Shader*					m_defaultShader;

	void setAmbientColor(f32 c) { m_ambientColor = c; }
	f32  getAmbientColor() { return m_ambientColor; }

	void setSunBright(f32 c) { m_sunBright = c; }
	f32  getSunBright() { return m_sunBright; }

	void setReflectionHeight(f32 h) { m_reflectionHeight = h; }	

	StateManager			state;

protected:
	void					renderLightPrePass(RenderList *in);
	void					renderLightPrePassLighting(RenderList *in);
	void					renderDeferred(RenderList *in);
	void					renderDeferredLighting(RenderList *in);
	void					renderForward(RenderList *in);
	void					renderForwardTrans(RenderList *in);

	//DATA/PARAMS
	u32						m_initialized;
	//Device
	LPDIRECT3DDEVICE9		m_dev;
	//RendSet
	RendSet					m_rend;
	IDirect3DBaseTexture9*	m_textures[8];
	//Utilities
	IDirect3DQuery9*		pQuery;
	string					m_outstr;
	string					m_outstr2;
	string					m_fadestr[32];
	f32						m_fadetime[32];
	u32						m_fadestrNum;
	LPDIRECT3DSURFACE9		m_screenRT;
	//FPS
	u32						m_fps;
	f32						m_fpsMin;
	f32						m_fpsMax;
	f32						m_frameTime;
	f32						m_frameTimeMin;
	f32						m_frameTimeMax;
	//Rendering
	Shader*					m_zfillShader;
	//Deferred lighting
	MRT*					m_mrt;
	Shader*					m_forwardShader;
	Shader*					m_deferredShaderFill;
	Shader*					m_deferredShaderLight;
	Shader*					m_deferredLightVolume;
	Shader*					m_guiShader;
	Shader*					m_waterShader;
	//Light Pre-Pass
	RenderTexture*			m_depthNormals;
	RenderTexture*			m_lightingLPP;

	RenderTexture*			m_backBuffer;
	RenderTexture*			m_backBufferBlured;
	RenderTexture*			m_backBufferBluredTemp;
	RenderTexture*			m_accumulationRT_HDR;
	void					prepareBackBuffers();

	HDRToneMap*				m_hdrToneMap;
	u32						m_hdrRendering;

	f32						m_reflectionHeight;

friend struct HQWater;

	u32						enableHDR();
	void					disableHDR();

	DepthOfField*			m_depthOfField;
	HeatHaze*				m_heatHaze;

	f32						m_ambientColor;
	f32						m_sunBright;

public:
	RenderTexture	*screenMap;
	RenderTexture	*blurMap[2];
	RenderTexture	*blurMapVSM[2];
	GuassianBlur	*guassianBlur;
	RenderTexture   *m_reflection;
	RenderTexture   *m_reflectionBlur;

	Texture			*rnm;
	RenderTexture	*ssaoMap;

	//Vertex Declarations
	LPDIRECT3DVERTEXDECLARATION9	m_vd[VD_SIZE];
};
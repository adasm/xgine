#pragma once
#include "XGine.h"

struct XGINE_API MaterialData
{
	string name;
	string shader;
	string tech;
	string techPoint, techSpot, techDir;
	vector< pair< string, UniversalValue > > params;
	vector< pair< string, string> > textures;
};

struct XGINE_API UMaterialFileRes : IRes
{
protected:
	friend ResMgr;

	~UMaterialFileRes();
	UMaterialFileRes();

	u32		load();
	void	release();

public:
	u32				apply(Light *light);

	Shader*			shader;
	D3DXHANDLE 		techPoint;
	D3DXHANDLE 		techSpot;
	D3DXHANDLE 		techDir;
	D3DXHANDLE		paramsBlock;
	D3DXHANDLE		*texHandles;
	Texture			**texSources;
	u32				numTextures;
	MaterialData	matData;
};

struct XGINE_API UMaterialFile : IMaterial
{
	UMaterialFile(string fileName);
	~UMaterialFile();
	
	u32		valid();

	u32		apply(Mat* world, Light *light);
	u32		apply(Mat* world, Light* light, RenderTexture* shadowMap) { return 0; }

	Shader*	getShader();
	u32		numPasses();
	void	beginPass(u32 i);
	void	endPass();

protected:
	UMaterialFileRes*	m_matRes;
};
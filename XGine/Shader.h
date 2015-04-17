#pragma once
#include "XGine.h"

struct ShaderMgr;
struct Multishader;

struct XGINE_API Shader
{
protected:
	friend ShaderMgr;
	friend Multishader;

	Shader();
	~Shader();

public:
	inline D3DXHANDLE		getParam(c8 *name);
	inline D3DXHANDLE		getTech(c8* name);
	inline Shader*			setTech(c8* name);
	inline Shader*			setTech(D3DXHANDLE handle);

	inline void begin();
	inline void end();

	inline void setVal(D3DXHANDLE h, void* v, u32 c);
	inline void setBool(D3DXHANDLE h, bool v);
	inline void setVec2(D3DXHANDLE h, const Vec2* v);
	inline void setVec3(D3DXHANDLE h, const Vec3* v);
	inline void setVec4(D3DXHANDLE h, const Vec4* v);
	inline void setF32(D3DXHANDLE h, const f32 f);
	inline void setTex(D3DXHANDLE h, Texture* tex);
	inline void setTex(D3DXHANDLE h, CubeTexture* tex);
	inline void setTex(D3DXHANDLE h, VolumeTexture* tex);	
	inline void setTex(D3DXHANDLE h, const LPDIRECT3DTEXTURE9 tex);
	inline void setMat(D3DXHANDLE h, const Mat* mat);
	inline void commitChanges();

	//inline u32 numPasses() { return passes; }

	inline void			setFileName(std::string name) { fileName = name; }
	inline std::string	getFileName() { return fileName; }
	

	LPD3DXEFFECT	lpEffect;
	u32				passes;
	D3DXHANDLE		currTechProcStr;
	u32				procBegin;
	std::string		fileName;
};
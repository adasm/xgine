#pragma once
#include "XGine.h"

class ShaderInclude : public ID3DXInclude
{
public:
	STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID * ppData, UINT * pBytes);
	STDMETHOD(Close)(LPCVOID pData);
};

struct XGINE_API ShaderMgr
{
	ShaderMgr() : numShaders(0) { }

	u32				init();
	void			close();
	
	u32				reload();
	Shader*			load(string fileName);
	Multishader*	load(string fileName, string cacheFileNameMask, u16 macroCount, const c8* macroNames[], const u16 macroBits[]);

	u32				numShaders;

protected:
	Shader* readFromCache(string fileName);
	Shader* createShaderCache(string fileName);

	typedef map<std::string, Shader*>		tShaderContainer;
	typedef tShaderContainer::iterator		tShaderContainerIt;
	typedef map<std::string, Multishader*>	tMultishaderContainer;
	typedef tMultishaderContainer::iterator tMultishaderContainerIt;

	tShaderContainer			m_shaders;
	tMultishaderContainer		m_multishaders;
};
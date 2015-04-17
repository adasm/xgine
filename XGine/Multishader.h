#pragma once
#include "XGine.h"
struct ShaderMgr;

struct XGINE_API Multishader
{
protected:
	friend ShaderMgr;

	Multishader(const string &sourceFileName, const string &cacheFileNameMask,
				u16 macroCount, const c8 *macroNames[], const u16 macroBits[]);
	~Multishader();

public:
	u32		getShaderCount();
	Shader* getShader(u16 macros[]);

protected:
	typedef stdext::hash_map<u16, Shader*>	ShaderMap;
	typedef ShaderMap::iterator				ShaderMapIt;

	std::string					m_sourceFileName;
	std::string					m_cacheFileNameMask;
	ShaderMap					m_shaders;
	u16							m_macroCount;
	std::vector<std::string>	m_macroNames;
	std::vector<u16>			m_macroBits;
	
	void	freeEffects();
	void	comouteHash(u16 &hash, u16 macros[]);
	Shader* readFromCache(const c8 *cacheFileName);
	Shader* createShaderCache(const c8 *cacheFileName, u16 macros[]);
};


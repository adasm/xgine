#include "Multishader.h"
#include <sys/types.h> // dla _stat
#include <sys/stat.h> // dla _stat

Multishader::Multishader(	const string &sourceFileName, const string &cacheFileNameMask,
							u16 macroCount, const c8 *macroNames[], const u16 macroBits[])
	: m_sourceFileName(sourceFileName),
	m_cacheFileNameMask(cacheFileNameMask), m_macroCount(macroCount), m_macroNames(macroCount), m_macroBits(macroCount)
{
	for (unsigned i = 0; i < m_macroCount; i++)
	{
		m_macroNames[i] = macroNames[i];
		m_macroBits[i] = macroBits[i];
	}
}

Multishader::~Multishader()
{
	freeEffects();
}


unsigned Multishader::getShaderCount()
{
	return m_shaders.size();
}

void Multishader::comouteHash(u16 &hash, u16 macros[])
{
	for (size_t i = 0; i < m_macroCount; i++)
		hash |= macros[i] << m_macroBits[i];
}

Shader* Multishader::readFromCache(const c8 *cacheFileName)
{
	ID3DXEffect		*lpEffect = 0;
	LPD3DXBUFFER	 lpErrBuf = 0;

	//Load shader cache
	Buffer buffer;
	if(!gEngine.kernel->fs->load(cacheFileName, buffer))
	{
		return 0;
	}

	//Create shader	
	HRESULT hr = D3DXCreateEffect(gEngine.device->getDev(),buffer.data,buffer.size,NULL,NULL,D3DXFX_DONOTSAVESTATE,NULL, &lpEffect, &lpErrBuf);
	if(FAILED(hr))
	{
		gEngine.kernel->mem->freeBuff(buffer);
		if(lpErrBuf)
		{
			LPVOID pCompilErrors = lpErrBuf->GetBufferPointer();
			gEngine.kernel->log->prn(LT_FATAL, "Multishader", (char*)pCompilErrors);
			lpErrBuf->Release();
		}
		return 0;
	}

	gEngine.kernel->mem->freeBuff(buffer);
	DXRELEASE(lpErrBuf);

	Shader *shader = new Shader();
	shader->lpEffect = lpEffect;
	return shader;
}

Shader* Multishader::createShaderCache(const c8 *cacheFileName, u16 macros[])
{
	Buffer buffer;

	string sourceFileName	= g_path_shaders + m_sourceFileName;

	//Load shader source
	if(!gEngine.kernel->fs->load(sourceFileName, buffer))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Multishader", "Couldn't load shader '%s'. Shader source loading failed.", cacheFileName);
		return 0;
	}

	//Create macros
	std::vector<D3DXMACRO>		D3dMacros;		D3dMacros.resize(m_macroCount+1);
	std::vector<std::string>	MacroValues;	MacroValues.resize(m_macroCount);
	
	char str[256]; u16 mi; 
	for (mi = 0; mi < m_macroCount; mi++)
	{
		sprintf(str, "%u", macros[mi]); MacroValues[mi] = str;
		D3dMacros[mi].Name = m_macroNames[mi].c_str();
		D3dMacros[mi].Definition = MacroValues[mi].c_str();
	}
	D3dMacros[mi].Name = NULL;
	D3dMacros[mi].Definition = NULL;

	//Create compiler
	ID3DXEffectCompiler *Compiler = NULL;
	{
		ID3DXBuffer *ErrBuf = NULL;

		u32 flags = 0;

#ifdef _DEBUG
		//flags |= D3DXSHADER_DEBUG;
		//flags |= D3DXSHADER_NO_PRESHADER;
		//flags |= D3DXSHADER_SKIPOPTIMIZATION;

		flags |= D3DXSHADER_OPTIMIZATION_LEVEL3;
#else
		flags |= D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif

		HRESULT hr = D3DXCreateEffectCompiler(buffer.data,buffer.size,&D3dMacros[0],NULL, flags, &Compiler, &ErrBuf);
		if(FAILED(hr))
		{
			if(ErrBuf)
			{
				LPVOID pCompilErrors = ErrBuf->GetBufferPointer();
				gEngine.kernel->log->prn(LT_FATAL, "Multishader", (char*)pCompilErrors);
				ErrBuf->Release();
			}
			return 0;
		}
	}

	//Compile shader
	ID3DXBuffer *EffectBuf = NULL;
	{
		ID3DXBuffer *ErrBuf = NULL;
		HRESULT hr = Compiler->CompileEffect(0, &EffectBuf, &ErrBuf);
		if(FAILED(hr))
		{
			if(ErrBuf)
			{
				LPVOID pCompilErrors = ErrBuf->GetBufferPointer();
				gEngine.kernel->log->prn(LT_FATAL, "Multishader", (char*)pCompilErrors);
				ErrBuf->Release();
			}
			return 0;
		}
	}
	
	//Save cache file
	if(g_allowShaderCache)
	{
		/*Buffer buffToFile;
		buffToFile.data = (c8*)EffectBuf->GetBufferPointer();
		buffToFile.size = EffectBuf->GetBufferSize();
		gEngine.kernel->fs->writeBuffToFile(cacheFileName, buffToFile);*/

		FSPakFileInfo file;
		file.compType = g_shaderCacheCompression;
		file.packedFileName = file.fileName = cacheFileName;
		file.buffer = new Buffer();
		file.buffer->data = (c8*)EffectBuf->GetBufferPointer();
		file.buffer->size = EffectBuf->GetBufferSize();
		file.mdate = gEngine.kernel->fs->getModDate(sourceFileName);
		gEngine.kernel->fs->addToArchive(g_shaderCacheName, file, "XPAK");
		delete(file.buffer);
	}

	//Create shader
	ID3DXEffect *lpEffect;
	{
		ID3DXBuffer *ErrBuf;
		HRESULT hr = D3DXCreateEffect(gEngine.device->getDev(), EffectBuf->GetBufferPointer(),EffectBuf->GetBufferSize(), NULL, NULL,  0,  NULL,  &lpEffect, &ErrBuf); 
		if (FAILED(hr))
		{
			if(ErrBuf)
			{
				LPVOID pCompilErrors = ErrBuf->GetBufferPointer();
				gEngine.kernel->log->prn(LT_FATAL, "Multishader", (char*)pCompilErrors);
				ErrBuf->Release();
			}
			return 0;
		}
	}

	gEngine.kernel->mem->freeBuff(buffer);

	DXRELEASE(EffectBuf);
	DXRELEASE(Compiler);

	Shader *shader = new Shader();
	shader->lpEffect = lpEffect;
	shader->setFileName(m_sourceFileName);
	return shader;
}

Shader* Multishader::getShader(u16 macros[])
{
	string g_path_effects = "shaders/";

	u16 Hash = 0;
	comouteHash(Hash, macros);

	ShaderMapIt sit = m_shaders.find(Hash);
	if(sit != m_shaders.end())
		return sit->second;

	c8 cacheFileNameStr[MAX_PATH];
	sprintf(cacheFileNameStr, m_cacheFileNameMask.c_str(), Hash);
	string cacheFileName = g_path_effects+cacheFileNameStr;
	string sourceFileName =g_path_effects+m_sourceFileName;

	if(gEngine.kernel->fs->exists(sourceFileName) && gEngine.kernel->fs->exists(cacheFileName) && g_allowShaderCache)
	{
		if(gEngine.kernel->fs->getModDate(sourceFileName) <= gEngine.kernel->fs->getModDate(cacheFileName))
		{
			Shader * shader = readFromCache(cacheFileName.c_str());
			if(!shader)
			{
					gEngine.kernel->log->prnEx(LT_FATAL, "Multishader", "Couldn't load shader '%s'. Reading from cache failed.", cacheFileName.c_str());
					return 0;
			}
			m_shaders.insert(std::make_pair(Hash, shader)).first;
			gEngine.kernel->log->prnEx(LT_SUCCESS, "Multishader", "Loaded shader '%s' ('%s')", cacheFileName.c_str(), sourceFileName.c_str());
			return shader;
		}
	}
	else if(gEngine.kernel->fs->exists(cacheFileName) && g_allowShaderCache)
	{
		Shader * shader = readFromCache(cacheFileName.c_str());
		if(!shader)
		{
				gEngine.kernel->log->prnEx(LT_FATAL, "Multishader", "Couldn't load shader '%s'. Source shader is missing. Reading from cache failed.", cacheFileName.c_str());
				return 0;
		}
		m_shaders.insert(std::make_pair(Hash, shader)).first;
		gEngine.kernel->log->prnEx(LT_SUCCESS, "Multishader", "Loaded shader '%s' ('%s')", cacheFileName.c_str(), sourceFileName.c_str());
		return shader;
	}

	Shader * shader = createShaderCache(cacheFileName.c_str(), macros);
	if(!shader)
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Multishader", "Couldn't load shader '%s'. Reading from cache failed.", cacheFileName.c_str());
		return 0;
	}
	m_shaders.insert(std::make_pair(Hash, shader)).first;
	gEngine.kernel->log->prnEx(LT_SUCCESS, "Multishader", "Loaded shader '%s' ('%s')", cacheFileName.c_str(), sourceFileName.c_str());
	gEngine.shaderMgr->numShaders++;
	return shader;
}

void Multishader::freeEffects()
{
	for (ShaderMap::reverse_iterator srit = m_shaders.rbegin(); srit != m_shaders.rend(); ++srit)
		if (srit->second->lpEffect != NULL)
			srit->second->lpEffect->Release();
	m_shaders.clear();
}
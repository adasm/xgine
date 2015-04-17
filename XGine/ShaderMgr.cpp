#include "ShaderMgr.h"
#include <sys/types.h> // dla _stat
#include <sys/stat.h> // dla _stat

u32 ShaderMgr::init()
{
	gEngine.kernel->fs->addSrc(g_shaderCacheName, "XPAK");
	return 1;
}

void ShaderMgr::close()
{
	for(tShaderContainerIt it = m_shaders.begin(); it != m_shaders.end(); ++it)
		delete(it->second);
	m_shaders.clear();
	for(tMultishaderContainerIt it = m_multishaders.begin(); it != m_multishaders.end(); ++it)
		delete(it->second);
	m_multishaders.clear();
}

u32	ShaderMgr::reload()
{
	return 0;

	/*for(tShaderContainerIt it = m_shaders.begin(); it != m_shaders.end(); ++it)
	{		
		Shader* sh = it->second;
		string fileName = sh->getFileName();
		Shader *shader = load(fileName);
		if(!shader)
		{
			gEngine.kernel->log->prnEx("Couldn't reload shader '%s'", fileName.c_str());
		}
		else
		{
			DXRELEASE(it->second->lpEffect);
			LPD3DXEFFECT lpEffect = shader->lpEffect;
			delete(shader);
			it->second->lpEffect = lpEffect;
		}
	}
	return 1;*/
}

Shader* ShaderMgr::load(string fileName)
{
	u32 tstart = timeGetTime();

	tShaderContainerIt it = m_shaders.find(fileName);
	if(it != m_shaders.end())
		return it->second;

	string sourceFileName = g_path_shaders + fileName;
	string cacheFileName = g_path_shaders + getWithoutExt(fileName)+".fxo";
	
	if(gEngine.kernel->fs->exists(sourceFileName) && gEngine.kernel->fs->exists(cacheFileName) && g_allowShaderCache)
	{
		u64 date1 = gEngine.kernel->fs->getModDate(sourceFileName);
		u64 date2 = gEngine.kernel->fs->getModDate(cacheFileName);
		if(date1 <= date2)
		{
			Shader *shader = readFromCache(fileName);
			if(!shader)
			{
					gEngine.kernel->log->prnEx(LT_FATAL, "ShaderMgr", "Couldn't load shader '%s'. Reading from cache failed.", cacheFileName.c_str());
					return 0;
			}
			shader->setFileName(fileName);
			m_shaders.insert(make_pair(fileName, shader));
			gEngine.kernel->log->prnEx(LT_SUCCESS, "ShaderMgr", "Loaded shader '%s' (from cache '%s') in %ums", fileName.c_str(), cacheFileName.c_str(), (u32)timeGetTime()-tstart);
			numShaders++;
			return shader;
		}
	}
	else if(gEngine.kernel->fs->exists(cacheFileName) && g_allowShaderCache)
	{
		Shader *shader = readFromCache(fileName);
		if(!shader)
		{
			gEngine.kernel->log->prnEx(LT_FATAL, "ShaderMgr", "Couldn't load shader '%s'. Source shader is missing.  Reading from cache failed.", cacheFileName.c_str());
			return 0;
		}
		shader->setFileName(fileName);
		m_shaders.insert(make_pair(fileName, shader));
		gEngine.kernel->log->prnEx(LT_SUCCESS, "ShaderMgr", "Loaded shader '%s' (from cache '%s') in %ums", fileName.c_str(), cacheFileName.c_str(), (u32)timeGetTime()-tstart);
		numShaders++;
		return shader;
	}

	Shader *shader = createShaderCache(fileName);
	if(!shader)
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "ShaderMgr", "Couldn't load shader '%s'. Reading from cache failed.", cacheFileName.c_str());
		return 0;
	}
	
	shader->setFileName(fileName);
	m_shaders.insert(make_pair(fileName, shader));
	gEngine.kernel->log->prnEx(LT_SUCCESS, "ShaderMgr", "Loaded shader '%s' in %ums", fileName.c_str(), (u32)timeGetTime()-tstart);
	numShaders++;
	return shader;
}

HRESULT ShaderInclude::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID * ppData, UINT * pBytes)
{
	Buffer buffer;
	string sourceFileName	= g_path_shaders + pFileName;
	if(!gEngine.kernel->fs->load(sourceFileName, buffer))return 1;
	*ppData = buffer.data; buffer.data = 0;
	*pBytes = buffer.size; buffer.size = 0;
	return 0;
}

HRESULT ShaderInclude::Close(LPCVOID pData)
{
	Buffer buffer;
	buffer.data = (c8*)pData; buffer.size = sizeof(pData);
	gEngine.kernel->mem->freeBuff(buffer);
	return 0;
}

Shader* ShaderMgr::createShaderCache(string fileName)
{
	string sourceFileName	= g_path_shaders + fileName;
	string cacheFileName	= g_path_shaders + getWithoutExt(fileName)+".fxo";

	Buffer buffer;
	if(!gEngine.kernel->fs->load(sourceFileName, buffer))return 0;

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

		HRESULT hr = D3DXCreateEffectCompiler(buffer.data,buffer.size, 0, new ShaderInclude(), flags, &Compiler, &ErrBuf);
		if(FAILED(hr))
		{
			if(ErrBuf)
			{
				LPVOID pCompilErrors = ErrBuf->GetBufferPointer();
				gEngine.kernel->log->prn(LT_FATAL, "ShaderMgr", (char*)pCompilErrors);
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
				gEngine.kernel->log->prn(LT_FATAL, "ShaderMgr", (char*)pCompilErrors);
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
				gEngine.kernel->log->prn(LT_FATAL, "ShaderMgr", (char*)pCompilErrors);
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
	shader->setFileName(fileName);
	return shader;
}

Shader* ShaderMgr::readFromCache(string fileName)
{
	ID3DXEffect		*lpEffect = 0;
	LPD3DXBUFFER	 lpErrBuf = 0;

	string sourceFilename	= g_path_shaders + fileName;
	string cacheFileName	= g_path_shaders + getWithoutExt(fileName)+".fxo";

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
			gEngine.kernel->log->prn(LT_FATAL, "ShaderMgr", (char*)pCompilErrors);
			lpErrBuf->Release();
		}
		return 0;
	}
	gEngine.kernel->mem->freeBuff(buffer);
	DXRELEASE(lpErrBuf);

	Shader *shader = new Shader();
	shader->lpEffect = lpEffect;
	shader->setFileName(fileName);
	return shader;
}


Multishader* ShaderMgr::load(string fileName, string cacheFileNameMask, u16 macroCount, const c8* macroNames[], const u16 macroBits[])
{
	u32 tstart = timeGetTime();

	tMultishaderContainerIt it = m_multishaders.find(fileName);
	if(it != m_multishaders.end())
		return it->second;

	Multishader * multishader = new Multishader(fileName, cacheFileNameMask, macroCount, macroNames, macroBits);
	m_multishaders.insert(make_pair(fileName, multishader));
	gEngine.kernel->log->prnEx(LT_SUCCESS, "ShaderMgr", "Loaded multishader '%s' in %ums", fileName.c_str(), (u32)timeGetTime()-tstart);
	return multishader;
}
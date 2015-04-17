#include "UMaterial.h"

UMaterialFileRes::UMaterialFileRes()
{
	shader = 0;
	techPoint = 0;
	techSpot = 0;
	techDir = 0;
	paramsBlock = 0;
	texSources = 0;
	texHandles = 0;
	numTextures = 0;
	matData.name = "";
	matData.shader = "";
	matData.tech = "";
}


UMaterialFileRes::~UMaterialFileRes()
{
	release();
}

void UMaterialFileRes::release()
{
	shader = 0;
	techPoint = 0;
	techSpot = 0;
	techDir = 0;
	paramsBlock = 0;
	for(u32 i = 0; i < numTextures; i++)
		gEngine.resMgr->release(texSources[i]);
	if(texSources) delete[]texSources; texSources = 0;
	if(texHandles) delete[]texHandles; texHandles = 0;
	numTextures = 0;
	matData.name = "";
	matData.shader = "";
	matData.tech = "";
	matData.params.clear();
	matData.textures.clear();
}

u32	UMaterialFileRes::load()
{
	release();

	Buffer buff;
	if(!gEngine.kernel->fs->load("materials/" + strFilename, buff))return 0;
	Tokenizer tokenizer(string(buff.data, buff.size));
	gEngine.kernel->mem->freeBuff(buff);
	tokenizer.clear(";");
//READ MATERIAL DATA
	UniversalValue uvalue;
	string token, name, value;
	if(!tokenizer.nextToken(&token) || strcmpi(token.c_str(), "mat_2_0")){ gEngine.kernel->log->prnEx("Expected mat_2_0 signature in '%s'", strFilename.c_str()); return 0; }
	if(!tokenizer.nextToken(&matData.name)){ gEngine.kernel->log->prnEx("Expected material name in '%s'", strFilename.c_str()); return 0; }
	if(!tokenizer.nextToken(&token) || strcmpi(token.c_str(), "shader") || !tokenizer.nextToken(&matData.shader))
		{ gEngine.kernel->log->prnEx("Expected material shader name in '%s'", strFilename.c_str()); return 0; }
	if(!tokenizer.nextToken(&token) || strcmpi(token.c_str(), "tech") || !tokenizer.nextToken(&matData.tech))
		{ gEngine.kernel->log->prnEx("Expected material techn name in '%s'", strFilename.c_str()); return 0; }
	matData.techPoint	= matData.tech	+ "_Point";
	matData.techSpot	= matData.tech	+ "_Spot";
	matData.techDir		= matData.tech	+ "_Dir";
	while(1)
	{
		if(!tokenizer.nextToken(&token)){ gEngine.kernel->log->prnEx("Expected 'end' token after all parameters in '%s'", strFilename.c_str()); return 0; }

			 if(strcmpi(token.c_str(), "end") == 0)break;
		else if(strcmpi(token.c_str(), "tex") == 0){
			if(!tokenizer.nextToken(&name) || !tokenizer.nextToken(&value))
				{ gEngine.kernel->log->prnEx("Expected texture name and source after token 'tex' in '%s'", strFilename.c_str()); return 0; }
			matData.textures.push_back( make_pair( name, value ) );
		}
		else if(strcmpi(token.c_str(), "bool") == 0){
			if(!tokenizer.nextToken(&name) || !tokenizer.nextToken(&value))
				{ gEngine.kernel->log->prnEx("Expected value name and value after token 'bool' in '%s'", strFilename.c_str()); return 0; }
			uvalue.type = UTYPE_BOOL;
			std::stringstream conv; conv.unsetf(std::ios::skipws); int v;
			conv << value;	conv >> v;
			uvalue.b = (bool)v;
			matData.params.push_back( make_pair( name, uvalue ) );
		}
		else if(strcmpi(token.c_str(), "float") == 0){
			if(!tokenizer.nextToken(&name) || !tokenizer.nextToken(&value))
				{ gEngine.kernel->log->prnEx("Expected value name and value after token 'float' in '%s'", strFilename.c_str()); return 0; }
			uvalue.type = UTYPE_FLOAT;
			std::stringstream conv; conv.unsetf(std::ios::skipws);
			conv << value;	conv >> uvalue.f;
			matData.params.push_back( make_pair( name, uvalue ) );
		}
	}
//LOAD MATERIAL
//shader
	shader = gEngine.shaderMgr->load(matData.shader);
	if(!shader){ return 0; }
//techniques
	techPoint	= shader->getTechHandle((char*)matData.techPoint.c_str());
	techSpot	= shader->getTechHandle((char*)matData.techSpot.c_str());
	techDir		= shader->getTechHandle((char*)matData.techDir.c_str());
	if(!techPoint)	{ gEngine.kernel->log->prnEx("Missing tech '%s' in shader '%s'.", matData.techPoint.c_str(), matData.shader.c_str()); }
	if(!techSpot)	{ gEngine.kernel->log->prnEx("Missing tech '%s' in shader '%s'.", matData.techSpot.c_str(), matData.shader.c_str()); }
	if(!techDir)	{ gEngine.kernel->log->prnEx("Missing tech '%s' in shader '%s'.", matData.techDir.c_str(), matData.shader.c_str()); }
//params
	if(FAILED(shader->lpEffect->BeginParameterBlock()))
	{ gEngine.kernel->log->prn("Failed on shader->lpEffect->BeginParameterBlock()."); return 0; }
	for(u32 i = 0; i < matData.params.size(); i++)
	{
		 if(matData.params[i].second.type == UTYPE_BOOL)
		 {
			  if(FAILED( shader->lpEffect->SetBool(matData.params[i].first.c_str(), matData.params[i].second.b) ) )
			  { gEngine.kernel->log->prnEx("Failed on shader->lpEffect->SetBool('%s', %u).", matData.params[i].first.c_str(), (u32)matData.params[i].second.b); return 0; }
		 }
		 else if(matData.params[i].second.type == UTYPE_FLOAT)
		 {
			  if(FAILED( shader->lpEffect->SetFloat(matData.params[i].first.c_str(), matData.params[i].second.f) ) )
			  { gEngine.kernel->log->prnEx("Failed on shader->lpEffect->SetFloat('%s', %f).", matData.params[i].first.c_str(), matData.params[i].second.f); return 0; }
		 }
	}
	paramsBlock = shader->lpEffect->EndParameterBlock();
	if(!paramsBlock) { gEngine.kernel->log->prn("Failed on shader->lpEffect->EndParameterBlock()."); return 0; }
//textures
	numTextures = matData.textures.size();
	texHandles = new D3DXHANDLE[ numTextures ];
	texSources = new Texture*  [ numTextures ];
	for(u32 i = 0; i < matData.textures.size(); i++)
	{
		texHandles[i] = shader->lpEffect->GetParameterByName(0, (char*) matData.textures[i].first.c_str());
		if(!texHandles[i]){ gEngine.kernel->log->prn("Failed on shader->lpEffect->GetParameterByName(.,.)."); return 0; }
		if(matData.textures[i].second.size() == 0)
			texSources[i] = 0;
		else
			texSources[i] = gEngine.resMgr->load<Texture>(matData.textures[i].second);
	}
	return 1;
}

u32 UMaterialFileRes::apply(Light *light)
{
	if(!shader || !techPoint || !paramsBlock)return 0;
	shader->setTech(techPoint);
	shader->lpEffect->ApplyParameterBlock(paramsBlock);
	gEngine.renderer->setShader(shader);
	for(u32 i = 0; i < numTextures; i++)
		shader->lpEffect->SetTexture(texHandles[i], (texSources[i])?texSources[i]->lpTexture:0);
	shader->commitChanges();

	if(light->getType() == LIGHT_POINT)
	{
		shader->setVector("g_lightColor",		light->getColor());
		shader->setVector("g_lightPos",			light->getPosV4());
		shader->setFloat("g_lightInvRange",		light->getInvRange());
		shader->commitChanges();
	}

	return 1;
}

////////////////////////////////////////////////////////////////////////

UMaterialFile::UMaterialFile(string fileName)
	: m_matRes(0)
{
	m_matRes = gEngine.resMgr->load<UMaterialFileRes>(fileName);
}

UMaterialFile::~UMaterialFile()
{
	gEngine.resMgr->release(m_matRes);
}

u32 UMaterialFile::valid()					
{ 
	return m_matRes != 0; 
}

u32 UMaterialFile::apply(Mat* world, Light *light)
{
	if(m_matRes)
		return m_matRes->apply(light);
	gEngine.renderer->setWorld(world);
	return 0;
}

Shader*	UMaterialFile::getShader()
{ 
	return 0; //m_matRes->shader; 
}

u32 UMaterialFile::numPasses()
{ 
	return m_matRes->shader->currTechPasses; 
}

void UMaterialFile::beginPass(u32 i)
{ 
	m_matRes->shader->beginPass(i); 
}

void UMaterialFile::endPass()
{ 
	m_matRes->shader->endPass(); 
}
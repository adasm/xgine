#include "Material.h"

Material::Material()
	: m_colorMap(0), m_normalMap(0), m_albedoMap(0), m_cubeMap(0), m_useAlphaTest(0)
{
	m_type					= MT_DEFAULT;
	m_textureRepeat			= 1.0f;
	m_textureAlbedoRepeat	= 1.0f;
	m_specularExponent		= 40.0f;
	m_diffuseColor			= Vec4(1,1,1,1);
	m_useNormalMap			= 0;
	m_useSpecular			= 0;
	m_useParallax			= DT_BUMP_MAPPING;
	m_useAlbedoMap			= 0;
	m_useCubeMap			= 0;
	m_useDiffuseColor		= 0;
	m_animationSpeed		= 1;

	m_splatting				= 0;
	m_indexMap			   = 0;
	m_colorMapSplatting[0] = 0;
	m_colorMapSplatting[1] = 0;
	m_colorMapSplatting[2] = 0;
	m_colorMapSplatting[3] = 0;
	m_normalMapSplatting[0] = 0;
	m_normalMapSplatting[1] = 0;
	m_normalMapSplatting[2] = 0;
	m_normalMapSplatting[3] = 0;
}

Material::Material(MATERIAL_TYPE type, string colorMap, string normalMap, string albedoMap, f32 textureRepeat, f32 textureAlbedoRepeat, f32 specularExponent, u32 useSpecular, DISPLACEMENT_TYPE useParallax, string cubeMap, Vec4 diffuseColor, u32 useDiffuseColor)
	: m_colorMap(0), m_normalMap(0), m_albedoMap(0), m_cubeMap(0), m_useAlphaTest(0)
{
	m_type = type;

	if(colorMap.size())							m_colorMap		= gEngine.resMgr->load<Texture>(colorMap);
	if(normalMap.size() && r_enabledNormalMap)	m_normalMap		= gEngine.resMgr->load<Texture>(normalMap);
	if(albedoMap.size() && r_enabledAlbedo)		m_albedoMap		= gEngine.resMgr->load<Texture>(albedoMap);
	if(cubeMap.size()   && r_enabledCubeMap)	m_cubeMap		= gEngine.resMgr->load<CubeTexture>(cubeMap);

	m_textureRepeat			= textureRepeat;
	m_textureAlbedoRepeat	= textureAlbedoRepeat;
	m_specularExponent		= (r_enabledSpecular)?(specularExponent):(0);
	m_diffuseColor			= diffuseColor;

	if(!m_colorMap)
	{
		m_diffuseColor		= Vec4(0,0,0,1);
		m_useDiffuseColor	= 1;
	}
	else
		m_useDiffuseColor		= useDiffuseColor;

	m_useSpecular			= (r_enabledSpecular)?(useSpecular):(0);

	if(m_normalMap && r_enabledParallaxMap)
		m_useParallax			= useParallax;
	else
		m_useParallax			= DT_BUMP_MAPPING;

	m_splatting				= 0;
	m_indexMap				= 0;
	m_colorMapSplatting[0]	= 0;
	m_colorMapSplatting[1]	= 0;
	m_colorMapSplatting[2]	= 0;
	m_colorMapSplatting[3]	= 0;
	m_normalMapSplatting[0] = 0;
	m_normalMapSplatting[1] = 0;
	m_normalMapSplatting[2] = 0;
	m_normalMapSplatting[3] = 0;

	m_animationSpeed		= 1;

	update();
}

Material::~Material()
{
	cleanup();
}

void Material::cleanup()
{
	gEngine.resMgr->release(m_colorMap);	m_colorMap	= 0;
	gEngine.resMgr->release(m_normalMap);	m_normalMap = 0;
	gEngine.resMgr->release(m_albedoMap);	m_albedoMap = 0;
	gEngine.resMgr->release(m_cubeMap);		m_albedoMap = 0;

	gEngine.resMgr->release(m_colorMapSplatting[0]);	m_colorMapSplatting[0] = 0;
	gEngine.resMgr->release(m_colorMapSplatting[1]);	m_colorMapSplatting[1] = 0;
	gEngine.resMgr->release(m_colorMapSplatting[2]);	m_colorMapSplatting[2] = 0;
	gEngine.resMgr->release(m_colorMapSplatting[3]);	m_colorMapSplatting[3] = 0;

	gEngine.resMgr->release(m_normalMapSplatting[0]);	m_normalMapSplatting[0] = 0;
	gEngine.resMgr->release(m_normalMapSplatting[1]);	m_normalMapSplatting[1] = 0;
	gEngine.resMgr->release(m_normalMapSplatting[2]);	m_normalMapSplatting[2] = 0;
	gEngine.resMgr->release(m_normalMapSplatting[3]);	m_normalMapSplatting[3] = 0;
}

void Material::update()
{
	if(!m_splatting)
		m_useNormalMap		= (r_enabledNormalMap && m_normalMap != 0);
	else 
		m_useNormalMap		= (r_enabledNormalMap && 1);

	m_useAlbedoMap			= (r_enabledAlbedo	  && m_albedoMap != 0);
	m_useCubeMap			= (r_enabledCubeMap	  && m_cubeMap != 0);

	m_useAlphaTest					= 0;
	m_useFlipBackfaceNormals		= 0;
	m_useBranchAnimation			= 0;
	m_useLeafAnimation				= 0;
	m_useSphereAmbient				= 0;
	m_useSubsurfaceScattering		= 0;
	m_animationSpeed				= 1;

	const u16 MacroBitsNum	= 12;
	const c8 *MacroNames[MacroBitsNum]	= {	"USE_NORMALMAP", "USE_SPECULAR", "USE_ALBEDO", "USE_PARALLAX", "USE_DIFFUSECOLOR", "USE_CUBEMAP", "USE_ALPHATEST", 
											"USE_FLIP_BACKFACE_NORMAL", "USE_BRANCH_ANIMATION", "USE_LEAF_ANIMATION", "USE_SPHERE_AMBIENT", "USE_SUBSURFACE_SCATTERING" };
	const u16 MacroBits[MacroBitsNum]	= { 0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13 };
		  u16 MacroValues[MacroBitsNum];
	

	string shaderName = "";

	if(m_type == MT_DEFAULT){			shaderName = "MultishaderDefault"; }
	else if(m_type == MT_TERRAIN){		shaderName = "MultishaderTerrain"; }
	else if(m_type == MT_ALPHATEST){	shaderName = "MultishaderDefault"; m_useAlphaTest = 1; m_useFlipBackfaceNormals = 1; }
	else if(m_type == MT_TREE){			shaderName = "MultishaderDefault"; m_useAlphaTest = 0; m_useBranchAnimation = r_enabledVegetationAnimation; }
	else if(m_type == MT_LEAF){			shaderName = "MultishaderDefault"; m_useAlphaTest = 1; m_useFlipBackfaceNormals = 1; m_useLeafAnimation = r_enabledVegetationAnimation; m_useSphereAmbient = 1; m_useSubsurfaceScattering = (r_enabledSubsurfaceScattering && 1); }

	m_useAnimation = (m_useBranchAnimation || m_useLeafAnimation);

	if(!shaderName.size()){ m_shader = 0; return; }

	m_multishader = gEngine.shaderMgr->load(shaderName+".fx", shaderName+"_%X.fxo", MacroBitsNum, MacroNames, MacroBits);

	

	MacroValues[0]	= m_useNormalMap;
	MacroValues[1]	= m_useSpecular;
	MacroValues[2]	= m_useAlbedoMap;
	MacroValues[3]	= (u32)m_useParallax;
	MacroValues[4]	= m_useDiffuseColor;
	MacroValues[5]	= m_useCubeMap;
	MacroValues[6]	= m_useAlphaTest;
	MacroValues[7]	= m_useFlipBackfaceNormals;
	MacroValues[8]	= m_useBranchAnimation;
	MacroValues[9]	= m_useLeafAnimation;
	MacroValues[10]	= m_useSphereAmbient;
	MacroValues[11]	= m_useSubsurfaceScattering;

	m_shader = m_multishader->getShader(MacroValues);
}
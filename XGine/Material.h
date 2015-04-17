/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

enum XGINE_API MATERIAL_TYPE
{
	MT_DEFAULT = 0,
	MT_ALPHATEST = 1,
	MT_GRASS = 2,
	MT_TREE = 3,
	MT_LEAF = 4,
	MT_TERRAIN = 5,
	MT_FORCE_DWORD = 0x6fffffff
};

enum XGINE_API DISPLACEMENT_TYPE
{
	DT_BUMP_MAPPING = 0,
	DT_PARALLAX_MAPPING = 1,
	DT_RELIEF_MAPPING = 2,
	DT_VANILLA_RELIEF_MAPPING = 3,
	DT_PARALLAX_OCCLUSION_MAPPING = 4,
	DT_FORCE_DWORD = 0x5fffffff 
};

struct XGINE_API Material
{
	Material();
	Material(MATERIAL_TYPE type, string colorMap, string normalMap = "", string albedoMap = "", f32 textureRepeat = 1.0f, f32 textureAlbedoRepeat = 1.0f, f32 specularExponent = 40.0f, u32 useSpecular = 0, DISPLACEMENT_TYPE useParallax = DT_BUMP_MAPPING, string cubeMap = "", Vec4 diffuseColor = Vec4(1,1,1,1), u32 useDiffuseColor = 0);
	~Material();
	
	void				cleanup();
	void				update();

	MATERIAL_TYPE		m_type;

	Texture*			m_colorMap;
	Texture*			m_normalMap;

	u32					m_splatting;
	LPDIRECT3DTEXTURE9	m_indexMap;
	Texture*			m_colorMapSplatting[4];
	Texture*			m_normalMapSplatting[4];

	Texture*			m_albedoMap;
	CubeTexture*		m_cubeMap;
	f32					m_textureRepeat;
	f32					m_textureAlbedoRepeat;
	f32					m_specularExponent;
	Vec4				m_diffuseColor;
	u32					m_useNormalMap;
	u32					m_useSpecular;
	DISPLACEMENT_TYPE	m_useParallax;
	u32					m_useAlbedoMap;
	u32					m_useCubeMap;
	u32					m_useDiffuseColor;
	u32					m_useSubsurfaceScattering;
	u32					m_useAlphaTest;
	u32					m_useSphereAmbient;
	u32					m_useFlipBackfaceNormals;
	u32					m_useBranchAnimation;
	u32					m_useLeafAnimation;
	u32					m_useAnimation;
	f32					m_animationSpeed;

	Multishader			*m_multishader;
	Shader				*m_shader;
};
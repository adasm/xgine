/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
/////////////////////////////////////////////////////////////////////////

#ifdef XGINE_EXPORTS
	#define XGINE_API __declspec(dllexport)
#else
	#define XGINE_API __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////

#define XGINE_VER_NO "6.0.0"
#define XGINE_VER "XGine " XGINE_VER_NO " [Build: " __TIMESTAMP__ "]"

/////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxerr.lib")
#define DIRECTINPUT_VERSION 0x0800

#pragma warning(disable:4018)
#pragma warning(disable:4100)
#pragma warning(disable:4238)
#pragma warning(disable:4244)
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#pragma warning(disable:4305)
#pragma warning(disable:4512)
#pragma warning(disable:4800)

#include "PCH_XGine.h"

typedef D3DXVECTOR2			Vec2, Vector2;
typedef D3DXVECTOR3			Vec3;
typedef D3DXVECTOR4			Vec4;
typedef D3DXMATRIX			Mat;
typedef D3DXPLANE			Plane;
typedef char				c8;
typedef	unsigned char		u8;
typedef signed short		i16;
typedef	unsigned short		u16;
typedef __int32				i32;
typedef unsigned __int32	u32;
typedef __int64				i64;
typedef unsigned __int64	u64;
typedef float				f32;
using namespace std;

#define DEFDEC(X) virtual~X(){}
#define FILELINE __FILE__,__LINE__
#define DXRELEASE(ptr) if(ptr){ptr->Release(); ptr = 0;}

#define tmin(a, b) (((a)<(b))?(a):(b))
#define tmax(a, b) (((b)<(a))?(a):(b))

/** P R E D E C L S **/
//NVTools
class  MeshMender;

//Tokenizer
struct Tokenizer;
//Kernel
enum   LOG_TYPE;
struct ILogger;
struct Logger;
struct FSFileEntry;
struct IFileSystem;
struct FileSystem;
struct UniversalValue;
struct Console;
struct Buffer;
struct MemoryTrack;
struct IMemoryManager;
struct MemoryManager;
struct Kernel;
//Timer
struct ITimer;
struct Timer;
//Device
struct RenderWindowInfo;
struct Device;
//Input
struct IInput;
struct Input;
//ImageEffects;
struct IPostprocess;
struct Bloom;
struct GuassianBlur;

//Bounding
struct BoundingBox;
struct BoundingSphere;
struct BoundingCone;
struct Quadtree;
template< typename T > struct Octree;
//Node
struct INode;
template< typename T > struct Node;

//GUI
class Desktop;
//ResMgr
struct IRes;
struct ResMgr;
struct Texture;
struct CubeTexture;
struct VolumeTexture;
struct Mesh;
struct Font;
//ShaderMgr
struct Shader;
struct Multishader;
struct ShaderMgr;
//Renderer
struct RenderTexture;
struct RenderTextureCube;
struct FrustumCulling;
struct OcclusionCulling;
struct ShadowMapping;
struct GeoPacket;
template< typename T > struct EntityList;
struct RenderList;
struct MRT;
struct Renderer2D;
enum   REND_TYPE;
enum   REND_PASS;
enum   REND_SHADER_TYPE;
enum   FuncCustomShader;
enum   CULL_TYPE;
struct RendSet;
enum   VertexDeclaration;
struct InstanceData;
struct InstanceDataPacked;
struct HDRToneMap;
struct DepthOfField;
struct HeatHaze;
struct VolumetricLightScattering;
struct StateManager;
struct Renderer;
//Materials
//struct IMaterial;
//struct UMaterial;
enum   MATERIAL_TYPE;
struct Material;
//Camera
class	ICamera;
class	PerspectiveCamera;
class	FreeCamera;
//Light
struct Light;
struct LighstList;

//Entities
struct EntityPose;
struct IEntity;
struct SkyDome;
struct RendObj;
struct TerrainVertex;
struct Terrain;
struct TerrainDesc;
struct TerrainManager;
struct Water;
struct HQWater;
//struct StaticMesh;
//struct DynamicMesh;
enum   ParticleBlending;
enum   BillboardRotation;
struct Particle;
struct Vegetation;
//struct DynamicLight;
struct Scene;

//alAudio
struct alAudio;

//Engine
struct Plugin;
struct Engine;

struct Surface;

/**  G L O B A L S  **/
extern XGINE_API Engine	gEngine;

extern XGINE_API string g_appName;

extern XGINE_API u32	g_logvfs;
extern XGINE_API u32	g_logmem;
extern XGINE_API u32	g_logker;
extern XGINE_API u32	r_width;
extern XGINE_API u32	r_width;
extern XGINE_API u32	r_height;
extern XGINE_API u32	r_windowed;
extern XGINE_API u32	r_wireframe;
extern XGINE_API u32	r_hidestats;
extern XGINE_API u32	r_multisample;
extern XGINE_API u32	r_geometryInstancing;
extern XGINE_API u32	r_packInstanceDataToFloat16;
extern XGINE_API f32	r_cameraNearZ;
extern XGINE_API f32	r_cameraFarZ;
extern XGINE_API string g_path_shaders;
extern XGINE_API string g_path_textures;
extern XGINE_API string g_path_models;
extern XGINE_API u32	r_shadowMapSpotSize;
extern XGINE_API u32	r_shadowMapDirSize;
extern XGINE_API f32	g_shadowMapDirViewVolume;
extern XGINE_API f32	g_shadowMapDirViewDist;
extern XGINE_API f32	g_shadowMapDirNearZ;
extern XGINE_API f32	g_shadowMapDirFarZ;
extern XGINE_API f32	r_shadowMappingZBias;

extern XGINE_API u32	r_optimizeRSChanges;

extern XGINE_API u32	g_allowShaderCache;
extern XGINE_API string g_shaderCacheName;
extern XGINE_API u32	g_shaderCacheCompression;
extern XGINE_API u32	g_allowMeshCache;
extern XGINE_API u32	g_allowResourcesCache;
extern XGINE_API u32	g_resourcesCacheCompression;

extern XGINE_API u32	r_autoGenerateLODs;
extern XGINE_API u32	r_enableLODs;

extern XGINE_API string g_terrainShader;
extern XGINE_API string g_terrainTexColorMap1;
extern XGINE_API string g_terrainTexColorMap2;
extern XGINE_API string g_terrainTexColorMap3;
extern XGINE_API string g_terrainTexColorMap4;
extern XGINE_API string g_terrainTexNormalMap1;
extern XGINE_API string g_terrainTexNormalMap2;
extern XGINE_API string g_terrainTexNormalMap3;
extern XGINE_API string g_terrainTexNormalMap4;
extern XGINE_API u32	g_terrainDebugBB;
extern XGINE_API u32	g_terrainSectorSize;
extern XGINE_API f32	g_terrainMaxHeight;
extern XGINE_API f32	g_terrainLowVBExponent;
extern XGINE_API f32	g_terrainLowVBExponentX;
extern XGINE_API u32	g_terrainMaxLODLevels;
extern XGINE_API f32	g_terrainMaxDistHighGeo;
extern XGINE_API f32	g_terrainMaxDistHighGeoStep;
extern XGINE_API f32	g_terrainMaxDistHighTex;
extern XGINE_API f32	g_terrainMaxDistHighTexFade;
extern XGINE_API u32	g_terrainLodStart;
extern XGINE_API u32	g_terrainLodEnabled;

extern XGINE_API u32 g_rendObjDebugBB;
extern XGINE_API u32 g_rendOctreeDebugBB;
 
extern XGINE_API f32 r_bloomBrightThreshold;
extern XGINE_API f32 r_bloomAmount;
extern XGINE_API f32 r_bloomIntensity;
extern XGINE_API f32 r_bloomBaseIntensity;
extern XGINE_API f32 r_bloomSaturation;
extern XGINE_API f32 r_bloomBaseSaturation;
extern XGINE_API u32 r_bloomEnabled;
extern XGINE_API u32 r_enabledSoftParticles;

extern XGINE_API u32 r_enabledReflectionBlur;
extern XGINE_API u32 r_enabledSkyReflection;
extern XGINE_API u32 r_enabledTerrainReflection;
extern XGINE_API u32 r_enabledSurfacesReflection;
extern XGINE_API u32 r_enabledEntitiesReflection;
extern XGINE_API u32 r_deferredStencilOptimize;
extern XGINE_API u32 r_deferredScissorOptimize;
extern XGINE_API u32 r_deferredDebugComplexity;
extern XGINE_API u32 r_enabledNormalMap;
extern XGINE_API u32 r_enabledParallaxMap;
extern XGINE_API u32 r_enabledSpecular;
extern XGINE_API u32 r_enabledAlbedo;
extern XGINE_API u32 r_enabledCubeMap;
extern XGINE_API u32 r_enabledSubsurfaceScattering;
extern XGINE_API u32 r_enabledVegetationAnimation;

extern XGINE_API u32 r_cullingLights;
extern XGINE_API u32 r_cullingFrustum;
extern XGINE_API u32 r_cullingOcclusion;

extern XGINE_API u32 r_shadowDist;
extern XGINE_API u32 r_enabledShadows; 
extern XGINE_API u32 r_enabledShadowMapping;
extern XGINE_API u32 r_enabledVarianceShadowMapping;
extern XGINE_API u32 r_enabledBlurVarianceShadowMaps;
extern XGINE_API u32 r_enabledBoxFilterVarianceShadowMaps;
extern XGINE_API u32 r_enabledExponentialShadowMapping;
extern XGINE_API u32 r_enabledShadowMappingGeoRendering;
extern XGINE_API u32 r_enabledShadowMappingDeferred;
extern XGINE_API u32 r_enabledShadowMappingPCF;
extern XGINE_API u32 r_enabledShadowMappingPCFHigh;
extern XGINE_API u32 r_enabledShadowMappingPCFMed;
extern XGINE_API u32 r_enabledSMScreenSpaceBlur;
extern XGINE_API u32 r_enabledShadowVolume;

extern XGINE_API u32 r_guassianBlurKernelSize;
extern XGINE_API u32 r_enabledDeferred;
extern XGINE_API u32 r_enabledLightPrePass;
extern XGINE_API u32 r_forceForward;
extern XGINE_API u32 r_enabledZPrePass;
extern XGINE_API u32 r_renderSky;
extern XGINE_API u32 r_renderTerrain;
extern XGINE_API u32 r_renderSurfaces;
extern XGINE_API u32 r_renderEntities;
extern XGINE_API u32 r_enabledSSAO;
extern XGINE_API f32 r_ssaoRadius;
extern XGINE_API f32 r_ssaoFresZSize;
extern XGINE_API f32 r_ssaoCap;
extern XGINE_API f32 r_ssaoMultiplier;
extern XGINE_API f32 r_ssaoDepthTolerance;

extern XGINE_API u32	r_hdrRendering;
extern XGINE_API u32	r_hdrToneMap;
extern XGINE_API f32	r_hdrToneMapScale;
extern XGINE_API u32	r_hdrEyeAdaptation;
extern XGINE_API f32	r_hdrEyeAdaptationTimeScale;
extern XGINE_API u32	r_hdrBloom;
extern XGINE_API f32	r_hdrBloomAmount;
extern XGINE_API u32	r_hdrBrightPass;
extern XGINE_API Vec3	r_hdrColorGrade;
extern XGINE_API u32	r_hdrColorGradeEnabled;

extern XGINE_API u32 r_dofEnabled;
extern XGINE_API f32 r_dofFocalPlane;
extern XGINE_API f32 r_dofNearFocalPlane;
extern XGINE_API f32 r_dofFarFocalPlane;
extern XGINE_API u32 r_dofEyeAdaptation;
extern XGINE_API f32 r_dofEyeAdaptationTimeScale;
extern XGINE_API f32 r_dofBlurExponent;

extern XGINE_API u32 r_heatHazeEnabled;
extern XGINE_API f32 r_heatHazeStrength;
extern XGINE_API f32 r_heatHazeTimeScale;

extern XGINE_API u32	r_waterRenderingEnabled;
extern XGINE_API u32	r_highQualityWaterEnabled;
extern XGINE_API u32	r_waterOcean;
extern XGINE_API f32	r_waterAmplitude;
extern XGINE_API f32	r_waterH0coef;
extern XGINE_API f32	r_waterH1coef;
extern XGINE_API f32	r_waterH2coef;
extern XGINE_API f32	r_waterH3coef;
extern XGINE_API f32	r_waterDetailNormalMapCoef;
extern XGINE_API f32	r_waterDetailNormalMapScale;
extern XGINE_API f32	r_waterFresnelR0;
extern XGINE_API f32	r_waterRefractionStrength;
extern XGINE_API f32	r_waterSpecularExp;
extern XGINE_API Vec3	r_waterColor;
extern XGINE_API f32	r_refExp;

extern XGINE_API u32	r_sunColorEnabled;
extern XGINE_API Vec3	r_sunColor;
extern XGINE_API u32	r_skyColorEnabled;
extern XGINE_API Vec3	r_skyColorFront;
extern XGINE_API Vec3	r_skyColorBack;
extern XGINE_API f32	r_sunSizeExp;

extern XGINE_API u32	r_enabledVLS;
extern XGINE_API u32	r_enabledHDRVLS;

extern XGINE_API Vec4	r_fogParams;
extern XGINE_API Vec3	r_fogColor;

/////////////////////////////////////////////////////////////////////
#include "Tokenizer.h"
#include "Kernel.h"
#include "Device.h"
#include "Input.h"
#include "IPostprocess.h"
#include "Bloom.h"
#include "GuassianBlur.h"
#include "RenderTexture.h"
#include "RenderTextureCube.h"
#include "FrustumCulling.h"
#include "OcclusionCulling.h"
#include "ShadowMapping.h"
#include "Bounding.h"
#include "Node.h"
#include "MRT.h"
#include "HDRToneMap.h"
#include "DepthOfField.h"
#include "HeatHaze.h"
#include "VolumetricLightScattering.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "IRes.h"
#include "ResMgr.h"
#include "Font.h"
#include "GUI.h"
#include "Texture.h"
#include "CubeTexture.h"
#include "VolumeTexture.h"
#include "ICamera.h"
#include "Camera.h"
#include "Mesh.h"
#include "Surface.h"
#include "Octree.h"
#include "RenderList.h"
#include "Shader.h"
#include "Multishader.h"
#include "ShaderMgr.h"
#include "Material.h"
#include "MathUtilities.h"
#include "IEntity.h"
#include "SkyDome.h"
#include "RendObj.h"
#include "Particle.h"
#include "Vegetation.h"
#include "Terrain.h"
#include "TerrainManager.h"
#include "Water.h"
#include "HQWater.h"
#include "Scene.h"
#include "Light.h"
#include "alAudio/alAudio.h"
#include "Plugin.h"
#include "Engine.h"


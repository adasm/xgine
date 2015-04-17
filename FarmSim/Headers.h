#pragma once
#include "../XGine/XGine.h"
#pragma comment(lib, "../Debug/XGine.lib")

#define getVarName(var) #var
#define G16R16(g, r) ((DWORD) (((WORD) (g) | ((WORD) (r) << 16))))
#define GetG16Value(rg) ((WORD) (rg))
#define GetR16Value(rg) ((WORD) ((DWORD)(rg) >> 16))
#define switchCoordFrom3dsMax(vec) Vec3(vec.x, vec.z, vec.y)

Vec3 getVec3MaxParameter(Vec3 *vec);
Vec3 getVec3MinParameter(Vec3 *vec);
Vec3 getVec3FromString(string vec);
Vec4 getVec4FromString(string vec);
int	getIntFromString(string entry);
float getFloatFromString(string entry);

void setSurfaceTransform(Surface* entity, Vec3 pos, Vec3 rot, bool rotateBB);
void setSurfaceTransform(Surface* entity, Mat *mat, bool rotateBB);
void QuatToEuler(const D3DXQUATERNION *quat, Vec3 *rot);
Vec2 getCoordinatesFromIndex(int index, float width);

#pragma comment(lib, "PhysXLoader.lib")
#pragma comment(lib, "NxCharacter.lib")
#include "NxPhysics.h"
#include "NxControllerManager.h"
#include "NxBoxController.h"
#include "NxCapsuleController.h"
#include "NxCooking.h"
#include "NxStream.h"
#include "NXU_helper.h"
#include "Stream.h"
#include <crtdbg.h>

struct Table;
struct TableEntry;
class ConfigManager;
struct IConfig;

enum AdditionalDataType;
struct AdditionalData;
class TriggerReport;

enum CollisionGroup;
struct Dynamics;


class KeyboardManager;

struct SunEntity;
struct CropType;
struct TerrainParams;

enum VariableType;
struct CfgVariable;
class CfgLoader;

class VehicleController;
class TriPod;

class LensFlare;
struct LightParams;
class DayNightManager;

struct ObjectParams;
struct SceneParams;
struct SceneEntry;

struct ShopItem;
enum DeviceShopType;
struct ShopParams;
struct IDeviceShop;

struct TractorShopItem;


class WheelSet;
struct WheelSetPose;
struct WheelSetParams;

struct VehiclePose;
struct TrailerPose;
class ActionBox;
struct IAgriDevice;
struct VehicleParams;
struct TrailerParams;
struct AgriDeviceParams;
struct TriPodDimms;
enum WheelDriveType;
struct VehicleParamsEx;
struct IVehicle;
struct Tractor;
struct Combine;
struct Plow;
struct Seeder;
enum VehicleType;
struct CombinePose;
struct CombineParams;
struct PlowDimms;
struct CutterParams;

class VehicleEngine;

struct ITrailer;
struct Trailer;
enum TrailerWheelNb;
enum TrailerType;

class GrassManager;
struct FieldParams;

struct FieldEntity;
typedef class FieldSector* FieldSectorPTR;
struct Field;

struct GrassMesh;

class Game;
class Allocator;
class CharacterController;
struct EntityData;
enum CollisionShapeType;
class World;


class Core;
struct Player;

struct TreeEntity;
struct TreeBunchEntity;
struct TreeBranchEntity;
struct TreeType;
struct TreeParams;

class VegetationManager;

struct HarvestDumpPointParams;
enum GrassTypeSelection;
class HarvestDumpPoint;

struct HarvestShopParams;
class HarvestShop;
struct TractorShop;


class TrafficManager;
class TrafficActor;
enum TrafficNodeType;
typedef struct TrafficNode * TrafficNodePTR;
class NavigationMesh;


#include "LensFlare.h"
#include "SunEntity.h"
#include "DayNightManager.h"
#include "KeyboardManager.h"
#include "ActionBox.h"

#include "GrassManager.h"
#include "GrassMesh.h"
#include "FieldEntity.h"
#include "FieldSector.h"

#include "WheelSet.h"
#include "VehicleController.h"
#include "Dimms.h"
#include "CFGLoader.h"


#include "World.h"
#include "UserData.h"
#include "IVehicle.h"
#include "IAgriDevice.h"
#include "Tractor.h"
#include "Combine.h"
#include "TriPod.h"
#include "Plow.h"
#include "Cutter.h"
#include "Seeder.h"

#include "ITrailer.h"
#include "Trailer.h"

#include "NxWheel.h"
#include "NxWheelDesc.h"
#include "CharacterController.h"
#include "Allocator.h"
#include "EntityData.h"
#include "Core.h"
#include "Player.h"
#include "ShopData.h"
#include "IDeviceShop.h"

#include "VehicleEngine.h"

#include "TerrainLoader.h"
#include "TreeEntity.h"
/*#include "TreeType.h"
#include "TreeParams.h"*/
#include "HarvestDumpPoint.h"
#include "VegetationManager.h"


#include "HarvestShop.h"

#include "Dynamics.h"
#include "Game.h"
#include "SceneLoader.h"

#include "AdditionalData.h"
#include "TriggerReport.h"
#include "Traffic.h"
#include "TrafficActor.h"
#include "NavigationMesh.h"

#include "TractorShop.h"

extern Core core;

extern float g_grassEntityRenderDistance;		//Whole entity distance rendering
extern float g_grassSectorRenderDistance;		//Sector distance rendering
extern float g_grassSectorLowRenderDistance;	//LowPoly sector distance rendering
extern float g_grassTileSize;					//Size of one tile
extern float g_grassTextureCellWidth;			//Width of one cell in Texture with crop Types;
extern int g_grassSectorSize;					//Size of sector in tiles
extern int g_grassEntitiesNum;					//Number of entities on whole scene
extern int g_grassSectorExistTime;				//Time in ms before deleting textures from RAM memory
extern float g_treeRenderHiDetailsDistance;		//Distance in which is rendered the best quality of trees
extern float g_treeRenderMedDetailsDistance;	//Distance in which is rendered good quality of trees
extern float g_treeRenderLowDetailsDistance;	//Distance in which is rendered bad quality of trees

extern string g_appCache;
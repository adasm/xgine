#pragma once
#include "Headers.h"

class World
{
public:
	void					init(SceneParams *sceneParams);
	void					close();

	void					update();
	void					updateInput();

	void					render();
	void					addToWorld(INode *entity, CollisionShapeType collShape, f32 density, CollisionGroup collGroup, Mesh* collisionMesh = NULL, bool dynamic = false);
	void					addToWorld(Light* light);
	void					addToWorld(IAgriDevice *device);
	void					addToWorld(IVehicle* vehicle);
	void					addToWorld(ITrailer* trailer);
	void					addToWorld(HarvestShop* shop);
	void					addToWorld(IDeviceShop* shop);
	void					addTerrainToWorld(const string& texHmName, const string& texIndName);


	void					newTurn();

	//void					loadVehicle(Vec3 pos, string fname);

	/*void					loadVehicleDimms(string fname, VehicleDimms* vehicleDimms);
	void					loadVehicleParamsEx(string fname, VehicleParamsEx* vehicleParams);*/
	/*IVehicle*				loadVehicle(Vec3 pos, string fname, VehicleType vehicleType);
	ITrailer*				loadTrailer(Vec3 pos, string fname);*/

	Scene*					getScene() { return m_scene; }
	Player*					getPlayerInfo() { return m_playerInfo; }
	GrassManager*			getGrassManager() { return m_grassManager; }
	TerrainManager*			getTerrainManager() { return m_terrainManager; }
	DayNightManager*		getDayNightManager() { return m_dayNightManager; }
	TrafficManager*			getTrafficManager() { return m_trafficManager; }

	float					getDeltaTime() { return m_deltaTime; }

	vector<IVehicle*>*		getVehicles() { return &m_vehicles; }
	vector<ITrailer*>*		getTrailers() { return &m_trailers; }
	vector<IAgriDevice*>*	getAgriDevices() { return &m_agriDevices; }
	vector<HarvestShop*>*	getHarvestShops() { return &m_harvestShops; }
	vector<IDeviceShop*>*	getDeviceShops() { return &m_deviceShops; }

	float					getWaterLevel() { return m_waterLevel; }
protected:
	float					m_waterLevel;
	float					m_deltaTime;
	Scene*					m_scene;
	GrassManager*			m_grassManager;
	TerrainManager*			m_terrainManager;
	DayNightManager*		m_dayNightManager;
	VegetationManager*		m_vegetationManager;
	TrafficManager*			m_trafficManager;

	Light*					m_torch;
	u32						m_torchEnabled;

	vector<IVehicle*>		m_vehicles;
	vector<ITrailer*>		m_trailers;
	vector<IAgriDevice*>	m_agriDevices;
	vector<HarvestShop*>	m_harvestShops;
	vector<IDeviceShop*>	m_deviceShops;




	//vector<Combine*>		m_combines;
	/*vector<IVehicle*>		m_vehicles;
	vector<ITrailer*>		m_trailers;*/

	Player*					m_playerInfo;
};
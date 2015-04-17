#pragma once
#include "Headers.h"


struct TractorShopItem : ShopItem
{
	WheelDriveType wdt;
	int gearNum;
	int fuelCapacity;
	int fuelConsumptionPerMth;
};


struct TractorShop : IDeviceShop
{
public:
	TractorShop(Vec3 entryPoint, ShopParams *shopParams);
	DeviceShopType				getShopType();
	Scene*						getScene() { return m_scene; }

	Vec3						getEntryPoint() { return m_entryPoint; }
	float						getEntryPointRadius() { return m_entryPointRadius; }

	void						nextItem();
	void						previousItem();

	void						update();
	void						updateInput();

	void						buyItem();
protected:
	bool						checkIfParkingIsFree(int x, int y, float interval);
	Scene*						m_scene;
	vector<TractorShopItem*>	m_items;
	int							m_actualItem;

	Vec3						m_itemDisplayPos;
	Vec3						m_entryPoint;
	float						m_entryPointRadius;
	FreeCamera*					m_camera;

	Vec3						m_parkingPos;
	int							m_parkingColumns;
	int							m_parkingRows;
};
//#pragma once
//#include "Headers.h"
//
//struct AgriDeviceShopItem : ShopItem
//{
//	int width;
//};
//
//struct AgriDeviceShop : IDeviceShop
//{
//public:
//	AgriDeviceShop(Vec3 entryPoint, ShopParams *shopParams);
//	DeviceShopType				getShopType() { return DST_AGRIDEVICE; }
//	Scene*						getScene() { return m_scene; }
//
//	Vec3						getEntryPoint() { return m_entryPoint; }
//	float						getEntryPointRadius() { return m_entryPointRadius; }
//
//	void						nextItem();
//	void						previousItem();
//
//	void						update();
//	void						updateInput();
//
//	void						buyItem();
//protected:
//	bool						checkIfParkingIsFree(int x, int y, float interval);
//	Scene*						m_scene;
//	vector<AgriDeviceShopItem*>	m_items;
//	int							m_actualItem;
//
//	Vec3						m_itemDisplayPos;
//	Vec3						m_entryPoint;
//	float						m_entryPointRadius;
//	FreeCamera*					m_camera;
//
//	Vec3						m_parkingPos;
//	int							m_parkingColumns;
//	int							m_parkingRows;
//};
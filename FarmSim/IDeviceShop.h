#pragma once
#include "Headers.h"


struct ShopItem
{
	vector<Surface*> SurfaceSet;
	string fname;
	string typeName;
	int price;
};

enum DeviceShopType
{
	DST_TRACTOR,
	DST_AGRIDEVICE,
};

struct IDeviceShop
{
public:
	virtual void				update() = 0;
	virtual void				updateInput() = 0;
	virtual DeviceShopType		getShopType() = 0;
	virtual Scene*				getScene() = 0;
	virtual Vec3				getEntryPoint() = 0;
	virtual float				getEntryPointRadius() = 0;
};
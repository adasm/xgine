#pragma once
#include "Headers.h"


struct Player
{
public:
	Player(Vec3 playerPos);
	CharacterController*	getCharController()				{ return m_character; }
	IVehicle*				getDrivingVehicle()				{ return m_drivingVehicle; }
	IDeviceShop*			getVisitingShop()				{ return m_visitingShop; }
	void					getOnVehicle(IVehicle* vehicle);
	void					getOutOfVehicle();
	void					getOutOfShop();
	void					updateInput();
	void					update();

	void					addMoney(int money);
	int						getMoney() { return m_money; }

	bool					isFreeFlyMode() { return m_freeFlyMode; }
protected:
	void					updateInputFreeFly();

	IVehicle*				searchVehicleToEnter();
	IDeviceShop*			searchDeviceShopToEnter();

	CharacterController*	m_character;
	IVehicle*				m_drivingVehicle;
	IDeviceShop*			m_visitingShop;

	int						m_money;
	bool					m_freeFlyMode;
};
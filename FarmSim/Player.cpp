#include "Player.h"


Player::Player(Vec3 playerPos)
{
	m_freeFlyMode = false;
	m_visitingShop = NULL;
	//m_visitingShop = new TractorShop(Vec3(0, 0, 0), "S");
	m_drivingVehicle = NULL;
	m_character = new CharacterController(core.dynamics, core.dynamics->getControllerManager(), core.dynamics->getScene(), NxVec3(playerPos), 5, 0.8f, 1.8f);
	m_money = 10000;
}

void Player::getOutOfVehicle()
{
	if(m_drivingVehicle->getBackTriPod())
	if(m_drivingVehicle->getVehicleType() == VT_TRACTOR && m_drivingVehicle->getBackTriPod()->isAttached() == AT_TRAILER)
	{
		if(((ITrailer*)(m_drivingVehicle->getBackTriPod()->getAttach()))->isDumperUp())
			return;
	}
	//m_drivingVehicle->getVehicleController()->stopMotorAndBreak();
	m_drivingVehicle->getVehicleController()->releaseMotor();
	m_drivingVehicle->getVehicleController()->setParkingBrake();
	NxVec3 newPos = m_drivingVehicle->getVehicleController()->getActor()->getGlobalPosition();
	Vec3 left = m_drivingVehicle->getVehicleController()->getForwardVec();
	D3DXVec3Cross(&left, &left, &Vec3(0, 1, 0));
	//if(m_drivingVehicle->getVehicleDimms()->chassisDimm.x > m_drivingVehicle->getVehicleDimms()->chassisDimm.z)
	newPos += NxVec3(left) * (1.5 + m_drivingVehicle->getVehicleController()->getVehicleParams()->chassisDimm.z);
	newPos.y += 2;
	m_character->setCharacterPos(newPos);
	m_character->update();
	m_drivingVehicle = NULL;
}

void Player::getOutOfShop()
{
	NxVec3 newPos = NxVec3(m_visitingShop->getEntryPoint().x, m_visitingShop->getEntryPoint().y, m_visitingShop->getEntryPoint().z);
	newPos.y += 3;
	m_character->setCharacterPos(newPos);
	m_character->update();
	m_visitingShop = NULL;
}

void Player::getOnVehicle(IVehicle* vehicle)
{
	m_drivingVehicle = vehicle;
}

void Player::updateInput()
{
	if(core.game->getKeyboardManager()->isChangeCamera())
		m_freeFlyMode = !m_freeFlyMode;
	if(m_freeFlyMode)
	{
		updateInputFreeFly();
		return;
	}
	if(m_visitingShop)
		m_visitingShop->updateInput();
	else
	{
		if(m_drivingVehicle)
		{
			m_drivingVehicle->updateInput();
			if(m_drivingVehicle->getBackTriPod())
				if(m_drivingVehicle->getBackTriPod()->isAttached() == AT_DEVICE)
				{
					((IAgriDevice*)m_drivingVehicle->getBackTriPod()->getAttach())->updateInput();
				}
			if(m_drivingVehicle->getFrontTriPod())
				if(m_drivingVehicle->getFrontTriPod()->isAttached() == AT_DEVICE)
				{
					((IAgriDevice*)m_drivingVehicle->getFrontTriPod()->getAttach())->updateInput();
				}
		}
	}
	if(core.game->getKeyboardManager()->isEnterVehicle())
	{
		if(m_visitingShop)
			getOutOfShop();
		else
		{
			IDeviceShop* shopToGetIn = searchDeviceShopToEnter();
			if(shopToGetIn)
			{
				m_visitingShop = shopToGetIn;
				getCharController()->setCharacterPos(NxVec3(0, 100, 0));
			}
		}

		if(!m_visitingShop)
		{
			if(m_drivingVehicle)
				getOutOfVehicle();
			else
			{
				IVehicle *vehToEnter = searchVehicleToEnter();
				if(vehToEnter)
				{
					getOnVehicle(vehToEnter);
					m_character->setCharacterPos(NxVec3(0, 100, 0));
				}
			}
		}
	}
	if(!m_visitingShop)
	{
		NxVec3 move(0, 0, 0);
		if(core.game->getKeyboardManager()->isForward())
			move += NxVec3(*core.game->getCamera()->getViewDirection());
		else
		{
			if(core.game->getKeyboardManager()->isBackward())
				move += -NxVec3(*core.game->getCamera()->getViewDirection());
		}
		if(core.game->getKeyboardManager()->isLeft())
			move += NxVec3(core.game->getCamera()->getViewLeft()->x, core.game->getCamera()->getViewLeft()->y, core.game->getCamera()->getViewLeft()->z);
		else
			if(core.game->getKeyboardManager()->isRight())
				move += -NxVec3(core.game->getCamera()->getViewLeft()->x, core.game->getCamera()->getViewLeft()->y, core.game->getCamera()->getViewLeft()->z);

		if(gEngine.input->isKeyDown(DIK_SPACE))
			getCharController()->startJump(15);
		getCharController()->move(move);
	}
}

void Player::update()
{
	gEngine.renderer->addTxt("Money - - - - - - - - - - %d", m_money);
	getCharController()->update();
	int money = 0;
	if(m_visitingShop)
		m_visitingShop->update();
	if(m_drivingVehicle)
	if(m_drivingVehicle->getBackTriPod())
	if(m_drivingVehicle->getVehicleType() == VT_TRACTOR && m_drivingVehicle->getBackTriPod()->isAttached() == AT_TRAILER)
	{
		ITrailer *trailer = (ITrailer*)m_drivingVehicle->getBackTriPod()->getAttach();
		if(trailer->isDumperUp() && trailer->getPayloadValue())
		{
			Vec3 dumpPoint = trailer->getPayloadDumpPos();
			int payloadToTransfer = gEngine.kernel->tmr->get() * 0.5f;
			if(payloadToTransfer > trailer->getPayloadValue())
				payloadToTransfer = trailer->getPayloadValue();
			for(int i = 0; i < core.game->getWorld()->getHarvestShops()->size(); i++)
			{
				//if((*core.game->getWorld()->getTrailers())[i]->getDumperActionBox()->isPointInside(&dumpPoint))
				HarvestShop* actualShop = (*core.game->getWorld()->getHarvestShops())[i];
				if(actualShop->getHarvestDumpPoint()->getActionBox()->isPointInside(&dumpPoint))
				{
					if(actualShop->getCapacity() > actualShop->getActualPayload(trailer->getLoadedCropType()))
					//if((*core.game->getWorld()->getTrailers())[i]->getPayloadValue() < (*core.game->getWorld()->getTrailers())[i]->getCapacity())
					{
						if(actualShop->getActualPayload(trailer->getLoadedCropType()) + trailer->getPayloadValue() < actualShop->getCapacity())
						{
							money = actualShop->sellHarvest(payloadToTransfer, trailer->getLoadedCropType());
							if(money > 0)
							{
								trailer->addPayload(-payloadToTransfer, trailer->getLoadedCropType());
								m_money += money;
							}
						}
						else
						{
							payloadToTransfer = actualShop->getCapacity() - actualShop->getActualPayload(trailer->getLoadedCropType());
							money = actualShop->sellHarvest(payloadToTransfer, trailer->getLoadedCropType());
							if(money > 0)
							{
								trailer->addPayload(-payloadToTransfer, trailer->getLoadedCropType());
								m_money += money;
							}
						}
					}
				}
			}


			//////////////////
		}
	}
}

IVehicle* Player::searchVehicleToEnter()
{
	float minDist = 100.0f;
	IVehicle* vehToGetOn = NULL;
	for(int i = 0; i < (*core.game->getWorld()->getVehicles()).size(); i++)
	{
		if(minDist > getCharController()->getCharacterActor()->getGlobalPosition().distance((*core.game->getWorld()->getVehicles())[i]->getVehicleController()->getActor()->getGlobalPosition()))
		{
			vehToGetOn = (*core.game->getWorld()->getVehicles())[i];
			minDist = getCharController()->getCharacterActor()->getGlobalPosition().distance((*core.game->getWorld()->getVehicles())[i]->getVehicleController()->getActor()->getGlobalPosition());
		}
	}
	return vehToGetOn;
}

IDeviceShop* Player::searchDeviceShopToEnter()
{
	IDeviceShop* shopToGetIn = NULL;
	IDeviceShop* actualShop;
	for(int i = 0; i < (*core.game->getWorld()->getDeviceShops()).size(); i++)
	{
		actualShop = (*core.game->getWorld()->getDeviceShops())[i];
		NxVec3 entryPoint = NxVec3(actualShop->getEntryPoint().x, actualShop->getEntryPoint().y, actualShop->getEntryPoint().z);
		if(actualShop->getEntryPointRadius() > getCharController()->getCharacterActor()->getGlobalPosition().distance(entryPoint))
			return actualShop;
	}
	return NULL;
}

void Player::addMoney(int money)
{
	m_money += money;
}

void Player::updateInputFreeFly()
{
	Vec3 move(0, 0, 0);
	if(core.game->getKeyboardManager()->isForward())
		move.z = 0.3f*(float)gEngine.kernel->tmr->get();
	else if(core.game->getKeyboardManager()->isBackward())
		move.z = -0.3f*(float)gEngine.kernel->tmr->get();
	if(core.game->getKeyboardManager()->isLeft())
		move.x = 0.3f*(float)gEngine.kernel->tmr->get();
	else if(core.game->getKeyboardManager()->isRight())
		move.x = -0.3f*(float)gEngine.kernel->tmr->get();
	core.game->getCamera()->moveByRel(move);
}
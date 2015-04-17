#include "TrafficActor.h"

TrafficActor::TrafficActor(IVehicle* vehicle)
{
	m_vehicle = vehicle;
	NxBoxShapeDesc triggerBox;
	NxActorDesc actorDesc;

	m_triggerPosition = Vec3(m_vehicle->getVehicleController()->getVehicleParams()->chassisDimm.x * 2, 0, 0);
	triggerBox.dimensions.set(m_vehicle->getVehicleController()->getVehicleParams()->chassisDimm.x * 2, m_vehicle->getVehicleController()->getVehicleParams()->chassisDimm.y*5, m_vehicle->getVehicleController()->getVehicleParams()->chassisDimm.z * 1.5f); //1m cube
	triggerBox.shapeFlags |= NX_TRIGGER_ENABLE; //Make this shape a trigger.

	actorDesc.setToDefault();
	actorDesc.shapes.pushBack(&triggerBox);
	actorDesc.globalPose.t.set(0, 0, 0); // Position at the origin

	m_triggerActor = core.dynamics->getScene()->createActor(actorDesc);
	m_actualNode = NULL;
	m_vehicle->getVehicleController()->getActor()->userData = (void*)new AdditionalData(DT_TRAFFIC_ACTOR, (void*)this);
	m_triggerActor->userData = (void*)new AdditionalData(DT_TRAFFIC_ACTOR, (void*)this);
	m_vehicle->update();
	m_triggerCollision = 0;
	m_triggerInCollision = false;
	m_upsideDownParameter = 0;
}

void TrafficActor::update()
{
	Vec3 actorForward = m_vehicle->getVehicleController()->getForwardVec();
	Vec3 actorLeft = m_vehicle->getVehicleController()->getLeftVec();
	Vec3 actorPos = m_vehicle->getPos();
	actorPos.y = 0;
	actorForward.y = 0;
	actorLeft.y = 0;
	D3DXVec3Normalize(&actorForward, &actorForward);
	D3DXVec3Normalize(&actorLeft, &actorLeft);
	Vec3 upVec;
	D3DXVec3Cross(&upVec, &m_vehicle->getVehicleController()->getForwardVec(), &m_vehicle->getVehicleController()->getLeftVec());
	if(getVec3MinParameter(&upVec).y)
	{
		gEngine.renderer->addTxt("upsideDownParameter: %d", m_upsideDownParameter);
		m_upsideDownParameter++;
		if(m_upsideDownParameter > 250)
			m_vehicle->getVehicleController()->resetPose();
	}
	else
		m_upsideDownParameter = 0;
	float difference = 5.0f;
	if(!m_actualNode)
		m_actualNode = core.game->getWorld()->getTrafficManager()->getNavigationMesh()->getNearestNode(actorPos);
	float distance = D3DXVec3Length(&Vec3(actorPos - m_actualNode->next[0]->position));
	if(distance <= difference)
	{
		m_actualNode = m_actualNode->next[0];
	}
	Vec3 direction = m_actualNode->next[0]->position - actorPos;
	direction.y = 0;
	D3DXVec3Normalize(&direction, &direction);
	Vec3 leftOfDirection;
	D3DXVec3Cross(&leftOfDirection, &direction, &Vec3(0, -1, 0));
	leftOfDirection.y = 0;
	D3DXVec3Normalize(&leftOfDirection, &leftOfDirection);
	float dot = D3DXVec3Dot(&actorForward, &direction);
	if(dot > 1)
		dot = 1;
	if(dot < -1)
		dot = -1;
	float angle = acos(dot);
	m_vehicle->getVehicleController()->setSteerAngle(0);
	if(angle)
	{
		float dot2 = D3DXVec3Dot(&leftOfDirection, &actorForward);
		if(dot2 > 1)
			dot2 = 1;
		if(dot2 < -1)
			dot2 = -1;
		if(acos(dot2) <= D3DX_PI * 0.5f)
			m_vehicle->getVehicleController()->setSteerAngle(-angle);
		else
			m_vehicle->getVehicleController()->setSteerAngle(angle);
	}
	if(!m_triggerInCollision && !m_triggerCollision)
	{
		if(m_vehicle->getVehicleController()->getSpeed() < 500.0f)
			m_vehicle->getVehicleController()->setAccelerate();
		else
			m_vehicle->getVehicleController()->setBrake();
	}
	else
	{
		m_vehicle->getVehicleController()->releaseMotor();
		m_vehicle->getVehicleController()->setBrake();
	}
	//gEngine.renderer->addTxt("Dot: %.3f\nPos: %.3f %.3f %.3f\nDistance: %.3f\nForward: %.3f %.3f %.3f\nLeft: %.3f %.3f %.3f\nNext node: %.3f %.3f %.3f\nAngle: %.3f\nNEXT VEHICLE", D3DXVec3Dot(&Vec3(m_vehicle->getVehicleController()->getLeftVec()), &leftOfDirection), m_vehicle->getPos().x, m_vehicle->getPos().y, m_vehicle->getPos().z, distance, m_vehicle->getVehicleController()->getForwardVec().x, m_vehicle->getVehicleController()->getForwardVec().y, m_vehicle->getVehicleController()->getForwardVec().z, m_vehicle->getVehicleController()->getLeftVec().x, m_vehicle->getVehicleController()->getLeftVec().y, m_vehicle->getVehicleController()->getLeftVec().z, m_actualNode->next[0]->position.x, m_actualNode->next[0]->position.y, m_actualNode->next[0]->position.z, angle);
	//gEngine.renderer->addTxt("Collision detection: %d", m_triggerCollision);
	m_vehicle->update();
	Mat rot;
	Vec3 transTrig;
	D3DXMatrixRotationQuaternion(&rot, &D3DXQUATERNION(m_vehicle->getVehicleController()->getActor()->getGlobalOrientationQuat().x, m_vehicle->getVehicleController()->getActor()->getGlobalOrientationQuat().y, m_vehicle->getVehicleController()->getActor()->getGlobalOrientationQuat().z, m_vehicle->getVehicleController()->getActor()->getGlobalOrientationQuat().w));
	D3DXVec3TransformCoord(&transTrig, &m_triggerPosition, &rot);
	NxMat34 mat = m_vehicle->getVehicleController()->getActor()->getGlobalPose();
	mat.t.x += transTrig.x;
	mat.t.y += transTrig.y;
	mat.t.z += transTrig.z;
	//D3DXVec3TransformCoord(&triggerPosition, &m_triggerPosition, m_vehicle->getVehicleController()->getVehiclePose()->
	//NxVec3 triggerPosition = 
	m_triggerActor->setGlobalPose(mat);
	m_triggerInCollision = false;
}

void TrafficActor::triggerIncrease()
{
	m_triggerCollision++;
}

void TrafficActor::triggerDecrease()
{
	m_triggerCollision--;
}

void TrafficActor::triggerInCollision()
{
	m_triggerInCollision = true;
}
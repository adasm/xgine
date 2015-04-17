#include "CharacterController.h"

#define COLLIDABLE_MASK	(1<<GROUP_COLLIDABLE_NON_PUSHABLE)

void SetActorCollisionGroup(NxActor* actor, NxCollisionGroup group)
{
    NxShape*const* shapes = actor->getShapes();
    NxU32 nShapes = actor->getNbShapes();
    while (nShapes--)
    {
        shapes[nShapes]->setGroup(group);
	}
}

void SetActorMaterial(NxActor *actor, NxMaterialIndex index)
{
	NxU32 nbShapes = actor->getNbShapes();
	NxShape*const* shapes = actor->getShapes();

	while (nbShapes--)
	{
		shapes[nbShapes]->setMaterial(index);
	}
}

CharacterController::CharacterController(Dynamics * dyn, NxControllerManager* manager, NxScene *scene, NxVec3 pos, f32 speed, NxVec3 dim)
{
	m_manager = manager;

	NxBoxControllerDesc desc;
	desc.position.x = pos.x;
	desc.position.y = pos.y;
	desc.position.z = pos.z;
	desc.extents = dim;
	desc.skinWidth = 0.1f;
	desc.slopeLimit = cosf(NxMath::degToRad(45.0f));
	desc.stepOffset = 0.5f;
	desc.upDirection = NX_Y;
	desc.callback = &m_hitReport;
	m_controller = m_manager->createController(scene, desc);

	m_speed = speed;
	m_jump = false;
	m_characterVec = NxVec3(0,0,0);
	m_dynamics = dyn;
}

CharacterController::CharacterController(Dynamics * dyn, NxControllerManager* manager, NxScene *scene, NxVec3 pos, f32 speed, NxReal radius, NxReal height)
{
	m_manager = manager;

	NxCapsuleControllerDesc desc;
	desc.position.x = pos.x;
	desc.position.y = pos.y;
	desc.position.z = pos.z;
	desc.height = height;
	desc.radius = radius;
	desc.skinWidth = 0.1f;
	desc.slopeLimit = cosf(NxMath::degToRad(45.0f));
	desc.stepOffset = 0.5f;
	desc.upDirection = NX_Y;
	desc.climbingMode = CLIMB_EASY;
	desc.callback = &m_hitReport;
	m_controller = m_manager->createController(scene, desc);

	m_speed = speed;
	m_jump = false;
	m_characterVec = NxVec3(0,0,0);
	m_dynamics = dyn;
}

CharacterController::~CharacterController()
{
	if(m_manager)
	{
		m_manager->releaseController(*m_controller);
	}
}


NxActor* CharacterController::getCharacterActor()
{
	return m_controller->getActor();
}

bool CharacterController::setCharacterPos(NxVec3 pos)
{
	return m_controller->setPosition(NxExtendedVec3(pos.x, pos.y, pos.z));
}

void CharacterController::updateCharacterExtents(NxScene *scene, NxVec3 offset)
{
	switch (m_controller->getType())
	{
	case NX_CONTROLLER_BOX:
		{
			NxBoxController* boxCtrl = ((NxBoxController*) m_controller);
			NxExtendedVec3 pos = boxCtrl->getPosition(); 
			NxVec3 dim = boxCtrl->getExtents() + offset; // expanded extent
			if (dim.x <= 0.0f || dim.y <= 0.0f || dim.z <= 0.0f)
				break;
			NxBounds3 worldBounds;
			worldBounds.setCenterExtents(NxVec3(pos.x, pos.y + offset.y, pos.z), dim); // expand upward
			
			boxCtrl->setCollision(false); // avoid checking overlap with ourselves
			if (scene->checkOverlapAABB(worldBounds))
				printf("\n>> Can NOT change the size of the box controller!");
			else
			{
				boxCtrl->setExtents(dim);
				boxCtrl->setPosition(pos + NxExtendedVec3(0, offset.y, 0));
			}
			boxCtrl->setCollision(true);
			break;
		}
	case NX_CONTROLLER_CAPSULE:
		{
			NxCapsuleController* capsuleCtrl = ((NxCapsuleController*) m_controller);
			NxF32 height = capsuleCtrl->getHeight() + offset.y; // height offset = offset.y
			NxF32 radius = capsuleCtrl->getRadius() + offset.y/2; // radius offset = offset.y/2
			NxExtendedVec3 pos = capsuleCtrl->getPosition();
			pos.y += offset.y; // radius offset + 1/2 * height offset
			if (height <= 0.0f || radius <= 0.0f)
				break;
			NxCapsule worldCapsule;
			worldCapsule.p0 = NxVec3(pos.x, pos.y-height/2, pos.z);
			worldCapsule.p1 = NxVec3(pos.x, pos.y+height/2, pos.z);
			worldCapsule.radius = radius;
			capsuleCtrl->setCollision(false);
			if (scene->checkOverlapCapsule(worldCapsule))
			{
				printf("\n>> Can NOT change the size of the capsule controller!");
			}
			else
			{
				capsuleCtrl->setHeight(height);
				capsuleCtrl->setRadius(radius);
				capsuleCtrl->setPosition(pos);
			}
			capsuleCtrl->setCollision(true);
			break;
		}
		break;
	}
}


void CharacterController::update()
{
	NxVec3 disp;
	m_dynamics->getScene()->getGravity(disp);

	if (m_pushCharacter)
	{
		NxVec3 horizontalDisp = m_characterVec;
		horizontalDisp.y = 0.0f;
		horizontalDisp.normalize();
		disp += horizontalDisp * m_speed;
	}
/////////////
	disp *= m_dynamics->getDeltaTime();
	NxF32 height = getHeight(m_dynamics->getDeltaTime()); // compute height(Y) in jumping
	if (height != 0.0f)
	{
		disp.y += height;
	}

	NxU32 collisionFlags;
	//Move(disp, collisionFlags);
	m_controller->move(disp, COLLIDABLE_MASK, 0.001f, collisionFlags);
	//m_controller->move(disp, 0, 0.001, collisionFlags);
	if (collisionFlags & NXCC_COLLISION_DOWN)  
		stopJump();
	else
	{
		if(!m_jump)
			startJump(0);
	}

	m_manager->updateControllers();
}

void CharacterController::startJump(NxF32 v0)
{
	if(m_jump)  return;
	m_jumpTime = 0.0f;
	m_V0	= v0;
	m_jump = true;
}

void CharacterController::stopJump()
{
	m_jump = false;
}

NxF32 CharacterController::getHeight(NxF32 elapsedTime)
{
	NxVec3 grav;
	m_dynamics->getScene()->getGravity(grav);
	if (!m_jump)  return 0.0f;
	NxF32 Vt = m_V0 + grav.y*m_jumpTime; // Vt = Vo + GT
	m_jumpTime += elapsedTime;
	return Vt*elapsedTime + 1/2*grav.y*elapsedTime*elapsedTime; // S = VtT + 1/2GT^2
}

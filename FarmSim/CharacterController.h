#pragma once
#include "Headers.h"

enum CollisionGroup
{
	GROUP_NON_COLLIDABLE,
	GROUP_COLLIDABLE_NON_PUSHABLE,
	GROUP_COLLIDABLE_PUSHABLE,
};

class HitReportController : public NxUserControllerHitReport
{
	public:

	virtual NxControllerAction onShapeHit(const NxControllerShapeHit& hit)
	{
		if(hit.shape)
		{
			NxCollisionGroup group = hit.shape->getGroup();
			if(group==GROUP_COLLIDABLE_PUSHABLE)
			{
				NxActor& actor = hit.shape->getActor();
				if(actor.isDynamic())
				{
					// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
					// useless stress on the solver. It would be possible to enable/disable vertical pushes on
					// particular objects, if the gameplay requires it.
					if(hit.dir.y==0.0f)
					{
						NxF32 coeff = actor.getMass() * hit.length * 10.0f;
						actor.addForceAtLocalPos(hit.dir*coeff, NxVec3(0,0,0), NX_IMPULSE);
					}
				}
			}
		}

		return NX_ACTION_NONE;
	}

	virtual NxControllerAction onControllerHit(const NxControllersHit& hit)
	{
		return NX_ACTION_NONE;
	}

};


class CharacterController
{
public:
	CharacterController(Dynamics * dyn, NxControllerManager* manager, NxScene *scene, NxVec3 pos, f32 speed, NxVec3 dim);
	CharacterController(Dynamics * dyn, NxControllerManager* manager, NxScene *scene, NxVec3 pos, f32 speed, NxReal radius, NxReal height);

	~CharacterController();

	NxActor* getCharacterActor();
	bool setCharacterPos(NxVec3 pos);

	inline void move(NxVec3 move) { m_pushCharacter = true; m_characterVec = move; }
	inline void setSpeed(float speed) { m_speed = speed; }

	void updateCharacterExtents(NxScene *scene, NxVec3 offset);
	void update();

	void startJump(NxF32 v0);
	void stopJump();

	NxF32 getHeight(NxF32 elapsedTime);

private:
	Dynamics*	m_dynamics;
	NxControllerManager* m_manager; // Singleton
	NxController* m_controller;
	HitReportController m_hitReport;

	NxVec3 m_characterVec;
	bool m_pushCharacter;

	bool m_jump;
	NxF32 m_V0;
	NxF32 m_jumpTime;

	float m_speed;
};


void SetActorCollisionGroup(NxActor *actor, NxCollisionGroup group);
void SetActorMaterial(NxActor *actor, NxMaterialIndex index);
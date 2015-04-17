#pragma once

//NVPhysX
#include "Headers.h"

NxWheelShape* AddWheelToActor(NxActor* actor, NxWheelDesc* wheelDesc, bool Coord3dsMax = false);

enum CollisionShapeType
{
	NO_COLLISION	= 0,
	BOX				= 1 << 0,
	SPHERE			= 1 << 1,
	TRIANGLE		= 1 << 2
};

struct Dynamics
{
	u32									init();
	void								close();

	void								release(NxActor *actor);
	NxActor*							createActor(Mesh *meshPtr, Mat *matWorld, f32 density, CollisionShapeType collisionShape, Vec3 *initialVel, bool staticActor = false);
	NxActor*							createBox(const NxVec3& pos, const NxVec3& boxDim, const NxReal density);
	NxActor*							createSphere(const NxVec3& pos, const NxReal radius, const NxReal density);
	NxActor*							addActor(NxActor *actor);

	void								startSimulation(float delta); // Start simulation (non-blocking)
	void								endSimulation(); // End - flush data stream
	inline float						getDeltaTime() { return m_deltaTime; }

	inline NxPhysicsSDK*				getSDK()	 { return m_physicsSDK; }
	inline NxCookingInterface*			getCooking() { return m_cooking; }
	inline NxScene*						getScene() { return m_scene; }
	inline NxControllerManager*			getControllerManager() { return m_manager; }
			TriggerReport*				getTriggerReport();
	void								updateCars(float delta);
protected:
	float								m_deltaTime;
	NxControllerManager*				m_manager;
	NxPhysicsSDK*						m_physicsSDK;
	NxCookingInterface*					m_cooking;
	NxScene*							m_scene;
	TriggerReport*						m_triggerReport;

	typedef map< NxActor*, NxActor* >	NxActorMap;
	typedef NxActorMap::iterator		NxActorMapIt;
	NxActorMap							m_actors;
};

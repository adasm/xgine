#include "Dynamics.h"
/*
u32 Dynamics::init()
{
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	btVector3 worldAabbMin(-1000,-1000,-1000);
	btVector3 worldAabbMax(1000,1000,1000);

	m_broadphase = new btAxisSweep3(worldAabbMin, worldAabbMax, 32766);

	m_solver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
	m_dynamicsWorld->getSolverInfo().m_numIterations = 4;
	m_dynamicsWorld->getSolverInfo().m_solverMode = SOLVER_SIMD+SOLVER_USE_WARMSTARTING;
	m_dynamicsWorld->getDispatchInfo().m_useEpa = true;
	m_dynamicsWorld->getDispatchInfo().m_enableSPU = true;
	m_dynamicsWorld->setGravity(btVector3(0,-9.81f,0));
	
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),0.0f);
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,0,0));
	btScalar mass(0.);
	btVector3 localInertia(0,0,0);
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	m_dynamicsWorld->addRigidBody(body);
	body->setFriction(1000);

	return 1;
}

void Dynamics::close()
{
	//remove rigidbodies
	if(m_dynamicsWorld->getNumCollisionObjects())
		gEngine.kernel->log->prnEx("Dynamics: Removing %u unreleased objects from dynamics", m_dynamicsWorld->getNumCollisionObjects());

	for(int i = m_dynamicsWorld->getNumCollisionObjects()-1; i >= 0; --i)
	{
		btCollisionObject* o = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* b = btRigidBody::upcast(o);
		if(b && b->getMotionState())
			delete b->getMotionState();
		m_dynamicsWorld->removeCollisionObject(o);
		delete o;
	}

	//delete
	delete m_dynamicsWorld;
	delete m_solver;
	delete m_broadphase;
	delete m_dispatcher;
	delete m_collisionConfiguration;
}

btRigidBody* Dynamics::createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape)
{
	bool dynamic = (mass != 0.0f);

	btVector3 localInertia(0,0,0);
	if(dynamic)
		shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody* body = new btRigidBody(mass, myMotionState, shape, localInertia);

	m_dynamicsWorld->addRigidBody(body);
	return body;
}

void Dynamics::removeRigidBody(btRigidBody* body)
{
	m_dynamicsWorld->removeRigidBody(body);
}

void Dynamics::update()
{
	//if(g_phys_enabled)
	{
		//m_dynamicsWorld->setGravity(btVector3(0,g_phys_gravity,0));
		
		m_dynamicsWorld->stepSimulation(gEngine.kernel->tmr->get());
	}
}
*/

/*m_dynamicsBody = dynamics->createRigidBody(m_dynamicsMass, btTransform(btQuaternion(m_rotation.y,m_rotation.x,m_rotation.z), (btVector3&)m_position), m_dynamicsShape);	
	m_dynamicsBody->setFriction(m_dynamicsFricion);*/

/*switch(collisionShape)
	{
		case BOX:
			m_dynamicsShape = new btBoxShape(btVector3(m_boundingBox.getWidth() * 0.5f, m_boundingBox.getHeight() * 0.5f, m_boundingBox.getDepth() * 0.5f));
			break;
		case SPHERE:
			m_dynamicsShape = new btSphereShape(m_boundingBox.getBoundingSphere()->Radius);
			break;
		case TRIANGLE:
			{
				btTriangleIndexVertexArray * indexVertexArrays = new btTriangleIndexVertexArray(m_mesh->getNumFaces(),
							m_mesh->getBIndices(), 3*sizeof(int), m_mesh->getNumVertices(),(btScalar*) &m_mesh->getBVertices()[0].x(),sizeof(btVector3));
				bool useQuantizedAabbCompression = true;
				m_dynamicsShape= new btBvhTriangleMeshShape(indexVertexArrays,useQuantizedAabbCompression,false);
			}break;
	}*/

/*if(m_dynamicsBody)	dynamics->removeRigidBody(m_dynamicsBody);
	if(m_dynamicsShape) delete(m_dynamicsShape);*/

/*if(m_dynamicsBody->getActivationState() == 1)
	{
		static btVector3 bbmin, bbmax;
		m_dynamicsBody->getWorldTransform().getOpenGLMatrix(m_world);
		m_dynamicsBody->getAabb(bbmin, bbmax);
		m_boundingBox.reset((Vec3)bbmin, (Vec3)bbmax);
	}*/

/*
struct XGINE_API Dynamics
{
	u32				init();
	void			close();

	btRigidBody*	createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape);
	void			removeRigidBody(btRigidBody* body);
	void			update();

protected:
	btCollisionConfiguration*	m_collisionConfiguration;
	btCollisionDispatcher*		m_dispatcher;
	btDynamicsWorld*			m_dynamicsWorld;
	btBroadphaseInterface*		m_broadphase;
	btConstraintSolver*			m_solver;
};
*/

const char* getNxSDKCreateError(const NxSDKCreateError& errorCode) 
{
	switch(errorCode) 
	{
		case NXCE_NO_ERROR: return "NXCE_NO_ERROR";
		case NXCE_PHYSX_NOT_FOUND: return "NXCE_PHYSX_NOT_FOUND";
		case NXCE_WRONG_VERSION: return "NXCE_WRONG_VERSION";
		case NXCE_DESCRIPTOR_INVALID: return "NXCE_DESCRIPTOR_INVALID";
		case NXCE_CONNECTION_ERROR: return "NXCE_CONNECTION_ERROR";
		case NXCE_RESET_ERROR: return "NXCE_RESET_ERROR";
		case NXCE_IN_USE_ERROR: return "NXCE_IN_USE_ERROR";
		default: return "Unknown error";
	}
};

u32 Dynamics::init()
{
	// Initialize PhysicsSDK
	UserAllocator* gAllocator = new UserAllocator;
	m_manager = NxCreateControllerManager(gAllocator);
	m_deltaTime = 0;


	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	m_physicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, NULL, desc, &errorCode);
	if(m_physicsSDK == NULL) 
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Dynamics", "PhysicsSDK Create error (%d - %s).", errorCode, getNxSDKCreateError(errorCode));
		return 0;
	}

	m_physicsSDK->setParameter(NX_CONTINUOUS_CD, true);
	m_physicsSDK->setParameter(NX_CCD_EPSILON, 0.01f);
	m_physicsSDK->setParameter(NX_SKIN_WIDTH, 0.0025f);

	// Initialize CookingSDK
	m_cooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	if(m_cooking == NULL) 
	{
		gEngine.kernel->log->prn(LT_FATAL, "Dynamics", "CookingLib Create error.");
		return 0;
	}
	if(!m_cooking->NxInitCooking())
	{
		gEngine.kernel->log->prn(LT_FATAL, "Dynamics", "CookingLib initialization error.");
		return 0;
	}

	m_physicsSDK->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425);

	// Create a scene
	m_triggerReport = new TriggerReport;
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= NxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.userTriggerReport     = m_triggerReport;
	sceneDesc.simType				= NX_SIMULATION_HW;
	m_scene = m_physicsSDK->createScene(sceneDesc);
	if(!m_scene)
	{
		sceneDesc.simType			= NX_SIMULATION_SW;
		m_scene = m_physicsSDK->createScene(sceneDesc);
	}
	if(m_scene == NULL) 
	{
		gEngine.kernel->log->prn(LT_FATAL, "Dynamics", "Unable to create a PhysX scene.");
		return 0;
	}

	// Set default material
	NxMaterial* defaultMaterial = m_scene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	//defaultMaterial->setStaticFriction(0.8f);
	defaultMaterial->setStaticFriction(1);
	//defaultMaterial->setDynamicFriction(0.5f);
	defaultMaterial->setDynamicFriction(1);

	// Create ground plane
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	m_scene->createActor(actorDesc);

	return 1;
}

TriggerReport* Dynamics::getTriggerReport()
{
	return m_triggerReport;
}

void Dynamics::close()
{
	if(m_physicsSDK != NULL)
	{
		if(m_scene != NULL) m_physicsSDK->releaseScene(*m_scene);
		m_scene = NULL;
		//NxReleaseCookingLib(m_cooking);
		NxReleasePhysicsSDK(m_physicsSDK);
		m_physicsSDK = NULL;
	}
}

void Dynamics::release(NxActor *actor)
{
	if(actor) 
	{
		NxActorMapIt it = m_actors.find(actor); 
		if(it != m_actors.end())
			m_actors.erase(it);

		NxConvexMesh* convex = NULL;
		NxTriangleMesh* mesh = NULL;
		NxU32 n = actor->getNbShapes();
		if (n > 0) 
		{
			for(NxU32 i = 0; i < n; ++i)
			{
				convex = NULL;
				mesh = NULL;

				NxShape* shape = actor->getShapes()[i];
				if (shape->isConvexMesh()) 
				{
					convex = &(shape->isConvexMesh()->getConvexMesh());
				}
				else if(shape->isTriangleMesh()) 
				{
					mesh = &(shape->isTriangleMesh()->getTriangleMesh());
				}
				actor->releaseShape(*shape);
				if (convex && convex->getReferenceCount() == 0) {
					m_physicsSDK->releaseConvexMesh(*convex);
					convex = NULL;
				}
				if (mesh && mesh->getReferenceCount() == 0) {
					m_physicsSDK->releaseTriangleMesh(*mesh);
					mesh = NULL;
				}
			}
		}
		NxScene& scene = actor->getScene();
		scene.releaseActor(*actor);
	}
}

NxActor* Dynamics::createActor(Mesh *meshPtr, Mat *matWorld, f32 density, CollisionShapeType collisionShape, Vec3 *initialVel, bool staticActor)
{
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	if(!meshPtr)return 0; //TUTAJ, jak jest meshPtr - sypie sie podczas cooking (nizej)
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////


	BoundingBox m_boundingBox;
	if(meshPtr)
		m_boundingBox = meshPtr->getBoundingBox();
	NxActor *m_actor;

	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
	if(initialVel) bodyDesc.linearVelocity = (NxVec3&)*initialVel;
	
	NxActorDesc actorDesc;
	actorDesc.body			= (staticActor)?(0):(&bodyDesc);
	actorDesc.density		= (staticActor)?(0):(density);
	if(matWorld) actorDesc.globalPose.setColumnMajor44((const NxF32*)matWorld);

	switch(collisionShape)
	{
		case BOX:
			{
				NxBoxShapeDesc boxDesc;
				boxDesc.dimensions = NxVec3(m_boundingBox.getWidth()/2, m_boundingBox.getHeight()/2, m_boundingBox.getDepth()/2);	
				boxDesc.localPose.t = NxVec3(0, 0, 0);
				actorDesc.shapes.pushBack(&boxDesc);
			}break;
		case SPHERE:
			{
				NxSphereShapeDesc sphereDesc;
				sphereDesc.radius = m_boundingBox.getBoundingSphere()->Radius;
				sphereDesc.localPose.t = NxVec3(0, 0, 0);
				actorDesc.shapes.pushBack(&sphereDesc);
			}break;
		case TRIANGLE:
			{
				if(meshPtr->getNumVertices())
				{
					actorDesc.body					= 0;
					NxTriangleMeshDesc meshDesc;
					meshDesc.numTriangles			= meshPtr->getNumFaces();
					meshDesc.triangleStrideBytes	= 3 * sizeof(u32);
					meshDesc.triangles				= meshPtr->getIndices();
					meshDesc.numVertices			= meshPtr->getNumVertices();
					meshDesc.pointStrideBytes		= 3 * sizeof(f32);
					meshDesc.points					= meshPtr->getVertices();
					meshDesc.flags					= 0;
					MemoryWriteBuffer buf;
					getCooking()->NxCookTriangleMesh(meshDesc, buf); //UserStream("bin.dat", false));//
					NxTriangleMeshShapeDesc meshShapeDesc; 
					meshShapeDesc.meshData = getSDK()->createTriangleMesh(MemoryReadBuffer(buf.data)); //UserStream("bin.dat", true));//
					meshShapeDesc.localPose.t = NxVec3(0, 0, 0);
					actorDesc.shapes.pushBack(&meshShapeDesc);
				}
			}break;
		default:
			gEngine.kernel->log->prn(LT_WARNING, "Dynamics", "Unknown CollisionType value. Collision shape has not been added.");
			break;
	}
	
	m_actor = getScene()->createActor(actorDesc);

	if(m_actor)
		m_actors.insert( make_pair( m_actor, m_actor ) );

	return m_actor;
}

NxActor* Dynamics::addActor(NxActor *actor)
{
	if(actor)
		m_actors.insert( make_pair( actor, actor ) );
	return actor;
}

void Dynamics::startSimulation(float delta)
{
	// Srart simulation (non-blocking)
	m_deltaTime = delta;
	m_scene->simulate(delta);
}

void Dynamics::endSimulation()
{
	// Fetch simulation results
	m_scene->flushStream();
	m_scene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}


NxActor* Dynamics::createBox(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(boxDim.x, boxDim.y, boxDim.z);
	//boxDesc.localPose.t = NxVec3(0, boxDim.y, 0);
	boxDesc.localPose.t = NxVec3(0, 0, 0);

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.globalPose.t = pos;

	NxBodyDesc bodyDesc;
	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = 0;
	}
	return m_scene->createActor(actorDesc);
}

NxActor* Dynamics::createSphere(const NxVec3& pos, const NxReal radius, const NxReal density)
{

	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius;
	sphereDesc.localPose.t = NxVec3(0, radius, 0);

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&sphereDesc);
	actorDesc.globalPose.t = pos;

	NxBodyDesc bodyDesc;
	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = 0;
	}
	return m_scene->createActor(actorDesc);
}

NxWheelShape* AddWheelToActor(NxActor* actor, NxWheelDesc* wheelDesc, bool Coord3dsMax)
{
	NxWheelShapeDesc wheelShapeDesc;

	// Wheel material globals
	NxMaterial* wsm = NULL;

	// Create a shared car wheel material to be used by all wheels
	if (!wsm)
	{
		NxMaterialDesc m;
		//m.flags |= NX_MF_DISABLE_FRICTION;
		wsm = core.dynamics->getScene()->createMaterial(m);
	}
	wheelShapeDesc.materialIndex = wsm->getMaterialIndex();

	wheelShapeDesc.localPose.t = wheelDesc->position;
	NxQuat q;
	if(Coord3dsMax)
		q.fromAngleAxis(90, NxVec3(0,0,1));
	else
		q.fromAngleAxis(90, NxVec3(0,1,0));
	wheelShapeDesc.localPose.M.fromQuat(q);

	NxReal heightModifier = (wheelDesc->wheelSuspension + wheelDesc->wheelRadius) / wheelDesc->wheelSuspension;

	wheelShapeDesc.suspension.spring = wheelDesc->springRestitution*heightModifier;
	wheelShapeDesc.suspension.damper = 0;//wheelDesc->springDamping*heightModifier;
	//wheelShapeDesc.suspension.damper = wheelDesc->springDamping*heightModifier;
	wheelShapeDesc.suspension.targetValue = wheelDesc->springBias*heightModifier;

	wheelShapeDesc.radius = wheelDesc->wheelRadius;
	wheelShapeDesc.suspensionTravel = wheelDesc->wheelSuspension; 
	wheelShapeDesc.inverseWheelMass = 10;	//not given!? TODO

	wheelShapeDesc.lateralTireForceFunction.stiffnessFactor *= wheelDesc->frictionToSide;	
	wheelShapeDesc.longitudalTireForceFunction.stiffnessFactor *= wheelDesc->frictionToFront;	

    NxWheelShape* wheelShape = NULL;
	wheelShape = static_cast<NxWheelShape *>(actor->createShape(wheelShapeDesc));
	return wheelShape;
}

void Dynamics::updateCars(float delta)
{
	// Look for wheel shapes
	NxU32 nbActors = m_scene->getNbActors();
    NxActor** actors = m_scene->getActors();
    while (nbActors--)
    {
		NxU32 nbShapes = actors[nbActors]->getNbShapes();
	    NxShape*const* shapes = actors[nbActors]->getShapes();
		while (nbShapes--)
		{
			NxShape* shape = shapes[nbShapes];
			if (shape->getType() == NX_SHAPE_WHEEL)
			{
				NxWheelShape* ws = (NxWheelShape*)shape;
				ShapeUserData* sud = (ShapeUserData*)(shape->userData);
				if (sud)
				{
					// Need to save away roll angle in wheel shape user data
					NxF32 rollAngle = sud->wheelShapeRollAngle;
//					rollAngle += ws->getAxleSpeed() * 1.0f/60.0f;
					rollAngle += ws->getAxleSpeed() * delta;
					//rollAngle += ws->getAxleSpeed() * gEngine.kernel->tmr->get() / 60.0f;
					while (rollAngle > NxTwoPi)	//normally just 1x
						rollAngle -= NxTwoPi;
					while (rollAngle < -NxTwoPi)	//normally just 1x
						rollAngle += NxTwoPi;

					// We have the roll angle for the wheel now
					sud->wheelShapeRollAngle = rollAngle;


					NxMat34 pose;
					pose = ws->getGlobalPose();

					NxWheelContactData wcd;
					NxShape* s = ws->getContact(wcd);

					NxReal r = ws->getRadius();
					NxReal st = ws->getSuspensionTravel();
					NxReal steerAngle = ws->getSteerAngle();

//					NxWheelShapeDesc state;
//					ws->saveToDesc(state);

					NxVec3 p0;
					NxVec3 dir;
					/*
					getWorldSegmentFast(seg);
					seg.computeDirection(dir);
					dir.normalize();*/
					
					p0 = pose.t;  //cast from shape origin
					pose.M.getColumn(1, dir);
					dir = -dir;	//cast along -Y.
					NxReal castLength = r + st;	//cast ray this long

//					renderer.addArrow(p0, dir, castLength, 1.0f);
	
					//have ground contact?
					// This code is from WheelShape.cpp in SDKs/core/common/src
					// if (contactPosition != NX_MAX_REAL)  
					if (s && wcd.contactForce > -1000)
					{
						NxVec3 temp = pose.t;
//						pose.t = p0 + dir * wcd.contactPoint;
//						pose.t -= dir * state.radius;	//go from contact pos to center pos.
						pose.t = wcd.contactPoint;
						pose.t -= dir * r;	//go from contact pos to center pos.
						/*pose.t.x = temp.x;
						pose.t.z = temp.z;*/
					}
					else
					{
						pose.t = p0 + dir * st;
						//pose.t.y += castLength;
					}

					NxMat33 rot, axisRot;
					NxQuat Quat, Quat1, Quat2;				//Quat2 to sta³y kwaternion obracaj¹cy ko³a wzgledem Y o 90 stopni
															//(nie wiem czemu tak sie dzieje i po co to jest)....
					Quat1.x = sin(steerAngle/2) * 0;		///kwaternion obrotu samochodu * oœ Y
					Quat1.y = sin(steerAngle/2) * 1;
					Quat1.z = sin(steerAngle/2) * 0;
					Quat1.w = cos(steerAngle/2);

					Quat2.fromAngleAxisFast(1.57f, NxVec3(0, 1, 0));
					sud->vehicleActor->getGlobalPose().M.toQuat(Quat);
					rot.fromQuat(Quat*Quat1*Quat2);
					//rot.multiply(rot, sud->vehicleActor->getGlobalPose().M);
					axisRot.rotY(0);

//					NxReal rollAngle = ((ShapeUserData*)(wheel->userData))->rollAngle;

					NxMat33 rollRot;
					rollRot.rotX(rollAngle);

					pose.M = rot * /*pose.M **/ axisRot * rollRot;

					sud->wheelShapePose = pose;
				}
			}
		}
	}
}
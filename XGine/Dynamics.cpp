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
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	m_physicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, NULL, desc, &errorCode);
	if(m_physicsSDK == NULL) 
	{
		gEngine.kernel->log->prnEx("PhysicsSDK Create error (%d - %s).", errorCode, getNxSDKCreateError(errorCode));
		return 0;
	}

	m_physicsSDK->setParameter(NX_CONTINUOUS_CD, true);
	m_physicsSDK->setParameter(NX_CCD_EPSILON, 0.01f);
	m_physicsSDK->setParameter(NX_SKIN_WIDTH, 0.0025f);

	// Initialize CookingSDK
	m_cooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	if(m_cooking == NULL) 
	{
		gEngine.kernel->log->prn("CookingLib Create error.");
		return 0;
	}
	if(!m_cooking->NxInitCooking())
	{
		gEngine.kernel->log->prn("CookingLib initialization error.");
		return 0;
	}

	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= NxVec3(0.0f, -9.81f, 0.0f);
	m_scene = m_physicsSDK->createScene(sceneDesc);
	if(m_scene == NULL) 
	{
		gEngine.kernel->log->prn("Unable to create a PhysX scene.");
		return 0;
	}

	// Set default material
	NxMaterial* defaultMaterial = m_scene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.8f);
	defaultMaterial->setDynamicFriction(0.5f);

	// Create ground plane
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	m_scene->createActor(actorDesc);

	return 1;
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
	BoundingBox m_boundingBox;
	m_boundingBox = meshPtr->getBoundingBox();
	NxActor *m_actor;

	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
	if(initialVel) bodyDesc.linearVelocity = (NxVec3&)*initialVel;
	
	NxActorDesc actorDesc;
	actorDesc.body			= (staticActor)?(0):(&bodyDesc);
	actorDesc.density		= (staticActor)?(0):(density);
	actorDesc.globalPose.setColumnMajor44((const NxF32*)matWorld);

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
			}break;
		default:
			gEngine.kernel->log->prn("Unknown CollisionType value. Collision shape has not been added.");
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

void Dynamics::startSimulation()
{
	// Srart simulation (non-blocking)
	m_scene->simulate(gEngine.kernel->tmr->get() / 60.0f);
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
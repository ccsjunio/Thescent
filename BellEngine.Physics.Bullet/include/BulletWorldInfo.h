#pragma once
#include "../physics/bullet/btBulletCollisionCommon.h"
#include "../physics/bullet/btBulletDynamicsCommon.h"

struct BulletWorldInfo
{
public:
	btDiscreteDynamicsWorld* dynamicsWorld;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
};
#pragma once
#include "../physics/bullet/btBulletCollisionCommon.h"
#include "../physics/bullet/btBulletDynamicsCommon.h"

class IBulletRigidBodyWrapper
{
protected:
	IBulletRigidBodyWrapper() {};

public:
	virtual ~IBulletRigidBodyWrapper() {};

	virtual btRigidBody* GetWrappedRigidBody() = 0;
};
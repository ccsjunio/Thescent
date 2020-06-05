#pragma once
#include "../physics/bullet/btBulletCollisionCommon.h"
#include "../physics/bullet/btBulletDynamicsCommon.h"

class IBulletColliderWrapper
{
protected:
	IBulletColliderWrapper() {};

public:
	virtual ~IBulletColliderWrapper() {};

	virtual btCollisionShape* GetWrappedShape() = 0;
};
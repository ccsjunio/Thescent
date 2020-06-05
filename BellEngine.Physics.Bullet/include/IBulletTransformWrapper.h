#pragma once
#include "../physics/bullet/btBulletCollisionCommon.h"
#include "../physics/bullet/btBulletDynamicsCommon.h"

class IBulletTransformWrapper
{
protected:
	IBulletTransformWrapper() {};

public:
	virtual ~IBulletTransformWrapper() {};

	virtual btTransform* GetWrappedTransform() = 0;
};
#pragma once
#include "IComponent.h"

class IPhysicsComponent : public IComponent
{
protected:
	IPhysicsComponent() {};

public:
	virtual ~IPhysicsComponent() {};
		
	virtual void ToggleCollisionDetection(const bool& option) = 0;
	virtual const bool AllowCollisionDetection() = 0;
};
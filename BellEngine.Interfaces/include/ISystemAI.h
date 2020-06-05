#pragma once
#include "ISystem.h"
#include <IFactory.h>
#include <Scene.h>

class ISystemAI : public ISystem
{
protected:
	IFactory<IAIBehavior>* behaviorFactory;
	Scene* scene;

	ISystemAI() : scene(nullptr), behaviorFactory(nullptr) {};

public:
	virtual ~ISystemAI() {};
	
	virtual IFactory<IAIBehavior>* GetBehaviorFactory() = 0;
	virtual void SetScene(Scene*& scene) = 0;
};
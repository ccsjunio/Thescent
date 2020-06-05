#pragma once
#include "ISystem.h"
#include "ParameterContainer.h"
#include "IFactory.h"
#include "IScriptedAnimation.h"
#include "IScript.h"
#include <Scene.h>

class ISystemScripting : public ISystem
{
protected:
	ISystemScripting() {};

public:
	virtual ~ISystemScripting() {};

	virtual const bool PlayAnimation(const std::string& name, const std::string& sceneName, const bool& requestedByUserInput = false) = 0;
	virtual IFactory<IScriptedAnimation>* GetScriptedAnimationFactory() = 0;
	virtual IFactory<IScript>* GetScriptFactory() = 0;
	virtual void SetScene(Scene* scene) = 0;
	virtual const bool ProcessMessage(ParameterContainer& input, ParameterContainer& output) = 0;
};
#pragma once
#include <IFactory.h>
#include <IScriptedAnimation.h>

class ScriptedAnimationFactory : public IFactory<IScriptedAnimation>
{
private:
	IScriptedAnimation* CreateLuaScriptedAnimation(ParameterContainer& parameters);

public:
	ScriptedAnimationFactory();
	~ScriptedAnimationFactory();

	IScriptedAnimation* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
};


#include "ScriptedAnimationFactory.h"
#include "LuaScriptedAnimation.h"

IScriptedAnimation* ScriptedAnimationFactory::CreateLuaScriptedAnimation(ParameterContainer& parameters)
{
	Parameter p;

	std::string name;
	if (!parameters.FindParameterByName("AnimationName", p)) return nullptr;
	name = p.value;

	bool isActive = false;
	if (parameters.FindParameterByName("IsActive", p)) isActive = strtoul(p.value.c_str(), NULL, 0) != 0;

	float delay = 0.0f;
	if (parameters.FindParameterByName("Delay", p)) delay = strtof(p.value.c_str(), NULL);

	unsigned int triggerID = 0;
	if (parameters.FindParameterByName("AnimationTrigger", p))
	{
		if (p.value == "immediate")
		{
			triggerID = 0;
		}
		else if (p.value == "event")
		{
			triggerID = 1;
		}
		else
		{
			return nullptr;
		}
	}

	unsigned int typeID = 0;
	if (parameters.FindParameterByName("AnimationType", p)) 
	{
		if (p.value == "parallel")
		{
			typeID = 0;
		}
		else if (p.value == "serial")
		{
			typeID = 1;
		}
		else
		{
			return nullptr;
		}
	}

	LuaScriptedAnimation* animation = new LuaScriptedAnimation();
	animation->SetName(name);
	animation->Activate(isActive);
	animation->SetDelay(delay);
	animation->SetAnimationTypeID(typeID);
	animation->SetTriggerID(triggerID);

	return animation;
}

ScriptedAnimationFactory::ScriptedAnimationFactory()
{
}

ScriptedAnimationFactory::~ScriptedAnimationFactory()
{
}

IScriptedAnimation* ScriptedAnimationFactory::Create(ParameterContainer& parameters)
{
	Parameter p;

	if (parameters.FindParameterByName("Type", p))
	{
		if (p.value == "LuaScriptedAnimation")
		{
			return CreateLuaScriptedAnimation(parameters);
		}
	}

	return nullptr;
}

const bool ScriptedAnimationFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return false;
}

#include "LuaScriptedAnimation.h"
#include <BellEngineID.h>

LuaScriptedAnimation::LuaScriptedAnimation()
{
	this->isActive = true;
	delay = 0.0f;
	trigger = AnimationTrigger_e::Immediate;
	type = AnimationType_e::Parallel;
}

LuaScriptedAnimation::~LuaScriptedAnimation()
{
	Dispose();
}

const std::string LuaScriptedAnimation::GetName()
{
	return this->name;
}

void LuaScriptedAnimation::SetName(const std::string& name)
{
	this->name = name;
}

const bool LuaScriptedAnimation::IsActive()
{
	return this->isActive.load();
}

void LuaScriptedAnimation::Activate(const bool& option)
{
	this->isActive.store(option);
}

const float LuaScriptedAnimation::GetDelay()
{
	return this->delay;
}

void LuaScriptedAnimation::SetDelay(const float& delay)
{
	this->delay = delay;
}

const unsigned int LuaScriptedAnimation::GetTriggerID()
{
	return (unsigned int)trigger;
}

void LuaScriptedAnimation::SetTriggerID(const unsigned int& triggerID)
{
	this->trigger = (LuaScriptedAnimation::AnimationTrigger_e)triggerID;
}

const unsigned int LuaScriptedAnimation::GetAnimationTypeID()
{
	return (unsigned int)this->type;
}

void LuaScriptedAnimation::SetAnimationTypeID(const unsigned int& typeID)
{
	this->type = (LuaScriptedAnimation::AnimationType_e)typeID;
}

const unsigned long long LuaScriptedAnimation::GetTypeID()
{
	return ENGINE_OBJ_ID_SCP_LUANIMATION;
}

const std::string LuaScriptedAnimation::GetType()
{
	return "LuaScriptedAnimation";
}

const bool LuaScriptedAnimation::AddScript(IScript* script)
{
	if (std::find(scripts.begin(), scripts.end(), script) == scripts.end())
	{
		scripts.push_back(script);
		return true;
	}

	return false;
}

const bool LuaScriptedAnimation::RemoveScript(IScript* script, const bool& deletePtr)
{
	auto it = std::find(scripts.begin(), scripts.end(), script);
	if (it == scripts.end())
	{
		scripts.erase(it);

		if (deletePtr)
		{
			DisposePointer(script);
		}

		return true;
	}

	return false;
}

const bool LuaScriptedAnimation::GetScripts(std::vector<IScript*>& scripts)
{
	scripts = this->scripts;
	return !scripts.empty();
}

ICloneable* LuaScriptedAnimation::Clone()
{
	LuaScriptedAnimation* clone = new LuaScriptedAnimation();
	clone->delay = delay;
	clone->trigger = trigger;
	clone->isActive.store(isActive.load());
	clone->scripts = scripts;
	clone->SetName(GetName());
	clone->type = type;

	return clone;
}

void LuaScriptedAnimation::Dispose()
{
	DisposeVector(scripts);
}

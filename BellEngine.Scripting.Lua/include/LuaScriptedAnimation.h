#pragma once
#include <IScriptedAnimation.h>
#include <IScript.h>
#include <atomic>

class LuaScriptedAnimation : public IScriptedAnimation
{
private:
	enum class AnimationType_e;
	enum class AnimationTrigger_e;

	std::string name;
	std::atomic<bool> isActive;
	float delay;
	LuaScriptedAnimation::AnimationTrigger_e trigger;
	LuaScriptedAnimation::AnimationType_e type;
	std::vector<IScript*> scripts;

public:
	enum class AnimationType_e
	{
		/*An animation where all of its scripts will be fired at the same time*/
		Parallel,
		/*An animation where all of its scripts will be fired one after the other, sequentially.*/
		Serial
	};
	enum class AnimationTrigger_e
	{
		/*An animation that starts immediatelly after the user request.*/
		Immediate,
		/*An animation that starts when an event happens in the system. Eg. triggered by another animation or a collision*/
		Event
	};

	LuaScriptedAnimation();
	~LuaScriptedAnimation();

	const std::string GetName();
	void SetName(const std::string& name);
	const bool IsActive();
	void Activate(const bool& option = true);
	const float GetDelay();
	void SetDelay(const float& delay);
	const unsigned int GetTriggerID();
	void SetTriggerID(const unsigned int& triggerID);
	const unsigned int GetAnimationTypeID();
	void SetAnimationTypeID(const unsigned int& typeID);
	const unsigned long long GetTypeID();
	const std::string GetType();
	const bool AddScript(IScript* script);
	const bool RemoveScript(IScript* script, const bool& deletePtr = true);
	const bool GetScripts(std::vector<IScript*>& scripts);
	ICloneable* Clone();
	void Dispose();
};


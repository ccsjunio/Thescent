#pragma once
#include <vector>
#include <Script.h>
#include <IIdentifiable.h>

class ScriptedAnimation : public IIdentifiable
{
private:

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

	ScriptedAnimation::AnimationTrigger_e trigger;
	ScriptedAnimation::AnimationType_e type;
	float delay;
	std::vector<Script*> scripts;

	ScriptedAnimation();
	~ScriptedAnimation();

	const std::string GetType();
};
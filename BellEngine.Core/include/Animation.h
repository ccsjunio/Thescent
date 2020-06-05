#pragma once
#include <vector>
#include <Script.h>
#include <IIdentifiable.h>

class Animation : public IIdentifiable
{
private:
	std::string name;

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

	Animation::AnimationTrigger_e trigger;
	Animation::AnimationType_e type;
	float delay;
	std::vector<Script*> scripts;
	
	Animation();
	~Animation();

	const unsigned long long GetTypeID();
	const std::string GetType();
	const std::string GetName();
	void SetName(const std::string& name);
};


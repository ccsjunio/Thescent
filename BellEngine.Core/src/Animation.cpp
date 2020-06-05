#include "Animation.h"
#include <BellEngineID.h>

Animation::Animation()
{
	delay = 0.0f;
	trigger = Animation::AnimationTrigger_e::Immediate;
	type = Animation::AnimationType_e::Parallel;
}

Animation::~Animation()
{
	if (scripts.size() > 0)
	{
		for (unsigned int i = 0; i < scripts.size(); i++)
		{
			if (scripts[i])
			{
				delete (scripts[i]);
			}
		}
	}
}

const std::string Animation::GetType()
{
	return "Animation";
}

const std::string Animation::GetName()
{
	return this->name;
}

void Animation::SetName(const std::string& name)
{
	this->name = name;
}

const unsigned long long Animation::GetTypeID()
{
	return ENGINE_OBJ_ID_SCP_ANIMATION;
}
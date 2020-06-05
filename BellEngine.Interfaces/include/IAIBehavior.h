#pragma once
#include "IComponent.h"
#include "ICloneable.h"
#include "IUpdatable.h" 
#include "ParameterContainer.h"
#include <functional>
#include <Entity.h>

class IAIBehavior : public IComponent, public IUpdatable
{
protected:
	IAIBehavior() {};

public:
	struct AIEvent_s
	{
		std::string eventDescription;		
		IAIBehavior* ai;
	};

	virtual ~IAIBehavior() {};

	virtual void BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)>) = 0;
	virtual const bool RunCommand(ParameterContainer& input, ParameterContainer& output) = 0;	
	
};
#pragma once
#include "IComponent.h"
#include <vector>
#include "IScript.h"

class IScriptedAnimation : public IComponent
{
protected:	
	IScriptedAnimation() {};

public:
	virtual ~IScriptedAnimation() {};

	virtual const float GetDelay() = 0;
	virtual void SetDelay(const float& delay) = 0;
	virtual const unsigned int GetTriggerID() = 0;
	virtual void SetTriggerID(const unsigned int& triggerID) = 0;
	virtual const unsigned int GetAnimationTypeID() = 0;
	virtual void SetAnimationTypeID(const unsigned int& typeID) = 0;
	virtual const bool AddScript(IScript* script) = 0;
	virtual const bool RemoveScript(IScript* script, const bool& deletePtr = true) = 0;
	virtual const bool GetScripts(std::vector<IScript*>& scripts) = 0;
};
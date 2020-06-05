#pragma once
#include "IExtendedInfo.h"
#include "IUpdatable.h"

class ISystem : public IExtendedInfo, public IUpdatable
{
protected:
	ISystem() {};

public:
	virtual ~ISystem() {};

	virtual const bool IsActive() = 0;
	virtual void Activate(const bool& value = true) = 0;
};
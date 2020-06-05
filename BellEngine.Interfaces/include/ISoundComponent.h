#pragma once
#include "IComponent.h"

class ISoundComponent : public IComponent
{
protected:
	ISoundComponent() {};

public:
	virtual ~ISoundComponent() {};

	virtual void SetPath(const std::string& path) = 0;
	virtual const std::string GetPath() = 0;
	virtual void SetFlags(const unsigned long long& flags) = 0;
	virtual const unsigned long long GetFlags() = 0;
};
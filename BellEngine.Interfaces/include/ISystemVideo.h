#pragma once
#include "IFactory.h"
#include "IShaderProgram.h"
#include "ISystem.h"

class ISystemVideo : public ISystem
{
protected:
	ISystemVideo() {};

public:
	virtual ~ISystemVideo() { };

	virtual void SetSystemFlags(const unsigned long long& flags) = 0;
};
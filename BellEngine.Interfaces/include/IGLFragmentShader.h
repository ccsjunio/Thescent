#pragma once
#include "IGLShader.h"

class IGLFragmentShader : public IGLShader
{
protected:
	IGLFragmentShader() {};

	virtual const bool GetUniformLocations() = 0;

public:
	virtual ~IGLFragmentShader() {};
};
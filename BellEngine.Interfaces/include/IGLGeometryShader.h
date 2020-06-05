#pragma once
#include "IGLShader.h"

class IGLGeometryShader : public IGLShader
{
protected:
	IGLGeometryShader() {};

	virtual const bool GetUniformLocations() = 0;

public:
	virtual ~IGLGeometryShader() {};
};
#pragma once
#include "IGLShader.h"
#include <ModelDef.h>

class IGLVertexShader : public IGLShader
{
protected:
	IGLVertexShader() {};

	virtual const bool GetUniformLocations() = 0;

public:
	virtual ~IGLVertexShader() {};

	virtual void SetRenderMode(const int& value) = 0;
	virtual void SetDrawInfo(const ModelDrawInfo& mdi) = 0;
	virtual void Draw() = 0;
};
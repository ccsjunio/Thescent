#pragma once
#include <IIdentifiable.h>

class IGLShader : public IIdentifiable
{
protected:
	IGLShader() {};

	virtual const bool GetUniformLocations() = 0;

public:
	virtual ~IGLShader() {};

	virtual const bool Initialize() = 0;
	virtual void SetProgramID(const unsigned int& programID) = 0;	
};
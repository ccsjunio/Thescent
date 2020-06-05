#pragma once
#include <string>
#include <map>
#include <Parameter.h>
#include <ICloneable.h>
#include <ParameterContainer.h>
#include <atomic>

class ShaderFeature : public ICloneable
{	
private:
	std::atomic<bool> isActive;
	std::string shaderType;
	std::string name;

public:
	ParameterContainer parameters;

	ShaderFeature();
	ShaderFeature(const std::string& name, const std::string& shaderType);
		
	void SetName(const std::string& name);
	const std::string GetName();
	const std::string GetShaderType();
	void SetShaderType(const std::string& type);
	const bool IsActive();
	void Activate(const bool& value);
	ICloneable* Clone();
};


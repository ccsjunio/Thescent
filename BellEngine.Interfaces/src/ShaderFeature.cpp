#include "ShaderFeature.h"

ShaderFeature::ShaderFeature()
{
	this->isActive.store(true);
}

ShaderFeature::ShaderFeature(const std::string& name, const std::string& shaderType)
{	
	this->name = name;
	this->shaderType = shaderType;
	this->isActive.store(true);
}

void ShaderFeature::SetName(const std::string& name)
{
	this->name = name;
}

const std::string ShaderFeature::GetName()
{
	return this->name;
}

const std::string ShaderFeature::GetShaderType()
{
	return this->shaderType;
}

void ShaderFeature::SetShaderType(const std::string& type)
{
	this->shaderType = type;
}

const bool ShaderFeature::IsActive()
{
	return isActive.load();
}

void ShaderFeature::Activate(const bool& value)
{
	this->isActive.store(value);
}

ICloneable* ShaderFeature::Clone()
{
	ShaderFeature* sf = new ShaderFeature();
	sf->name = name;
	sf->parameters = parameters;
	sf->shaderType = shaderType;
	sf->isActive.store(isActive.load());

	return sf;
}

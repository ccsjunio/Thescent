/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#include "ShaderProgram.h"
#include <BellEngineID.h>

ShaderProgram::ShaderProgram()
{
	id = 0;
}

ShaderProgram::~ShaderProgram()
{
}

const unsigned long long ShaderProgram::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_SHADERPROGRAM;
}

void ShaderProgram::SetID(unsigned int& id)
{
	this->id = id;
}

const std::string ShaderProgram::GetName()
{
    return this->name;
}

void ShaderProgram::SetName(const std::string& name)
{
    this->name = name;
}

unsigned int ShaderProgram::GetID()
{
	return id;
}

const std::string ShaderProgram::GetType()
{
	return "ShaderProgram";
}

void ShaderProgram::Dispose()
{
	DisposeVector(shaders);
}

const bool ShaderProgram::AddShader(IShader*& value)
{
	auto it = std::find(shaders.begin(), shaders.end(), value);
	if (it == shaders.end())
	{
		shaders.push_back(value);
		return true;
	}

	return false;
}

const bool ShaderProgram::RemoveShader(IShader*& value, const bool& deletePtr)
{
	auto it = std::find(shaders.begin(), shaders.end(), value);
	if (it != shaders.end())
	{
		shaders.erase(it);
		
		if (deletePtr)
		{
			DisposePointer(value);
		}

		return true;
	}

	return false;
}

const bool ShaderProgram::GetShaders(std::vector<IShader*>& value)
{
	value = this->shaders;
	return !value.empty();
}

IShader* ShaderProgram::FindShaderByName(const std::string& value)
{
	auto it = std::find_if(shaders.begin(), shaders.end(), [value](IShader*& s) { return s->GetName() == value; });
	if (it != shaders.end())
	{
		return *it;
	}

	return nullptr;
}

const bool ShaderProgram::HasFeatureSupport(const std::string& value)
{
	return std::find(featureSupport.begin(), featureSupport.end(), value) != featureSupport.end();
}

const bool ShaderProgram::AddFeatureSupport(const std::string& value)
{
	auto it = std::find(featureSupport.begin(), featureSupport.end(), value);
	if (it == featureSupport.end())
	{
		featureSupport.push_back(value);
		return true;
	}

	return false;
}

const bool ShaderProgram::RemoveFeatureSupport(const std::string& value)
{
	auto it = std::find(featureSupport.begin(), featureSupport.end(), value);
	if (it != featureSupport.end())
	{
		featureSupport.erase(it);
		return true;
	}

	return false;
}

const bool ShaderProgram::AddFBOPass(const std::string& value)
{
	auto it = std::find(fboPasses.begin(), fboPasses.end(), value);
	if (it == fboPasses.end())
	{
		fboPasses.push_back(value);
		return true;
	}

	return false;
}

const bool ShaderProgram::RemoveFBOPass(const std::string& value)
{
	auto it = std::find(fboPasses.begin(), fboPasses.end(), value);
	if (it != fboPasses.end())
	{
		fboPasses.erase(it);
		return true;
	}

	return false;
}

const bool ShaderProgram::HasFBOPass(const std::string& value)
{
	return std::find(fboPasses.begin(), fboPasses.end(), value) != fboPasses.end();
}
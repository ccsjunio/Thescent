#include "../include/GLShaderContainer.h"
#include <GLCommon.h>

GLShaderContainer::GLShaderContainer()
{
}

GLShaderContainer::~GLShaderContainer()
{
	Dispose();
}

const bool GLShaderContainer::AddShaderProgram(IShaderProgram*& value)
{
	if (std::find(programs.begin(), programs.end(), value) == programs.end())
	{
		programs.push_back(value);
		return true;
	}

	return false;
}

const bool GLShaderContainer::RemoveShaderProgram(IShaderProgram*& value, const bool& deletePtr)
{
	auto it = std::find(programs.begin(), programs.end(), value);
	if (it != programs.end())
	{
		programs.erase(it);

		if (deletePtr)
		{
			delete value;
			value = nullptr;
		}

		return true;
	}

	return false;
}

const bool GLShaderContainer::GetShaderPrograms(std::vector<IShaderProgram*>& value)
{
	value = this->programs;
	return !value.empty();
}

IShaderProgram* GLShaderContainer::FindShaderProgramByName(const std::string& value)
{
	auto it = std::find_if(programs.begin(), programs.end(), [value](IShaderProgram*& program) {return program->GetName() == value; });
	if (it != programs.end())
	{
		return *it;
	}

	return nullptr;
}

IShaderProgram* GLShaderContainer::FindShaderProgramByFeature(const std::string& value)
{
	auto it = std::find_if(programs.begin(), programs.end(), [value](IShaderProgram*& program) { return program->HasFeatureSupport(value); });
	if (it != programs.end())
	{
		return *it;
	}

	return nullptr;
}

IShaderProgram* GLShaderContainer::FindShaderProgramByIndex(const size_t& value)
{
	if (programs.size() > value)
	{
		return programs[value];
	}

	return nullptr;
}

IShaderProgram* GLShaderContainer::FindShaderProgramByAllFeatures(const std::vector<ShaderFeature*>& value)
{
	bool hasAllFeatures = false;
	for (auto& program : programs)
	{		
		for (auto& sf : value)
		{
			hasAllFeatures = program->HasFeatureSupport(sf->GetName());
			if (!hasAllFeatures)
			{
				break;
			}
		}

		if (hasAllFeatures)
		{
			return program;
		}
	}

	return nullptr;
}

void GLShaderContainer::Dispose()
{
	for (auto& program : programs)
	{
		glDeleteProgram(program->GetID());
	}
	DisposeVector(programs);
}

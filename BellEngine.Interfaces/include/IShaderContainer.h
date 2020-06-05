#pragma once
#include <vector>
#include <IShader.h>
#include <IShaderProgram.h>
#include <ShaderFeature.h>

class IShaderContainer
{
protected:
	IShaderContainer() {};

public:
	virtual ~IShaderContainer() {};

	virtual const bool AddShaderProgram(IShaderProgram*& value) = 0;
	virtual const bool RemoveShaderProgram(IShaderProgram*& value, const bool& deletePtr = true) = 0;
	virtual const bool GetShaderPrograms(std::vector<IShaderProgram*>& value) = 0;
	virtual IShaderProgram* FindShaderProgramByName(const std::string& value) = 0;
	virtual IShaderProgram* FindShaderProgramByFeature(const std::string& value) = 0;
	virtual IShaderProgram* FindShaderProgramByIndex(const size_t& value) = 0;
	virtual IShaderProgram* FindShaderProgramByAllFeatures(const std::vector<ShaderFeature*>& value) = 0;
};
#pragma once
#include <IShaderContainer.h>
#include <IDisposable.h>

class GLShaderContainer : public IShaderContainer, public IDisposable
{
private:
	std::vector<IShaderProgram*> programs;

public:
	GLShaderContainer();
	~GLShaderContainer();

	const bool AddShaderProgram(IShaderProgram*& value);
	const bool RemoveShaderProgram(IShaderProgram*& value, const bool& deletePtr = true);
	const bool GetShaderPrograms(std::vector<IShaderProgram*>& value);
	IShaderProgram* FindShaderProgramByName(const std::string& value);
	IShaderProgram* FindShaderProgramByFeature(const std::string& value);
	IShaderProgram* FindShaderProgramByIndex(const size_t& value);
	IShaderProgram* FindShaderProgramByAllFeatures(const std::vector<ShaderFeature*>& value);

	void Dispose();
};


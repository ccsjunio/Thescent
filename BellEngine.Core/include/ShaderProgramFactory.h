#pragma once
#include <IFactory.h>
#include <IShaderProgram.h>

class ShaderProgramFactory : public IFactory<IShaderProgram>
{
private:
	IShaderProgram* CreateShaderProgram(ParameterContainer& parameters);

public:
	ShaderProgramFactory();
	~ShaderProgramFactory();

	IShaderProgram* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
};


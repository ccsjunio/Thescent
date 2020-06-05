#pragma once
#include <IShaderContainer.h>
#include <IFactory.h>
#include <ShaderCompiler.h>
#include <map>
class ShaderContainerFactory : public IFactory<IShaderContainer>
{
private:
	struct ShaderProgramDef
	{
		std::string name;
		std::vector<std::pair<unsigned long long /*TypeID*/, std::string /*Path*/>> shaders;
		std::vector<std::string> availableShaderFeatures, availableFBOPasses;
	};

	ShaderCompiler* shaderCompiler;
	
	IShaderContainer* CreateGLShaderContainer(ParameterContainer& parameters);
	IShader* CreateShader(ParameterContainer& parameters);
	IShaderProgram* CreateShaderProgram(ParameterContainer& parameters);

public:
	ShaderContainerFactory();
	~ShaderContainerFactory();

	IShaderContainer* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
};


#pragma once
#include <IFileReader.h>
#include <IFactory.h>
#include <IShaderContainer.h>

class ShaderFileReader : public IFileReader
{
private:
	IFactory<IShaderContainer>* containerFactory;

	const bool ReadShaderContainer(ParameterContainer& input, ParameterContainer& output);

public:

	ShaderFileReader();
	~ShaderFileReader();

	IShaderContainer* Create(ParameterContainer& parameters);

	const bool ReadToParameterContainer(ParameterContainer& input, ParameterContainer& output);
};


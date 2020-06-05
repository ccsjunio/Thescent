#pragma once
#include <IGLFragmentShader.h>
#include <GLCommon.h>
#include <GLMCommon.h>
#include <vector>
#include "LightShaderStruct.h"
#include <ILight.h>

class ColorizeFragmentShader : public IGLFragmentShader
{
private:	
	std::string name;
	GLint diffuseColour_UL;
	GLint specularColour_UL;
	GLint isLit_ul;
	unsigned int programID;
	
	GLint maxLightCount;
	std::vector<LightShaderStruct> lightsUL;

	const bool GetUniformLocations();

public:
	ColorizeFragmentShader();
	~ColorizeFragmentShader();

	const bool SetLights(const std::vector<ILight*>& lights);

	const unsigned long long GetTypeID();
	const std::string GetType();
	const std::string GetName();
	void SetName(const std::string& name);

	const bool Initialize();
	void SetProgramID(const unsigned int& programID);

	const bool SetIsLit(const bool& value);
	const bool SetDiffuse(const Vector4D& value);
	const bool SetSpecular(const Vector4D& value);
};


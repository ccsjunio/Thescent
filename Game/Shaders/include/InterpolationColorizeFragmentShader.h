#pragma once
#include <IGLFragmentShader.h>
#include <GLCommon.h>
#include <GLMCommon.h>
#include <vector>
#include "LightShaderStruct.h"
#include <ILight.h>

class InterpolationColorizeFragmentShader : public IGLFragmentShader
{
private:
	std::string name;
	GLint colorA_ul, colorB_ul, specularColour_UL, deltaTime_ul, runTime_ul;
	unsigned int programID;

	GLint maxLightCount;
	std::vector<LightShaderStruct> lightsUL;

	const bool GetUniformLocations();

public:
	InterpolationColorizeFragmentShader();
	~InterpolationColorizeFragmentShader();

	const unsigned long long GetTypeID();
	const std::string GetType();
	const std::string GetName();
	void SetName(const std::string& name);

	const bool Initialize();
	void SetProgramID(const unsigned int& programID);

	const bool SetLights(const std::vector<ILight*>& lights);
	const bool SetDeltaTime(const double& value);
	const bool SetRunTime(const double& value);
	const bool SetColorA(const Vector4D& value);
	const bool SetColorB(const Vector4D& value);
	const bool SetSpecular(const Vector4D& value);
};


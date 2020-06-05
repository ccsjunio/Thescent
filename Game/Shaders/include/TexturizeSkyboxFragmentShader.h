#pragma once
#include <IGLFragmentShader.h>
#include <GLCommon.h>
#include <GLMCommon.h>
#include <ITexture.h>

class TexturizeSkyboxFragmentShader : public IGLFragmentShader
{
private:
	std::string name;
	GLint skyboxTexture_ul;

	unsigned int programID;

	const bool GetUniformLocations();

public:
	TexturizeSkyboxFragmentShader();
	~TexturizeSkyboxFragmentShader();

	const unsigned long long GetTypeID();
	const std::string GetType();
	const std::string GetName();
	void SetName(const std::string& name);

	const bool Initialize();
	void SetProgramID(const unsigned int& programID);

	const bool SetSkyboxTexture(ITexture*& value);
};

#pragma once
#include <IGLFragmentShader.h>
#include <GLCommon.h>
#include <GLMCommon.h>
#include <vector>
#include <ITexture.h>
#include "LightShaderStruct.h"
#include <ILight.h>

class ImposterFragmentShaderV2 : public IGLFragmentShader
{
private:
	std::string name;

	GLint diffuseColour_UL, specularColour_UL, textureCount_ul, transparency_ul, uvoffset_ul, tiling_ul;
	std::vector<GLint> textureSamples_ul;

	GLint maxTextureCount;

	unsigned int programID;

	const bool GetUniformLocations();

public:
	ImposterFragmentShaderV2();
	~ImposterFragmentShaderV2();

	const unsigned long long GetTypeID();
	const std::string GetType();
	const std::string GetName();
	void SetName(const std::string& name);

	const bool Initialize();
	void SetProgramID(const unsigned int& programID);

	const bool SetLights(const std::vector<ILight*>& lights);
	const bool SetDiffuse(const Vector4D& value);
	const bool SetSpecular(const Vector4D& value);
	const bool SetTextures(const std::vector<ITexture*>& textures);
	const bool SetUVOffset(const Vector2D& value);
	const bool SetTiling(const Vector2D& value);
	const bool SetTransparency(const int& value);
};

#include "../include/ImposterFragmentShaderV2.h"
#include <BellEngineID.h>
#include <LightPoint.h>
#include <LightDirectional.h>
#include <LightSpot.h>

const bool ImposterFragmentShaderV2::GetUniformLocations()
{
	this->diffuseColour_UL = glGetUniformLocation(this->programID, "diffuseColour");
	this->specularColour_UL = glGetUniformLocation(this->programID, "specularColor");
	this->textureCount_ul = glGetUniformLocation(this->programID, "textureCount");
	this->transparency_ul = glGetUniformLocation(this->programID, "transparency");
	this->uvoffset_ul = glGetUniformLocation(this->programID, "uvoffset");
	this->tiling_ul = glGetUniformLocation(this->programID, "tiling");

	this->textureSamples_ul.resize(this->maxTextureCount);
	char textureSampleName[100];
	for (size_t i = 0; i < this->maxTextureCount; i++)
	{
		sprintf_s(textureSampleName, 100, "%s[%d]", "textureSample", i);
		textureSamples_ul[i] = glGetUniformLocation(this->programID, textureSampleName);
	}

	return true;
}

ImposterFragmentShaderV2::ImposterFragmentShaderV2()
{
	this->tiling_ul =
		this->transparency_ul =
		this->uvoffset_ul =
		this->textureCount_ul =
		this->programID =
		this->diffuseColour_UL =
		this->specularColour_UL = 0;

	this->maxTextureCount = 30;
}

ImposterFragmentShaderV2::~ImposterFragmentShaderV2()
{

}

const bool ImposterFragmentShaderV2::SetLights(const std::vector<ILight*>& lights)
{
	/*No lights in this shader*/
	return true;
}

const unsigned long long ImposterFragmentShaderV2::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_GLSHADER_FRAGMENT_IMPOSTER;
}

const std::string ImposterFragmentShaderV2::GetType()
{
	return "ImposterFragmentShaderV2";
}

const std::string ImposterFragmentShaderV2::GetName()
{
	return this->name;
}

void ImposterFragmentShaderV2::SetName(const std::string& name)
{
	this->name = name;
}

const bool ImposterFragmentShaderV2::Initialize()
{
	bool rc = true;

	GetUniformLocations();

	return rc;
}

void ImposterFragmentShaderV2::SetProgramID(const unsigned int& programID)
{
	glUseProgram(programID);

	if (this->programID != programID)
	{
		this->programID = programID;
		GetUniformLocations();
	}

	this->programID = programID;
}

const bool ImposterFragmentShaderV2::SetDiffuse(const Vector4D& value)
{
	glUniform4f(this->diffuseColour_UL, value.r, value.g, value.b, value.a);
	return true;
}

const bool ImposterFragmentShaderV2::SetSpecular(const Vector4D& value)
{
	glUniform4f(this->specularColour_UL, value.r, value.g, value.b, value.a);
	return true;
}

const bool ImposterFragmentShaderV2::SetTextures(const std::vector<ITexture*>& textures)
{
	//Tells the shader how many textures this gameobject supports
	size_t texturesSz = textures.size();
	if (texturesSz > this->maxTextureCount)
	{
		/*Cannot have more than the max amount of textures!*/
		return false;
	}

	glUniform1i(this->textureCount_ul, (GLint)texturesSz);

	//Textures
	for (size_t i = 0; i < texturesSz; i++)
	{
		ITexture* texture = textures[i];

		glActiveTexture(GL_TEXTURE0 + (GLint)i);			// Texture Unit x, max is 32
		glBindTexture(GL_TEXTURE_2D, texture->GetID());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glUniform1i(textureSamples_ul[i], (GLint)i);
	}
	//------------------------------------------------------------------------

	glCullFace(GL_BACK);

	return true;
}

const bool ImposterFragmentShaderV2::SetUVOffset(const Vector2D& value)
{
	glUniform2f(this->uvoffset_ul, value.x, value.y);
	return true;
}

const bool ImposterFragmentShaderV2::SetTiling(const Vector2D& value)
{
	glUniform2f(this->tiling_ul, value.x, value.y);
	return true;
}

const bool ImposterFragmentShaderV2::SetTransparency(const int& value)
{
	glUniform1i(this->transparency_ul, value);
	return true;
}

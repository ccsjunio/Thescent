#include "..\include\TexturizeSkyboxFragmentShader.h"
#include <BellEngineID.h>

const bool TexturizeSkyboxFragmentShader::GetUniformLocations()
{
	this->skyboxTexture_ul = glGetUniformLocation(this->programID, "textureProcessInfo.skybox");

	return true;
}

TexturizeSkyboxFragmentShader::TexturizeSkyboxFragmentShader()
{
	programID = skyboxTexture_ul = 0;
}

TexturizeSkyboxFragmentShader::~TexturizeSkyboxFragmentShader()
{

}

const unsigned long long TexturizeSkyboxFragmentShader::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_GLSHADER_FRAGMENT_TEXTURIZE_SKYBOX;
}

const std::string TexturizeSkyboxFragmentShader::GetType()
{
	return "TexturizeSkyboxFragmentShader";
}

const std::string TexturizeSkyboxFragmentShader::GetName()
{
	return this->name;
}

void TexturizeSkyboxFragmentShader::SetName(const std::string& name)
{
	this->name = name;
}

const bool TexturizeSkyboxFragmentShader::Initialize()
{
	bool rc = true;

	GetUniformLocations();

	return rc;
}

void TexturizeSkyboxFragmentShader::SetProgramID(const unsigned int& programID)
{
	glUseProgram(programID);

	if (this->programID != programID)
	{
		this->programID = programID;
		GetUniformLocations();
	}

	this->programID = programID;
}

const bool TexturizeSkyboxFragmentShader::SetSkyboxTexture(ITexture*& value)
{
	glCullFace(GL_FRONT_AND_BACK);

	glActiveTexture(GL_TEXTURE30);
	glBindTexture(GL_TEXTURE_CUBE_MAP, value->GetID());
	glUniform1i(this->skyboxTexture_ul, 30);

	return true;
}

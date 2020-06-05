#include "../include/ImposterFragmentShader.h"
#include <BellEngineID.h>
#include <LightPoint.h>
#include <LightDirectional.h>
#include <LightSpot.h>

const bool ImposterFragmentShader::GetUniformLocations()
{
	this->diffuseColour_UL = glGetUniformLocation(this->programID, "diffuseColour");
	this->specularColour_UL = glGetUniformLocation(this->programID, "specularColor");
	this->textureCount_ul = glGetUniformLocation(this->programID, "textureProcessInfo.textureCount");
	this->transparency_ul = glGetUniformLocation(this->programID, "textureProcessInfo.transparency");
	this->uvoffset_ul = glGetUniformLocation(this->programID, "textureProcessInfo.uvoffset");
	this->tiling_ul = glGetUniformLocation(this->programID, "textureProcessInfo.tiling");

	this->textureSamples_ul.resize(this->maxTextureCount);
	char textureSampleName[100];
	for (size_t i = 0; i < this->maxTextureCount; i++)
	{
		sprintf_s(textureSampleName, 100, "%s[%d]", "textureProcessInfo.textureSample", i);
		textureSamples_ul[i] = glGetUniformLocation(this->programID, textureSampleName);
	}

	this->lightsUL.resize(this->maxLightCount);
	std::string tempLightProp;
	for (size_t i = 0; i < this->maxLightCount; i++)
	{
		tempLightProp = "lightArray[" + std::to_string(i) + "].position";
		this->lightsUL[i].positionUL = glGetUniformLocation(this->programID, tempLightProp.c_str());

		tempLightProp = "lightArray[" + std::to_string(i) + "].atten";
		this->lightsUL[i].attenUL = glGetUniformLocation(this->programID, tempLightProp.c_str());

		tempLightProp = "lightArray[" + std::to_string(i) + "].direction";
		this->lightsUL[i].directionUL = glGetUniformLocation(this->programID, tempLightProp.c_str());

		tempLightProp = "lightArray[" + std::to_string(i) + "].param1";
		this->lightsUL[i].param1UL = glGetUniformLocation(this->programID, tempLightProp.c_str());

		tempLightProp = "lightArray[" + std::to_string(i) + "].diffuse";
		this->lightsUL[i].diffuseUL = glGetUniformLocation(this->programID, tempLightProp.c_str());

		tempLightProp = "lightArray[" + std::to_string(i) + "].specular";
		this->lightsUL[i].specularUL = glGetUniformLocation(this->programID, tempLightProp.c_str());

		tempLightProp = "lightArray[" + std::to_string(i) + "].param2";
		this->lightsUL[i].param2UL = glGetUniformLocation(this->programID, tempLightProp.c_str());
	}

	return true;
}

ImposterFragmentShader::ImposterFragmentShader()
{
	this->tiling_ul =
		this->transparency_ul =
		this->uvoffset_ul =
		this->textureCount_ul =
		this->programID =
		this->diffuseColour_UL =
		this->specularColour_UL = 0;

	this->maxTextureCount = 30;
	this->maxLightCount = 30;
}

ImposterFragmentShader::~ImposterFragmentShader()
{

}

const bool ImposterFragmentShader::SetLights(const std::vector<ILight*>& lights)
{
	size_t current_light_count = lights.size();
	for (size_t i = 0; i < current_light_count; i++)
	{
		LightShaderStruct lightUL = this->lightsUL[i];
		ILight* light = lights[i];
		light->Update(0.0);

		switch (light->GetTypeID())
		{
		case ENGINE_OBJ_ID_GPH_LIGHTPOINT:
		{
			Vector3D position = ((LightPoint*)light)->GetPosition();
			glUniform4f(lightUL.positionUL, position.x, position.y, position.z, 1.0f);
			glUniform4f(lightUL.directionUL, 0.0f, 0.0f, 0.0f, 0.0f);
			glUniform4f(lightUL.attenUL,
				((LightPoint*)light)->GetConstantAttenuation(),		// Constant attenuation
				((LightPoint*)light)->GetLinearAttenuation(),	// Linear attenuation
				((LightPoint*)light)->GetQuadraticAttenuation(),	// Quadratic attenuation
				((LightPoint*)light)->GetCutoffDistance());		// Distance cut off	
			glUniform4f(lightUL.param1UL, light->GetEnumID(), 0.0f, 0.0f, 0.0f);
		}
		break;
		case ENGINE_OBJ_ID_GPH_LIGHTDIRECTIONAL:
		{
			glUniform4f(lightUL.positionUL, 0.0f, 0.0f, 0.0f, 0.0f);
			Vector3D direction = ((LightDirectional*)light)->GetDirection();
			glUniform4f(lightUL.directionUL, direction.x, direction.y, direction.z, 1.0f);
			glUniform4f(lightUL.attenUL, 0.0f, 0.0f, 0.0f, 0.0f);
			glUniform4f(lightUL.param1UL, light->GetEnumID(), 0.0f, 0.0f, 0.0f);
		}
		break;
		case ENGINE_OBJ_ID_GPH_LIGHTSPOT:
		{
			Vector3D position = ((LightSpot*)light)->GetPosition();
			glUniform4f(lightUL.positionUL, position.x, position.y, position.z, 1.0f);
			Vector3D direction = ((LightSpot*)light)->GetDirection();
			glUniform4f(lightUL.directionUL, direction.x, direction.y, direction.z, 1.0f);
			glUniform4f(lightUL.attenUL,
				((LightSpot*)light)->GetConstantAttenuation(),		// Constant attenuation
				((LightSpot*)light)->GetLinearAttenuation(),	// Linear attenuation
				((LightSpot*)light)->GetQuadraticAttenuation(),	// Quadratic attenuation
				((LightSpot*)light)->GetCutoffDistance());		// Distance cut off
			glUniform4f(lightUL.param1UL, light->GetEnumID(), ((LightSpot*)light)->GetInnerAngle(), ((LightSpot*)light)->GetOuterAngle(), 1.0f);
		}
		break;
		default:
		{
			printf("ImposterFragmentShader: Passing unknown light to the shader\n!\n");
		}
		break;
		}

		Vector4D diffuse = light->GetDiffuse();
		glUniform4f(lightUL.diffuseUL, diffuse.r, diffuse.g, diffuse.b, diffuse.a);
		Vector4D specular = light->GetSpecular();
		glUniform4f(lightUL.specularUL, specular.r, specular.g, specular.b, specular.a);
		glUniform4f(lightUL.param2UL, (light->IsVisible() ? 1.0f : 0.0f), 0.0f, 0.0f, 1.0f);
	}

	return current_light_count != 0;
}

const unsigned long long ImposterFragmentShader::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_GLSHADER_FRAGMENT_IMPOSTER;
}

const std::string ImposterFragmentShader::GetType()
{
	return "ImposterFragmentShader";
}

const std::string ImposterFragmentShader::GetName()
{
	return this->name;
}

void ImposterFragmentShader::SetName(const std::string& name)
{
	this->name = name;
}

const bool ImposterFragmentShader::Initialize()
{
	bool rc = true;

	GetUniformLocations();

	return rc;
}

void ImposterFragmentShader::SetProgramID(const unsigned int& programID)
{
	glUseProgram(programID);

	if (this->programID != programID)
	{
		this->programID = programID;
		GetUniformLocations();
	}

	this->programID = programID;
}

const bool ImposterFragmentShader::SetDiffuse(const Vector4D& value)
{
	glUniform4f(this->diffuseColour_UL, value.r, value.g, value.b, value.a);
	return true;
}

const bool ImposterFragmentShader::SetSpecular(const Vector4D& value)
{
	glUniform4f(this->specularColour_UL, value.r, value.g, value.b, value.a);
	return true;
}

const bool ImposterFragmentShader::SetTextures(const std::vector<ITexture*>& textures)
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

const bool ImposterFragmentShader::SetUVOffset(const Vector2D& value)
{
	glUniform2f(this->uvoffset_ul, value.x, value.y);
	return true;
}

const bool ImposterFragmentShader::SetTiling(const Vector2D& value)
{
	glUniform2f(this->tiling_ul, value.x, value.y);
	return true;
}

const bool ImposterFragmentShader::SetTransparency(const int& value)
{
	glUniform1i(this->transparency_ul, value);
	return true;
}

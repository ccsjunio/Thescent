#include "..\include\ColorizeFragmentShader.h"
#include <BellEngineID.h>
#include <LightPoint.h>
#include <LightDirectional.h>
#include <LightSpot.h>

const bool ColorizeFragmentShader::GetUniformLocations()
{
	this->diffuseColour_UL = glGetUniformLocation(this->programID, "diffuseColour");
	this->specularColour_UL = glGetUniformLocation(this->programID, "specularColor");
	this->isLit_ul = glGetUniformLocation(this->programID, "colorizeInfo.isLit");

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

ColorizeFragmentShader::ColorizeFragmentShader()
{
	this->programID = 
		this->isLit_ul =
		this->diffuseColour_UL =
		this->specularColour_UL = 0;

	this->maxLightCount = 30;
}

ColorizeFragmentShader::~ColorizeFragmentShader()
{

}

const bool ColorizeFragmentShader::SetLights(const std::vector<ILight*>& lights)
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
			printf("TexturizeFragmentShader: Passing unknown light to the shader\n!\n");
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

const unsigned long long ColorizeFragmentShader::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_GLSHADER_FRAGMENT_COLORIZE;
}

const std::string ColorizeFragmentShader::GetType()
{
	return "ColorizeFragmentShader";
}

const std::string ColorizeFragmentShader::GetName()
{
	return this->name;
}

void ColorizeFragmentShader::SetName(const std::string& name)
{
	this->name = name;
}

const bool ColorizeFragmentShader::Initialize()
{
	bool rc = true;

	GetUniformLocations();

	return rc;
}

void ColorizeFragmentShader::SetProgramID(const unsigned int& programID)
{
	glUseProgram(programID);

	if (this->programID != programID)
	{
		this->programID = programID;
		GetUniformLocations();
	}

	this->programID = programID;
}

const bool ColorizeFragmentShader::SetIsLit(const bool& value)
{
	glUniform1i(this->isLit_ul, value);
	return true;
}

const bool ColorizeFragmentShader::SetDiffuse(const Vector4D& value)
{
	glUniform4f(this->diffuseColour_UL, value.r, value.g, value.b, value.a);
	return true;
}

const bool ColorizeFragmentShader::SetSpecular(const Vector4D& value)
{
	glUniform4f(this->specularColour_UL, value.r, value.g, value.b, value.a);
	return true;
}

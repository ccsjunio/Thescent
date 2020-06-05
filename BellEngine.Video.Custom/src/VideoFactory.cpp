#include "VideoFactory.h"
#include <RenderComponent.h>
#include <StringHelpers.h>
#include <ClothRenderComponent.h>
#include <GLCommon.h>
#include <ParticleEmitterComponent.h>
#include <MathHelper.h>
#include <LightPoint.h>
#include <LightSpot.h>
#include <LightDirectional.h>

IComponent* VideoFactory::CreateRenderComponent(ParameterContainer& parameters)
{
	Parameter temp;
	bool rc = false;
		
	rc = parameters.FindParameterByName("RenderMode", temp);
	if (!rc) return nullptr;
	int renderMode = 0;
	std::string visibilityMode = temp.value.c_str();
	if (visibilityMode == "Fill")
	{
		renderMode = (GL_FILL);
	}
	else if (visibilityMode == "Wireframe")
	{
		renderMode = (GL_LINE);
	}
	else
	{
		renderMode = (GL_FILL);
	}

	int alternateRenderMode = 0;
	rc = parameters.FindParameterByName("AlternateRenderMode", temp);
	if (rc) alternateRenderMode = std::stoi(temp.value.c_str(), nullptr, 0);

	rc = parameters.FindParameterByName("IsVisible", temp);
	if (!rc) return nullptr;
	bool isVisible = std::stoi(temp.value.c_str(), nullptr, 0) != 0;
	
	rc = parameters.FindParameterByName("SpecularColor", temp);
	if (!rc) return nullptr;
	Vector4D specular = StringHelpers::StrToVec4(temp.value);

	rc = parameters.FindParameterByName("DiffuseColor", temp);
	if (!rc) return nullptr;
	Vector4D diffuse = StringHelpers::StrToVec4(temp.value);

	RenderComponent* cp = new RenderComponent();
	cp->SetRenderMode(renderMode);
	cp->SetVisibility(isVisible);
	cp->SetSpecular(specular);
	cp->SetDiffuse(diffuse);
	cp->SetAlternateRenderMode(alternateRenderMode);

	return cp;
}

IComponent* VideoFactory::CreateClothRenderComponent(ParameterContainer& parameters)
{
	Parameter temp;
	bool rc = false;

	rc = parameters.FindParameterByName("RenderMode", temp);
	if (!rc) return nullptr;
	int renderMode = 0;
	std::string visibilityMode = temp.value.c_str();
	if (visibilityMode == "Fill")
	{
		renderMode = (GL_FILL);
	}
	else if (visibilityMode == "Wireframe")
	{
		renderMode = (GL_LINE);
	}
	else
	{
		renderMode = (GL_FILL);
	}

	int alternateRenderMode = 0;
	rc = parameters.FindParameterByName("AlternateRenderMode", temp);
	if (rc) alternateRenderMode = std::stoi(temp.value.c_str(), nullptr, 0);

	rc = parameters.FindParameterByName("IsVisible", temp);
	if (!rc) return nullptr;
	bool isVisible = std::stoi(temp.value.c_str(), nullptr, 0) != 0;

	rc = parameters.FindParameterByName("SpecularColor", temp);
	if (!rc) return nullptr;
	Vector4D specular = StringHelpers::StrToVec4(temp.value);

	rc = parameters.FindParameterByName("DiffuseColor", temp);
	if (!rc) return nullptr;
	Vector4D diffuse = StringHelpers::StrToVec4(temp.value);

	ClothRenderComponent* cp = new ClothRenderComponent();
	cp->SetRenderMode(renderMode);
	cp->SetVisibility(isVisible);
	cp->SetSpecular(specular);
	cp->SetDiffuse(diffuse);
	cp->SetAlternateRenderMode(alternateRenderMode);

	return cp;
}

IComponent* VideoFactory::CreateParticleEmitterComponent(ParameterContainer& parameters)
{
	Parameter p;
	bool rc = false;

	ParticleEmitterComponent::ParticleDefinition particleDef;
	{
		rc = parameters.FindParameterByName("Shape", p);
		if (rc)
		{
			if (p.value == "Sphere")
			{
				particleDef.particleCreationVolume = ENGINE_OBJ_ID_PHY_COLLIDERSPHERE;
			}
			else if (p.value == "AABB")
			{
				particleDef.particleCreationVolume = ENGINE_OBJ_ID_PHY_COLLIDERAABB;
			}
			else if (p.value == "Box")
			{
				particleDef.particleCreationVolume = ENGINE_OBJ_ID_PHY_COLLIDERBOX;
			}
		}

		rc = parameters.FindParameterByName("StartRotation", p);
		if (rc) particleDef.startRotation = StringHelpers::StrToVec3(p.value);

		rc = parameters.FindParameterByName("DiffuseColor", p);
		if (rc) particleDef.diffuseColor = StringHelpers::StrToVec4(p.value);

		rc = parameters.FindParameterByName("SpecularColor", p);
		if (rc) particleDef.specularColor = StringHelpers::StrToVec4(p.value);

		rc = parameters.FindParameterByName("MinVelocity", p);
		if (rc) particleDef.minSpeed = StringHelpers::StrToVec3(p.value);

		rc = parameters.FindParameterByName("MaxVelocity", p);
		if (rc) particleDef.maxSpeed = StringHelpers::StrToVec3(p.value);

		rc = parameters.FindParameterByName("MinDeltaPosition", p);
		if (rc) particleDef.minDeltaPosition = StringHelpers::StrToVec3(p.value);

		rc = parameters.FindParameterByName("MaxDeltaPosition", p);
		if (rc) particleDef.maxDeltaPosition = StringHelpers::StrToVec3(p.value);

		rc = parameters.FindParameterByName("MinLifeSeconds", p);
		if (rc) particleDef.minLifeSeconds = strtof(p.value.c_str(), NULL);

		rc = parameters.FindParameterByName("MaxLifeSeconds", p);
		if (rc) particleDef.maxLifeSeconds = strtof(p.value.c_str(), NULL);

		rc = parameters.FindParameterByName("MinParticlesPerFrame", p);
		if (rc) particleDef.minParticlesPerFrame = (int)strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("MaxParticlesPerFrame", p);
		if (rc) particleDef.maxParticlesPerFrame = (int)strtoul(p.value.c_str(), NULL, 0);

		rc = parameters.FindParameterByName("NumParticlesPerSecond", p);
		if (rc) particleDef.numParticlesPerSecond = strtof(p.value.c_str(), NULL);

		rc = parameters.FindParameterByName("MinDeltaAABBPosition", p);
		if (rc) particleDef.minDeltaAABBPosition = StringHelpers::StrToVec3(p.value);

		rc = parameters.FindParameterByName("MaxDeltaAABBPosition", p);
		if (rc) particleDef.maxDeltaAABBPosition = StringHelpers::StrToVec3(p.value);

		rc = parameters.FindParameterByName("MinDeltaRadiusPosition", p);
		if (rc) particleDef.minDeltaRadiusPosition = strtof(p.value.c_str(), NULL);

		rc = parameters.FindParameterByName("MaxDeltaRadiusPosition", p);
		if (rc) particleDef.maxDeltaRadiusPosition = strtof(p.value.c_str(), NULL);

		rc = parameters.FindParameterByName("MinStartingScale", p);
		if (rc) particleDef.minStartingScale = strtof(p.value.c_str(), NULL);

		rc = parameters.FindParameterByName("MaxStartingScale", p);
		if (rc) particleDef.maxStartingScale = strtof(p.value.c_str(), NULL);

		rc = parameters.FindParameterByName("MinScaleChangePercentPerSecond", p);
		if (rc) particleDef.minScaleChangePercentPerSecond = strtof(p.value.c_str(), NULL);

		rc = parameters.FindParameterByName("MaxScaleChangePercentPerSecond", p);
		if (rc) particleDef.maxScaleChangePercentPerSecond = strtof(p.value.c_str(), NULL);

		rc = parameters.FindParameterByName("MinRotationalChangePerSecond", p);
		if (rc)
		{
			Vector3D eulerAngles = StringHelpers::StrToVec3(p.value);
			particleDef.minRotationalChangePerSecond = MathHelper::EulerAnglesToQuaternion(eulerAngles);
		}

		rc = parameters.FindParameterByName("MaxRotationalChangePerSecond", p);
		if (rc)
		{
			Vector3D eulerAngles = StringHelpers::StrToVec3(p.value);
			particleDef.maxRotationalChangePerSecond = MathHelper::EulerAnglesToQuaternion(eulerAngles);
		}

		rc = parameters.FindParameterByName("FadeOutOverLifetime", p);
		if (rc) particleDef.fadeOutOverLifetime = strtoul(p.value.c_str(), NULL, 0) != 0;

		rc = parameters.FindParameterByName("IsImposter", p);
		if (rc) particleDef.isImposter = strtoul(p.value.c_str(), NULL, 0) != 0;
	}
	
	Vector3D acceleration = Vector3D();
	rc = parameters.FindParameterByName("Acceleration", p);
	if (rc) acceleration = StringHelpers::StrToVec3(p.value);

	Vector3D position = Vector3D();
	rc = parameters.FindParameterByName("Position", p);
	if (rc) position = StringHelpers::StrToVec3(p.value);

	std::string name;
	rc = parameters.FindParameterByName("Name", p);
	if (rc) name = p.value;

	bool isActive = false;
	rc = parameters.FindParameterByName("IsActive", p);
	if (rc) isActive = strtoul(p.value.c_str(), NULL, 0) != 0;

	bool allowNewParticles = true;
	rc = parameters.FindParameterByName("AllowNewParticles", p);
	if (rc) allowNewParticles = strtoul(p.value.c_str(), NULL, 0) != 0;

	size_t defaultNumberOfParticles = 100;
	rc = parameters.FindParameterByName("DefaultNumberOfParticles", p);
	if (rc) defaultNumberOfParticles = strtoull(p.value.c_str(), NULL, 0);

	ParticleEmitterComponent* pec = new ParticleEmitterComponent();
	pec->SetName(name);
	pec->SetPosition(position);
	pec->SetAcceleration(acceleration);
	pec->Activate(isActive);
	pec->ToggleParticleCreation(allowNewParticles);
	pec->SetDefaultNumberOfParticles(defaultNumberOfParticles);
	pec->Initialize(particleDef);

	return pec;
}

IComponent* VideoFactory::CreateDirectionalLightComponent(ParameterContainer& parameters)
{
	Parameter p;

	std::string name;
	if (parameters.FindParameterByName("Name", p)) name = p.value;

	bool isVisible = false;
	if (parameters.FindParameterByName("IsVisible", p)) isVisible = strtoul(p.value.c_str(), NULL, 0) != 0;

	Vector3D position = Vector3D();
	if (parameters.FindParameterByName("Position", p)) position = StringHelpers::StrToVec3(p.value);

	Vector4D diffuse = Vector4D();
	if (parameters.FindParameterByName("DiffuseColor", p)) diffuse = StringHelpers::StrToVec4(p.value);

	Vector4D specular = Vector4D();
	if (parameters.FindParameterByName("SpecularColor", p)) specular = StringHelpers::StrToVec4(p.value);

	Vector3D direction = Vector3D();
	if (parameters.FindParameterByName("Direction", p)) direction = StringHelpers::StrToVec3(p.value);

	LightDirectional* light = new LightDirectional();
	light->SetName(name);
	light->SetPosition(position);
	light->SetDiffuse(diffuse);
	light->SetSpecular(specular);
	light->SetVisibility(isVisible);
	light->SetDirection(direction);
	light->SetUp(Vector3D(0.0f, 1.0f, 0.0f));
	light->SetScale(Vector3D(1.0f, 1.0f, 1.0f));
	light->SetRotation(Vector3D(0.0f));
	light->SetOrientation(Vector3D(0.0f, 0.0f, -1.0f));

	return light;
}

IComponent* VideoFactory::CreateSpotLightComponent(ParameterContainer& parameters)
{
	Parameter p;

	std::string name;
	if (parameters.FindParameterByName("Name", p)) name = p.value;

	bool isVisible = false;
	if (parameters.FindParameterByName("IsVisible", p)) isVisible = strtoul(p.value.c_str(), NULL, 0) != 0;

	Vector3D position = Vector3D();
	if (parameters.FindParameterByName("Position", p)) position = StringHelpers::StrToVec3(p.value);

	Vector4D diffuse = Vector4D();
	if (parameters.FindParameterByName("DiffuseColor", p)) diffuse = StringHelpers::StrToVec4(p.value);

	Vector4D specular = Vector4D();
	if (parameters.FindParameterByName("SpecularColor", p)) specular = StringHelpers::StrToVec4(p.value);

	Vector3D direction = Vector3D();
	if (parameters.FindParameterByName("Direction", p)) direction = StringHelpers::StrToVec3(p.value);

	float constAtten = 0.0f;
	if (parameters.FindParameterByName("ConstAttenuation", p)) constAtten = strtof(p.value.c_str(), NULL);

	float linearAtten = 0.0f;
	if (parameters.FindParameterByName("LinearAttenuation", p)) linearAtten = strtof(p.value.c_str(), NULL);

	float quadAtten = 0.0f;
	if (parameters.FindParameterByName("QuadraticAttenuation", p)) quadAtten = strtof(p.value.c_str(), NULL);

	float distanceCutOff = 0.0f;
	if (parameters.FindParameterByName("DistanceCutOff", p)) distanceCutOff = strtof(p.value.c_str(), NULL);

	float innerAngle = 0.0f;
	if (parameters.FindParameterByName("InnerAngle", p)) innerAngle = strtof(p.value.c_str(), NULL);

	float outerAngle = 0.0f;
	if (parameters.FindParameterByName("OuterAngle", p)) outerAngle = strtof(p.value.c_str(), NULL);

	LightSpot* light = new LightSpot();
	light->SetName(name);
	light->SetPosition(position);
	light->SetDiffuse(diffuse);
	light->SetSpecular(specular);
	light->SetVisibility(isVisible);
	light->SetConstantAttenuation(constAtten);
	light->SetLinearAttenuation(linearAtten);
	light->SetQuadraticAttenuation(quadAtten);
	light->SetCutoffDistance(distanceCutOff);
	light->SetInnerAngle(innerAngle);
	light->SetOuterAngle(outerAngle);
	light->SetUp(Vector3D(0.0f, 1.0f, 0.0f));
	light->SetScale(Vector3D(1.0f, 1.0f, 1.0f));
	light->SetRotation(Vector3D(0.0f));
	light->SetOrientation(Vector3D(0.0f, 0.0f, -1.0f));

	return light;
}

IComponent* VideoFactory::CreatePointLightComponent(ParameterContainer& parameters)
{
	Parameter p;

	std::string name;
	if (parameters.FindParameterByName("Name", p)) name = p.value;

	bool isVisible = false;
	if (parameters.FindParameterByName("IsVisible", p)) isVisible = strtoul(p.value.c_str(), NULL, 0) != 0;

	Vector3D position = Vector3D();
	if (parameters.FindParameterByName("Position", p)) position = StringHelpers::StrToVec3(p.value);

	Vector4D diffuse = Vector4D();
	if (parameters.FindParameterByName("DiffuseColor", p)) diffuse = StringHelpers::StrToVec4(p.value);

	Vector4D specular = Vector4D();
	if (parameters.FindParameterByName("SpecularColor", p)) specular = StringHelpers::StrToVec4(p.value);

	float constAtten = 0.0f;
	if (parameters.FindParameterByName("ConstAttenuation", p)) constAtten = strtof(p.value.c_str(), NULL);

	float linearAtten = 0.0f;
	if (parameters.FindParameterByName("LinearAttenuation", p)) linearAtten = strtof(p.value.c_str(), NULL);

	float quadAtten = 0.0f;
	if (parameters.FindParameterByName("QuadraticAttenuation", p)) quadAtten = strtof(p.value.c_str(), NULL);

	float distanceCutOff = 0.0f;
	if (parameters.FindParameterByName("DistanceCutOff", p)) distanceCutOff = strtof(p.value.c_str(), NULL);

	LightPoint* light = new LightPoint();
	light->SetName(name);
	light->SetPosition(position);
	light->SetDiffuse(diffuse);
	light->SetSpecular(specular);
	light->SetVisibility(isVisible);
	light->SetConstantAttenuation(constAtten);
	light->SetLinearAttenuation(linearAtten);
	light->SetQuadraticAttenuation(quadAtten);
	light->SetCutoffDistance(distanceCutOff);
	light->SetUp(Vector3D(0.0f, 1.0f, 0.0f));
	light->SetScale(Vector3D(1.0f, 1.0f, 1.0f));
	light->SetRotation(Vector3D(0.0f));
	light->SetOrientation(Vector3D(0.0f, 0.0f, -1.0f));

	return light;
}

VideoFactory::VideoFactory()
{
}

VideoFactory::~VideoFactory()
{
}

IComponent* VideoFactory::Create(ParameterContainer& parameters)
{
	Parameter p;
	bool rc = parameters.FindParameterByName("Type", p);

	if (rc)
	{
		if (p.value == "RenderComponent")
		{
			return CreateRenderComponent(parameters);
		}
		else if (p.value == "ClothRenderComponent")
		{
			return CreateClothRenderComponent(parameters);
		}
		else if (p.value == "ParticleEmitterComponent")
		{
			return CreateParticleEmitterComponent(parameters);
		}
		else if (p.value == "LightDirectional")
		{
			return CreateDirectionalLightComponent(parameters);
		}
		else if (p.value == "LightSpot")
		{
			return CreateSpotLightComponent(parameters);
		}
		else if (p.value == "LightPoint")
		{
			return CreatePointLightComponent(parameters);
		}
	}

	return nullptr;
}

const bool VideoFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return false;
}

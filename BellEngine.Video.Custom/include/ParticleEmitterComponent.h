#pragma once
#include <IParticleEmitter.h>
#include <BellEngineID.h>

class ParticleEmitterComponent : public IParticleEmitter
{
public:
	struct ParticleDefinition
	{
	public:
		unsigned long long particleCreationVolume;
		Vector3D minSpeed;
		Vector3D maxSpeed;
		Vector3D minDeltaPosition;
		Vector3D maxDeltaPosition;
		Vector4D diffuseColor, specularColor;
		float minLifeSeconds;
		float maxLifeSeconds;
		int minParticlesPerFrame;
		int maxParticlesPerFrame;
		float numParticlesPerSecond;
		Vector3D minDeltaAABBPosition;
		Vector3D maxDeltaAABBPosition;		// AABB
		float minDeltaRadiusPosition;
		float maxDeltaRadiusPosition;		// SPHERE	
		float minStartingScale;
		float maxStartingScale;
		float minScaleChangePercentPerSecond;
		float maxScaleChangePercentPerSecond;
		Vector3D startRotation;
		Quaternion minRotationalChangePerSecond;
		Quaternion maxRotationalChangePerSecond;
		bool fadeOutOverLifetime;	//This will change the alpha value of the object over time - from 1.0 at the start to 0.0 at the final lifetime
		bool isImposter;

		ParticleDefinition()
		{
			this->startRotation = Vector3D(0.0f, 0.0f, 0.0f);
			this->diffuseColor = Vector4D(1.0f, 1.0f, 1.0f, 1.0f);
			this->specularColor = Vector4D(1.0f, 1.0f, 1.0f, 1.0f);
			this->minParticlesPerFrame = 20;
			this->maxParticlesPerFrame = 50;
			this->minLifeSeconds = 0.0f;
			this->maxLifeSeconds = 3.0f;
			this->numParticlesPerSecond = 50;
			this->minDeltaPosition = Vector3D(0.0f, 0.0f, 0.0f);
			this->maxDeltaPosition = Vector3D(0.0f, 5.0f, 0.0f);
			this->minSpeed = Vector3D(0.0f, 1.0f, 0.0f);
			this->maxSpeed = Vector3D(0.0f, 3.0f, 0.0f);
			this->particleCreationVolume = 0ull;
			this->minDeltaAABBPosition = Vector3D(0.0f, 0.0f, 0.0f);
			this->maxDeltaAABBPosition = Vector3D(0.0f, 0.0f, 0.0f);
			this->minDeltaRadiusPosition = 0.0f;
			this->maxDeltaRadiusPosition = 1.0f;
			this->minStartingScale = 0.1f;
			this->maxStartingScale = 1.0f;
			this->minScaleChangePercentPerSecond = 0.0f;
			this->maxScaleChangePercentPerSecond = 0.0f;
			this->minRotationalChangePerSecond = Quaternion(Vector3D(0.0f, 0.0f, 0.0f));
			this->maxRotationalChangePerSecond = Quaternion(Vector3D(0.0f, 0.0f, 0.0f));
			this->isImposter = false;
			this->fadeOutOverLifetime = false;
		}
	};

	ParticleEmitterComponent();
	~ParticleEmitterComponent();

	void Activate(const bool& value);
	const bool IsActive();
	const bool IsUpdateComplete();

	const bool AddShaderFeature(ShaderFeature* ptr);
	const bool RemoveShaderFeature(ShaderFeature* ptr, const bool& deleterPtr = true);
	const bool HasShaderFeature(const std::string& featureName);
	const bool GetShaderFeatures(std::vector<ShaderFeature*>& shaderFeatures);
	void SetDefaultNumberOfParticles(size_t& value);
	const size_t GetDefaultNumberOfParticles();
	void SetPosition(const Vector3D& position);
	const Vector3D GetPosition();
	void SetAcceleration(const Vector3D& acceleration);
	const Vector3D GetAcceleration();
	const Quaternion GetImposterRotationFromEye(const Vector3D& particlePosition, const Vector3D& eyePosition);
	void CreateNewParticle();
	const bool GetParticles(std::vector<Particle*>& particles, const Vector3D& eyePosition = Vector3D(0.0f, 0.0f, 0.0f));
	void ToggleParticleCreation(const bool& value);
	const bool GetParticleCreationState();
	void Initialize(const ParticleEmitterComponent::ParticleDefinition& definition);

	const std::string GetType();
	const unsigned long long GetTypeID();
	const std::string GetName();
	void SetName(const std::string& name);

	void Update(const FrameTime& frameTime);
	ICloneable* Clone();
	void Dispose();

private:
	ParticleEmitterComponent::ParticleDefinition currentParticleDefinition;
	Vector3D acceleration;
	Vector3D position;
	std::string name;
	std::vector<Particle*> particles;
	double timeSinceLastParticle;
	double particleCreationPeriod;	//This is the inverse of number of particles per second	
	bool allowNewParticles;
	bool isUpdateComplete;
	bool isActive;
	size_t defaultNumberOfParticles;
	std::vector<ShaderFeature*> shaderFeatures;
};


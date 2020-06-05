#include "ParticleEmitterComponent.h"
#include <MathHelper.h>
#include <StringHelpers.h>

ParticleEmitterComponent::ParticleEmitterComponent()
{
	this->defaultNumberOfParticles = 100;
	this->allowNewParticles = false;
	this->currentParticleDefinition = ParticleEmitterComponent::ParticleDefinition();
	this->particleCreationPeriod = 0.0;
	this->timeSinceLastParticle = 0.0;
	this->isActive = true;
	this->isUpdateComplete = false;
	acceleration = Vector3D();
	position = Vector3D();
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	Dispose();
}

void ParticleEmitterComponent::Activate(const bool& value)
{
	this->isActive = value;
}

const bool ParticleEmitterComponent::IsActive()
{
	return this->isActive;
}

const bool ParticleEmitterComponent::IsUpdateComplete()
{
	return this->isUpdateComplete;
}

const bool ParticleEmitterComponent::AddShaderFeature(ShaderFeature* ptr)
{	
	if (ptr)
	{
		auto it = std::find(shaderFeatures.begin(), shaderFeatures.end(), ptr);
		if (it == shaderFeatures.end())
		{
			shaderFeatures.push_back(ptr);
			return true;
		}
	}	

	return false;
}

const bool ParticleEmitterComponent::RemoveShaderFeature(ShaderFeature* ptr, const bool& deleterPtr)
{
	if (ptr)
	{
		auto it = std::find(shaderFeatures.begin(), shaderFeatures.end(), ptr);
		if (it != shaderFeatures.end())
		{
			shaderFeatures.erase(it);

			if (deleterPtr)
			{
				DisposePointer(ptr);
			}

			return true;
		}
	}

	return false;
}

const bool ParticleEmitterComponent::HasShaderFeature(const std::string& featureName)
{
	auto it = std::find_if(shaderFeatures.begin(), shaderFeatures.end(), [featureName](ShaderFeature* sf) { return sf->GetName() == featureName; });
	if (it != shaderFeatures.end())
	{
		return true;
	}

	return false;
}

const bool ParticleEmitterComponent::GetShaderFeatures(std::vector<ShaderFeature*>& shaderFeatures)
{
	shaderFeatures = this->shaderFeatures;
	return !shaderFeatures.empty();
}

void ParticleEmitterComponent::SetDefaultNumberOfParticles(size_t& value)
{
	this->defaultNumberOfParticles = value;
}

const size_t ParticleEmitterComponent::GetDefaultNumberOfParticles()
{
	return this->defaultNumberOfParticles;
}

void ParticleEmitterComponent::SetPosition(const Vector3D& position)
{
	this->position = position;
}

const Vector3D ParticleEmitterComponent::GetPosition()
{
	return this->position;
}

void ParticleEmitterComponent::SetAcceleration(const Vector3D& acceleration)
{
	this->acceleration = acceleration;
}

const Vector3D ParticleEmitterComponent::GetAcceleration()
{
	return this->acceleration;
}

const Quaternion ParticleEmitterComponent::GetImposterRotationFromEye(const Vector3D& particlePosition, const Vector3D& eyePosition)
{
	// Angle returned is in radian already...
	float angle = atan2(particlePosition.x - eyePosition.x, particlePosition.z - eyePosition.z);

	//* ( 180.0 / PI );
	// Since we aren't paying any attention to the x and z rotations.
	return Quaternion(glm::vec3(0.0f, angle, 0.0f));
}

void ParticleEmitterComponent::CreateNewParticle()
{
	// Find the 1st particle that has life <= 0.0f
	for (std::vector<Particle*>::iterator it = this->particles.begin(); it != this->particles.end(); it++)
	{
		// Is this particle "dead"? 
		Particle* particle = *it;
		particle->diffuseColor = currentParticleDefinition.diffuseColor;
		particle->specularColor = currentParticleDefinition.specularColor;

		if (particle->lifeTime <= 0.0f)
		{
			// Make it "alive" and set to some values
			particle->lifeTime = MathHelper::RandomInRange<float>(currentParticleDefinition.minLifeSeconds, currentParticleDefinition.maxLifeSeconds);

			// Set the initial velocity
			particle->velocity.x = MathHelper::RandomInRange<float>(currentParticleDefinition.minSpeed.x, currentParticleDefinition.maxSpeed.x);
			particle->velocity.y = MathHelper::RandomInRange<float>(currentParticleDefinition.minSpeed.y, currentParticleDefinition.maxSpeed.y);
			particle->velocity.z = MathHelper::RandomInRange<float>(currentParticleDefinition.minSpeed.z, currentParticleDefinition.maxSpeed.z);

			//pCurParticle->position = this->position;
			float scale = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minStartingScale, this->currentParticleDefinition.maxStartingScale);
			particle->scale = Vector3D(scale, scale, scale);
			particle->scalePerSecond = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minScaleChangePercentPerSecond, this->currentParticleDefinition.maxScaleChangePercentPerSecond);
						
			// Use SLERP to pick a rotational change per second
			particle->rotationPerSeccond = glm::slerp(this->currentParticleDefinition.minRotationalChangePerSecond, this->currentParticleDefinition.maxRotationalChangePerSecond, MathHelper::RandomInRange<float>(0.0f, 1.0f));

			if (this->currentParticleDefinition.particleCreationVolume == ENGINE_OBJ_ID_PHY_COLLIDERSPHERE)
			{
				// Pick a random point and check if it's inside a sphere
				bool bKeepPickingPoints = true;
				while (bKeepPickingPoints)
				{
					particle->position.x = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minDeltaAABBPosition.x,
						this->currentParticleDefinition.maxDeltaAABBPosition.x);
					particle->position.y = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minDeltaAABBPosition.y,
						this->currentParticleDefinition.maxDeltaAABBPosition.y);
					particle->position.z = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minDeltaAABBPosition.z,
						this->currentParticleDefinition.maxDeltaAABBPosition.z);

					if (glm::distance(glm::vec3(0.0f, 0.0f, 0.0f), particle->position) >= this->currentParticleDefinition.minDeltaRadiusPosition)
					{
						if (glm::distance(glm::vec3(0.0f, 0.0f, 0.0f), particle->position) <= this->currentParticleDefinition.maxDeltaRadiusPosition)
						{
							// We're good
							particle->position += this->position;
							bKeepPickingPoints = false;
						}
					}
				}
			}
			else if (this->currentParticleDefinition.particleCreationVolume == ENGINE_OBJ_ID_PHY_COLLIDERAABB || this->currentParticleDefinition.particleCreationVolume == ENGINE_OBJ_ID_PHY_COLLIDERBOX)
			{
				particle->position.x = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minDeltaAABBPosition.x, this->currentParticleDefinition.maxDeltaAABBPosition.x);
				particle->position.y = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minDeltaAABBPosition.y, this->currentParticleDefinition.maxDeltaAABBPosition.y);
				particle->position.z = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minDeltaAABBPosition.z, this->currentParticleDefinition.maxDeltaAABBPosition.z);

				particle->position += this->position;
			}
			else
			{
				particle->position.x = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minDeltaPosition.x, this->currentParticleDefinition.maxDeltaPosition.x);
				particle->position.y = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minDeltaPosition.y,	this->currentParticleDefinition.maxDeltaPosition.y);
				particle->position.z = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minDeltaPosition.z,	this->currentParticleDefinition.maxDeltaPosition.z);

				particle->position += this->position;
			}

			particle->lockToCamera = this->currentParticleDefinition.isImposter;

			particle->alphaPerSecond = 0.0f;
			if (this->currentParticleDefinition.fadeOutOverLifetime)
			{
				// Calculate how fast the alpha value should change
				particle->alphaPerSecond = 1.0f / particle->lifeTime;
			}
		}
	}
}

const bool ParticleEmitterComponent::GetParticles(std::vector<Particle*>& _particles, const Vector3D& eyePosition)
{
	// Copy the internal vector of particles to the one that is passed.

	_particles.clear();
	_particles.reserve(this->particles.size());
	//	// Sets the pointer values to zero (NULL)
	//	vec_pParticles.resize( this->m_vec_pParticles.size(), 0 );

	// NOTE: There has to be space there, 
	// AND the objects have to be initialized already
	//	std::copy( this->m_vec_pParticles.begin(), 
	//			   this->m_vec_pParticles.end(), 
	//			   vec_pParticles.begin() );

	for (std::vector<Particle*>::iterator it = this->particles.begin(); it != this->particles.end(); it++)
	{
		// If alive, copy over
		Particle* particle = *it;

		if (particle->lockToCamera)
		{
			particle->rotation = this->GetImposterRotationFromEye(particle->position, eyePosition);
		}

		if (particle->lifeTime > 0.0f)
		{
			_particles.push_back(particle);
		}
	}

	return !_particles.empty();
}

void ParticleEmitterComponent::ToggleParticleCreation(const bool& value)
{
	this->allowNewParticles = value;
}

const bool ParticleEmitterComponent::GetParticleCreationState()
{
	return this->allowNewParticles;
}

void ParticleEmitterComponent::Initialize(const ParticleEmitterComponent::ParticleDefinition& definition)
{
	// In case we are calling Initialize() again... 
	// Clear all the existing particles
	// - If the new size is LARGER than the current vector, 
	//   allocate the extra space and call new
	// - If the new size is SMALLER than the current vector, 
	//   go though and call delete on all the "extra" particles. 
	DisposeVector(particles);

	// Load the vector with the maximum number of particles
	// Reserve doesn't create the objects, it just sets asside space in 1 step
	this->particles.reserve(defaultNumberOfParticles);

	for (size_t i = 0; i < this->defaultNumberOfParticles; i++)
	{
		//If we are calling Initialize() AGAIN, then we DON'T want to call new
		Particle* particle = new Particle();
		particle->diffuseColor = definition.diffuseColor;
		particle->specularColor = definition.specularColor;
		particle->position.x = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minDeltaPosition.x, this->currentParticleDefinition.maxDeltaPosition.x);
		particle->position.y = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minDeltaPosition.y, this->currentParticleDefinition.maxDeltaPosition.y);
		particle->position.z = MathHelper::RandomInRange<float>(this->currentParticleDefinition.minDeltaPosition.z, this->currentParticleDefinition.maxDeltaPosition.z);

		//particle->position += this->position;

		// Set the lifetime to 0.0 for all of them
		particle->lifeTime = 0.0f;

		this->particles.push_back(particle);
	}

	this->currentParticleDefinition = definition;
	this->particleCreationPeriod = 1.0f / (float)this->currentParticleDefinition.numParticlesPerSecond;
}

const std::string ParticleEmitterComponent::GetType()
{
	return "ParticleEmitterComponent";
}

const unsigned long long ParticleEmitterComponent::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_PARTICLEEMITTERCOMPONENT;
}

const std::string ParticleEmitterComponent::GetName()
{
	return this->name;
}

void ParticleEmitterComponent::SetName(const std::string& name)
{
	this->name = name;
}

void ParticleEmitterComponent::Update(const FrameTime& frameTime)
{
	if (!this->isActive) return;
	this->isUpdateComplete = false;

	SetFrameTime(frameTime);
	// Loop through the particles:
	// - Decrease the life by "deltaTime"
	// - If "still alive" (lifeTime > 0.0):
	//    - Move the particle

	for(auto& particle : particles)
	{
		particle->lifeTime -= this->frameTime.GetDeltaTime();

		if (particle->lifeTime > 0.0f)
		{
			particle->velocity += (this->acceleration * (float)this->frameTime.GetDeltaTime());
			particle->position += (particle->velocity * (float)this->frameTime.GetDeltaTime());
			particle->scale *= (1.0f + ((particle->scalePerSecond * (float)this->frameTime.GetDeltaTime())));

			// Rotate particle
			// Using SLERP to make a rotational change from 0 degrees (all axes) to the maximum rotation angle per second.
			Quaternion rotChange = glm::slerp(Quaternion(Vector3D(0.0f, 0.0f, 0.0f)), particle->rotationPerSeccond, (float)this->frameTime.GetDeltaTime());
			particle->rotation *= rotChange;

			// Does this particle fade to black over time
			if (currentParticleDefinition.fadeOutOverLifetime)
			{
				particle->diffuseColor.a -= particle->alphaPerSecond * (float)this->frameTime.GetDeltaTime();
			}
		}
	}

	// How many new particles should we create this frame? 
	//int numParticleToCreate = Helpers::RandomInRange<int>(this->minParticlesPerFrame, this->maxParticlesPerFrame);

	/*for (int i = 0; i != numParticleToCreate; i++)
	{
		this->CreateNewParticle();
	}*/

	if (this->allowNewParticles)
	{
		this->timeSinceLastParticle += this->frameTime.GetDeltaTime();
		if (this->timeSinceLastParticle >= this->particleCreationPeriod)
		{
			// Create a new particle
			this->CreateNewParticle();
			// Reset the timer
			this->timeSinceLastParticle = 0.0f;
		}
	}

	this->isUpdateComplete = true;
}

ICloneable* ParticleEmitterComponent::Clone()
{
	ParticleEmitterComponent* clone = new ParticleEmitterComponent();
	clone->acceleration = acceleration;
	clone->allowNewParticles = allowNewParticles;
	clone->currentParticleDefinition = currentParticleDefinition;
	clone->isActive = isActive;
	clone->name = name + "_" + StringHelpers::GenerateRandomString(4);
	clone->position = position;
	clone->particleCreationPeriod = particleCreationPeriod;
	clone->Initialize(currentParticleDefinition);

	return clone;
}

void ParticleEmitterComponent::Dispose()
{
	DisposeVector(particles);
}

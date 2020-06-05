#pragma once
#include <GLMCommon.h>
#include <vector>
#include <string>
#include "IUpdatable.h"
#include "IComponent.h"
#include "ShaderFeature.h"

class IParticleEmitter : public IComponent, public IUpdatable
{
protected:
	IParticleEmitter() {};

public:
	class Particle
	{
	public:
		float scalePerSecond, lifeTime, alphaPerSecond;
		Vector3D position, velocity, scale;
		Vector4D diffuseColor, specularColor;
		Quaternion rotation, rotationPerSeccond;
		bool lockToCamera;

		Particle()
		{
			position = Vector3D(0.0f, 0.0f, 0.0f);
			velocity = Vector3D(0.0f, 0.0f, 0.0f);
			lifeTime = 0.0f;
			diffuseColor = Vector4D(1.0f, 1.0f, 1.0f, 1.0f);
			specularColor = Vector4D(0.0f, 0.0f, 0.0f, 1.0f);
			scale = Vector3D();
			scalePerSecond = 0.0f;
			alphaPerSecond = 0.0f;
			rotation = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
			rotationPerSeccond = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);			
			lockToCamera = false;
		};
		~Particle() {};

		const Matrix4D GetWorldMatrix(const Vector3D& eyePosition)
		{
			Matrix4D m = Matrix4D(1.0f);

			//Translation transform
			Matrix4D matTrans = GetTranslationMatrix();
			m = m * matTrans;

			
			Matrix4D rotateMat = GetRotationMatrix(eyePosition);
			m = m * rotateMat;

			//Scale transform
			Matrix4D scale = GetScaleMatrix();
			m = m * scale;

			return m;
		}
		const Matrix4D GetWorldMatrix()
		{
			Matrix4D m = Matrix4D(1.0f);

			//Translation transform
			Matrix4D matTrans = GetTranslationMatrix();
			m = m * matTrans;

			Matrix4D rotateMat = GetRotationMatrix();
			m = m * rotateMat;

			//Scale transform
			Matrix4D scale = GetScaleMatrix();
			m = m * scale;

			return m;
		}
		const Matrix4D GetTranslationMatrix()
		{
			return glm::translate(Matrix4D(1.0f), position);
		}
		const Matrix4D GetScaleMatrix()
		{
			return glm::scale(Matrix4D(1.0f), scale);
		}
		const Matrix4D GetRotationMatrix()
		{
			return Matrix4D(rotation);
		}
		const Matrix4D GetRotationMatrix(const Vector3D& eyePosition)
		{
			auto r = GetRotationFromEye(eyePosition);
			return Matrix4D(r);
		}
		const Vector3D GetUpVector()
		{
			return Vector3D(0.0f, 1.0f, 0.0f);
		}
		const Vector3D GetForwardInWorld()
		{
			// The "forward" vector is +ve Z
			// (the 4th number is because we need a vec4 later)
			Vector4D orientationV4 = Vector4D(Vector3D(0.0f, 0.0f, -1.0f), 1.0f);

			Matrix4D identityMatrix = Matrix4D(1.0f);	// Identity

			Matrix4D rotateMat = Matrix4D(rotation);
			Matrix4D model = identityMatrix * rotateMat;

			// Like in the vertex shader, I multiply the test points
			// by the model matrix (MUST be a VEC4 because it's a 4x4 matrix)
			Vector4D forwardInWorldSpace = model * orientationV4;

			return forwardInWorldSpace;
		}
		const Quaternion GetRotationFromEye(const Vector3D& eyePosition)
		{
			// Angle returned is in radian already...
			float angle = atan2(position.x - eyePosition.x, position.z - eyePosition.z);

			//* ( 180.0 / PI );
			// Since we aren't paying any attention to the x and z rotations.
			return Quaternion(glm::vec3(0.0f, angle, 0.0f));
		}
	};
		
	~IParticleEmitter() {};

	virtual const bool AddShaderFeature(ShaderFeature* ptr) = 0;
	virtual const bool RemoveShaderFeature(ShaderFeature* ptr, const bool& deleterPtr = true) = 0;
	virtual const bool HasShaderFeature(const std::string& featureName) = 0;
	virtual const bool GetShaderFeatures(std::vector<ShaderFeature*>& shaderFeatures) = 0;
	virtual const Quaternion GetImposterRotationFromEye(const Vector3D& particlePosition, const Vector3D& eyePosition) = 0;
	virtual void CreateNewParticle() = 0;
	virtual const bool GetParticles(std::vector<Particle*>& particles, const Vector3D& eyePosition = Vector3D(0.0f, 0.0f, 0.0f)) = 0;
	virtual void ToggleParticleCreation(const bool& value) = 0;
	virtual const bool GetParticleCreationState() = 0;
	virtual void SetDefaultNumberOfParticles(size_t& value) = 0;
	virtual const size_t GetDefaultNumberOfParticles() = 0;
	virtual void SetPosition(const Vector3D& position) = 0;
	virtual const Vector3D GetPosition() = 0;
	virtual void SetAcceleration(const Vector3D& acceleration) = 0;
	virtual const Vector3D GetAcceleration() = 0;
};
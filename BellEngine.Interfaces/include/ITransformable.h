#pragma once
#include <ITransform.h>
#include <MathHelper.h>
#include <atomic>

class ITransformable
{
protected:
	bool allowUpTransform;
	std::atomic<bool> overrideWorldMatrix;
	ITransform* transform;
	std::atomic<Matrix4D> worldMatrix;

	ITransformable() : transform(nullptr), allowUpTransform(false)
	{
		overrideWorldMatrix.store(false);
	};

	void DisposeTransform()
	{
		if (this->transform)
		{
			delete (this->transform);
			this->transform = nullptr;
		}
	}

public:
	virtual ~ITransformable() {};

	void AllowUpTransform(bool option)
	{
		allowUpTransform = option;
	}
	bool IsupAllowedtoTransform()
	{
		return allowUpTransform;
	}

	void SetTransform(ITransform* transform)
	{
		this->transform = transform;
	}
	ITransform* GetTransform()
	{
		return this->transform;
	}

	void OverrideWorldMatrix(const bool& overrideWorldMatrix) 
	{   
		this->overrideWorldMatrix.store(overrideWorldMatrix);
	};
	void ApplyWorldMatrix(const Matrix4D& worldMatrix)
	{
		this->worldMatrix.store(worldMatrix);
	}

	const Matrix4D GetWorldMatrix()
	{
		if (this->overrideWorldMatrix.load())
		{
			return this->worldMatrix.load();
		}
		
		Matrix4D m = Matrix4D(1.0f);

		//Translation transform
		Matrix4D matTrans = GetTranslationMatrix();
		m = m * matTrans;

		Matrix4D rotateMat = GetRotationMatrix();
		m = m * rotateMat;

		//Scale transform
		Matrix4D scale = GetScaleMatrix();
		m = m * scale;

		this->worldMatrix.store(m);

		return m;
	}
	const Matrix4D GetTranslationMatrix()
	{
		return glm::translate(Matrix4D(1.0f), transform->GetPosition());
	}
	const Matrix4D GetScaleMatrix()
	{
		return glm::scale(Matrix4D(1.0f), transform->GetScale());
	}
	const Matrix4D GetRotationMatrix()
	{
		Quaternion q = transform->GetRotation();
		return Matrix4D(q);
	}
	const Vector3D GetUpVector()
	{
		if (allowUpTransform)
		{
			Matrix4D matRotation = GetRotationMatrix();
			Vector4D upVector = glm::vec4(transform->GetUp(), 1.0f);
			Vector4D newUp = matRotation * upVector;

			// Update the "At"
			transform->SetUp(glm::vec3(newUp));
		}

		return transform->GetUp();
	}
	const Vector3D GetForwardInWorld()
	{
		// The "forward" vector is +ve Z
		// (the 4th number is because we need a vec4 later)
		Vector4D orientationV4 = Vector4D(transform->GetOrientation(), 1.0f);

		Matrix4D identityMatrix = Matrix4D(1.0f);	// Identity

		Quaternion q = transform->GetRotation();
		Matrix4D rotateMat = Matrix4D(q);
		Matrix4D model = identityMatrix * rotateMat;

		// Like in the vertex shader, I multiply the test points
		// by the model matrix (MUST be a VEC4 because it's a 4x4 matrix)
		Vector4D forwardInWorldSpace = model * orientationV4;

		return forwardInWorldSpace;
	}
		
	void SetOrientation(const Vector3D& orientation)
	{
		GetTransform()->SetOrientation(orientation);
	}
	void SetUp(const Vector3D& up)
	{
		GetTransform()->SetUp(up);
	}
	void SetScale(const Vector3D& scale)
	{
		GetTransform()->SetScale(scale);
	}
	const Vector3D GetScale()
	{
		return GetTransform()->GetScale();
	}
	void SetPosition(const Vector3D& position)
	{
		GetTransform()->SetPosition(position);
	}
	const Vector3D GetPosition()
	{
		return GetTransform()->GetPosition();
	}
	void SetRotation(const Vector3D& eulerAngles)
	{
		Vector3D radians = Vector3D();
		radians.x = glm::radians(eulerAngles.x);
		radians.y = glm::radians(eulerAngles.y);
		radians.z = glm::radians(eulerAngles.z);

		Quaternion _rotation = glm::quat(radians);
		SetRotation(_rotation);
	}
	void SetRotation(const Quaternion& quaternion)
	{
		GetTransform()->SetRotation(quaternion);
	}
	const Vector3D GetRotation_Euler()
	{
		return MathHelper::QuaternionToEulerAngles(GetTransform()->GetRotation());
	}
	const Quaternion GetRotation()
	{
		return GetTransform()->GetRotation();
	}
	const Vector3D GetUp()
	{
		return GetTransform()->GetUp();
	}
	const Vector3D GetOrientation()
	{
		return GetTransform()->GetOrientation();
	}
};
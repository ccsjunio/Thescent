#include "CameraFree.h"
#include <BellEngineID.h>
#include <IRigidbody.h>

CameraFree::CameraFree()
{
	FarClippingPlane = NearClippingPlane = FOV = 0.0f;
	Up = EyePosition = Vector3D();
	target = Vector3D();
}

CameraFree::~CameraFree()
{

}

const Matrix4D CameraFree::GetViewMatrix()
{
	return glm::lookAt(EyePosition.load(), GetTargetPosition(), Up.load());
}

const Matrix4D CameraFree::GetProjectionMatrix(const float& aspectRatio)
{
	return glm::perspective
	(
		glm::radians(FOV.load()),
		aspectRatio,
		NearClippingPlane.load(),
		FarClippingPlane.load()
	);
}

void CameraFree::SetEyePosition(const Vector3D& eyePosition)
{
	this->EyePosition.store(eyePosition);
}

const Vector3D CameraFree::GetEyePosition()
{
	return this->EyePosition.load();
}

void CameraFree::SetFarClippingPlane(const float& farClippingPlane)
{
	this->FarClippingPlane.store(farClippingPlane);
}

const float CameraFree::GetFarClippingPlane()
{
	return this->FarClippingPlane.load();
}

void CameraFree::SetNearClippingPlane(const float& nearClippingPlane)
{
	this->NearClippingPlane.store(nearClippingPlane);
}

const float CameraFree::GetNearClippingPlane()
{
	return this->NearClippingPlane.load();
}

void CameraFree::SetFOV(const float& fov)
{
	this->FOV.store(fov);
}

const float CameraFree::GetFOV()
{
	return this->FOV.load();
}

void CameraFree::SetUp(const Vector3D& up)
{
	this->Up.store(up);
}

const Vector3D CameraFree::GetUp()
{
	return this->Up.load();
}

const bool CameraFree::IsUpdateComplete()
{
	return isUpdateComplete.load();
}

void CameraFree::Update(const FrameTime& deltaTime)
{
	isUpdateComplete.store(false);

	SetFrameTime(deltaTime);

	isUpdateComplete.store(true);
}

const std::string CameraFree::GetType()
{
	return "CameraFree";
}

void CameraFree::SetTarget(Vector3D target)
{
	this->target.store(target);
}

const Vector3D CameraFree::GetTargetPosition()
{
	return target.load();
}

const Vector3D CameraFree::GetForward()
{
	return glm::normalize(target.load() - EyePosition.load());
}

std::string CameraFree::GetTargetName()
{
	return std::string();
}

const std::string CameraFree::GetName()
{
	return this->name;
}

void CameraFree::SetName(const std::string& name)
{
	this->name = name;
}

const bool CameraFree::IsFacingEntity(Entity* entity)
{
	if (entity)
	{
		IRigidbody* rb = entity->GetComponent<IRigidbody*>();
		if (rb)
		{
			ICollider* collider = rb->GetCollider();
			if (collider)
			{
				std::vector<Vector3D> testpoints;
				if (collider->GetTransformedProperties()->GetExtremes(testpoints))
				{
					//https://stackoverflow.com/questions/203787/how-can-i-check-if-one-game-object-can-see-another for reference
					float cos_angle = cos(glm::radians(FOV.load()));
					Vector3D test_point_vector = Vector3D();
					float dot_product = 0.0f;

					for (Vector3D tp : testpoints)
					{
						test_point_vector = glm::normalize(tp - EyePosition.load());
						dot_product = glm::dot(GetForward(), test_point_vector);
						if (dot_product > cos_angle)
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

const unsigned long long CameraFree::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_CAMERAFREE;
}
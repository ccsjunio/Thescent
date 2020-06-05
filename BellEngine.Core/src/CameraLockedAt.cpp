#include "CameraLockedAt.h"
#include <BellEngineID.h>

CameraLockedAt::CameraLockedAt()
{
	FarClippingPlane.store(0.0f);
	NearClippingPlane.store(0.0f);
	FOV.store(0.0f);
	Up.store(Vector3D());
	EyePosition.store(Vector3D());
	movementStep.store(10.0f);
	target.store(nullptr);
	rigidbody.store(nullptr);
}

CameraLockedAt::~CameraLockedAt()
{
}

const Matrix4D CameraLockedAt::GetViewMatrix()
{
	return glm::lookAt(EyePosition.load(), GetTargetPosition(), Up.load());
}

const Matrix4D CameraLockedAt::GetProjectionMatrix(const float& aspectRatio)
{
	return glm::perspective
	(
		glm::radians(FOV.load()),
		aspectRatio,
		NearClippingPlane.load(),
		FarClippingPlane.load()
	);
}

void CameraLockedAt::SetEyePosition(const Vector3D& eyePosition)
{
	this->EyePosition.store(eyePosition);
}

const Vector3D CameraLockedAt::GetEyePosition()
{
	return this->EyePosition.load();
}

void CameraLockedAt::SetFarClippingPlane(const float& farClippingPlane)
{
	this->FarClippingPlane.store(farClippingPlane);
}

const float CameraLockedAt::GetFarClippingPlane()
{
	return this->FarClippingPlane.load();
}

void CameraLockedAt::SetNearClippingPlane(const float& nearClippingPlane)
{
	this->NearClippingPlane.store(nearClippingPlane);
}

const float CameraLockedAt::GetNearClippingPlane()
{
	return this->NearClippingPlane.load();
}

void CameraLockedAt::SetFOV(const float& fov)
{
	this->FOV.store(fov);
}

const float CameraLockedAt::GetFOV()
{
	return this->FOV.load();
}

void CameraLockedAt::SetUp(const Vector3D& up)
{
	this->Up.store(up);
}

const Vector3D CameraLockedAt::GetUp()
{
	return this->Up.load();
}

const bool CameraLockedAt::IsUpdateComplete()
{
	return isUpdateComplete.load();
}

const bool CameraLockedAt::IsFacingEntity(Entity* entity)
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

const std::string CameraLockedAt::GetName()
{
	return this->name;
}

void CameraLockedAt::SetName(const std::string& name)
{
	this->name = name;
}

const float CameraLockedAt::GetMovementStep()
{
	return movementStep;
}

void CameraLockedAt::SetMoveFactor(const float& movementStep)
{
	this->movementStep = movementStep;
}

void CameraLockedAt::MoveForward()
{
	rigidbody.load()->MoveFB(movementStep, this->frameTime.GetDeltaTime());
}

void CameraLockedAt::MoveBackward()
{
	rigidbody.load()->MoveFB(-movementStep, this->frameTime.GetDeltaTime());
}

void CameraLockedAt::StrafeLeft()
{
	rigidbody.load()->MoveRL(-movementStep, this->frameTime.GetDeltaTime());
}

void CameraLockedAt::StrafeRight()
{
	rigidbody.load()->MoveRL(movementStep, this->frameTime.GetDeltaTime());
}

void CameraLockedAt::MoveUp()
{
	rigidbody.load()->MoveUD(movementStep, this->frameTime.GetDeltaTime());
}

void CameraLockedAt::MoveDown()
{
	rigidbody.load()->MoveUD(-movementStep, this->frameTime.GetDeltaTime());
}

Entity* CameraLockedAt::GetTarget()
{
	return target.load();
}

IRigidbody* CameraLockedAt::GetRigidbody()
{
	return rigidbody.load();
}

void CameraLockedAt::SetTarget(Entity* target)
{
	this->target.store(target);
	this->rigidbody.store(target->GetComponent<IRigidbody*>());
}

void CameraLockedAt::Update(const FrameTime& frameTime)
{
	isUpdateComplete.store(false);

	SetFrameTime(frameTime);

	if (rigidbody)
	{
		EyePosition.store(rigidbody.load()->GetWorldMatrix() * Vector4D(Vector3D(0.0f, 0.0f, 0.1f), 1.0f));
		//level->camera->Up = (AIM->GetWorldMatrix() * Vector4D(Vector3D(0.0f, 1.0f, 0.0f), 1.0f)) - Vector4D(AIM->position, 1.0f);
		Up.store(rigidbody.load()->GetRotationMatrix() * Vector4D(rigidbody.load()->GetUp(), 1.0f));
		//Up = rigidbody->GetUpVector();
	}

	isUpdateComplete.store(true);
}

const Vector3D CameraLockedAt::GetForward()
{
	if (!rigidbody)
	{
		return Vector3D();
	}

	return rigidbody.load()->GetForwardInWorld();
}

const Vector3D CameraLockedAt::GetTargetPosition()
{
	return rigidbody.load()->GetPosition();
}

std::string CameraLockedAt::GetTargetName()
{
	if (!target)
	{
		return "";
	}

	return target.load()->GetName();
}

const std::string CameraLockedAt::GetType()
{
	return "CameraLockedAt";
}

const unsigned long long CameraLockedAt::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_CAMERALOCKEDAT;
}
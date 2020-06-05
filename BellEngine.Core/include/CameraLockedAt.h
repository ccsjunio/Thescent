#pragma once
#include "ICamera.h"
#include <IRigidbody.h>
#include <Entity.h>
#include <atomic>

class CameraLockedAt : public ICamera
{
private:
	std::atomic<Entity*> target;
	std::atomic<IRigidbody*> rigidbody;
	std::atomic<float> movementStep;
	std::string name;
	std::atomic<bool> isUpdateComplete;
	std::atomic<Vector3D> EyePosition;
	std::atomic<Vector3D> Up;
	std::atomic<float> FOV;
	std::atomic<float> FarClippingPlane;
	std::atomic<float> NearClippingPlane;

public:
	CameraLockedAt();
	~CameraLockedAt();

	const Matrix4D GetViewMatrix();
	const Matrix4D GetProjectionMatrix(const float& aspectRatio);

	void SetEyePosition(const Vector3D& eyePosition);
	const Vector3D GetEyePosition();
	void SetFarClippingPlane(const float& farClippingPlane);
	const float GetFarClippingPlane();
	void SetNearClippingPlane(const float& nearClippingPlane);
	const float GetNearClippingPlane();
	void SetFOV(const float& fov);
	const float GetFOV();
	void SetUp(const Vector3D& up);
	const Vector3D GetUp();
	const bool IsUpdateComplete();	
	const bool IsFacingEntity(Entity* entity);
	const std::string GetName();
	void SetName(const std::string& name);
	const unsigned long long GetTypeID();
	const float GetMovementStep();
	void SetMoveFactor(const float& movementStep);
	void MoveForward();
	void MoveBackward();
	void StrafeLeft();
	void StrafeRight();
	void MoveUp();
	void MoveDown();	
	Entity* GetTarget();
	IRigidbody* GetRigidbody();
	void SetTarget(Entity* target);	
	const Vector3D GetForward();
	const Vector3D GetTargetPosition();
	std::string GetTargetName();
	const std::string GetType();
	void Update(const FrameTime& frameTime);
};


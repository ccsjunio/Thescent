#pragma once
#include "ICamera.h"
#include <Entity.h>
#include <atomic>

class CameraFree : public ICamera
{
private:
	std::atomic<Vector3D> target;
	std::string name;
	std::atomic<bool> isUpdateComplete;
	std::atomic<Vector3D> EyePosition;
	std::atomic<Vector3D> Up;
	std::atomic<float> FOV;
	std::atomic<float> FarClippingPlane;
	std::atomic<float> NearClippingPlane;

public:
	CameraFree();
	~CameraFree();
	
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
	const unsigned long long GetTypeID();
	const std::string GetType();
	void SetTarget(Vector3D target);
	const Vector3D GetTargetPosition();
	const Vector3D GetForward();
	std::string GetTargetName();
	const std::string GetName();
	void SetName(const std::string& name);
	void Update(const FrameTime& deltaTime);
};


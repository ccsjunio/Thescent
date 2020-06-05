#pragma once
#include <MathLibrary.h>
#include <FrameTime.h>
#include <IUpdatable.h>

class Wind : public IUpdatable
{
private:
	bool isActive, isUpdateComplete;
	bool isConstrained;
	/*Area of activity*/
	float radius;
	Vector3D position;
	/*Force*/
	float maxWindStep, minWindStep;
	Vector3D currentForce;

public:
	Wind();
	~Wind();

	const bool IsUpdateComplete();
	void Activate(const bool& value);
	const bool IsActive();
	void GetWindStep(float& maxWindStep, float& minWindStep);
	void SetWindData(const float& radius, const Vector3D& position, const float& maxWindStep, const float& minWindStep);
	const bool GetContactArea(float& radius, Vector3D& position);
	const Vector3D GetForce();
	void Update(const FrameTime& frameTime);
};


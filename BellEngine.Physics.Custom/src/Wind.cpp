#include "Wind.h"
#include <MathHelper.h>

Wind::Wind()
{
	this->isActive = true;
	this->isConstrained = false;
	this->position = Vector3D();
	this->radius = 0.0f;
	this->maxWindStep = 1.0f;
	this->minWindStep = 0.0f;
	this->currentForce = Vector3D();
}

Wind::~Wind()
{

}

const bool Wind::IsUpdateComplete()
{
	return this->isUpdateComplete;
}

void Wind::Activate(const bool& value)
{
	this->isActive = value;
}

const bool Wind::IsActive()
{
	return this->isActive;
}

void Wind::GetWindStep(float& maxWindStep, float& minWindStep)
{
	maxWindStep = this->maxWindStep;
	minWindStep = this->minWindStep;
}

void Wind::SetWindData(const float& radius, const Vector3D& position, const float& maxWindStep, const float& minWindStep)
{
	this->isConstrained = radius != 0.0f;
	this->radius = radius;
	this->position = this->isConstrained ? position : Vector3D();
	this->maxWindStep = maxWindStep;
	this->minWindStep = minWindStep;
}

const bool Wind::GetContactArea(float& radius, Vector3D& position)
{
	if (isConstrained)
	{
		radius = this->radius;
		position = this->position;
		return true;
	}

	radius = 0.0f;
	position = Vector3D();
	return false;
}

const Vector3D Wind::GetForce()
{
	return currentForce;
}

void Wind::Update(const FrameTime& frameTime)
{
	this->isUpdateComplete = false;

	SetFrameTime(frameTime);

	/*No force yet, then generate*/
	/*if (glm::length(currentForce) == 0.0f)
	{
		currentForce.x = MathHelper::RandomInRange(minWindStep, maxWindStep);
		currentForce.y = MathHelper::RandomInRange(minWindStep, maxWindStep);
		currentForce.z = MathHelper::RandomInRange(minWindStep, maxWindStep);
	}*/

	currentForce.x = (float)sin(this->frameTime.GetDeltaTime() * ((double)maxWindStep - (double)minWindStep)) * 10.0f;
	currentForce.y = (float)sin(this->frameTime.GetDeltaTime() * ((double)maxWindStep - (double)minWindStep)) * 10.0f;
	currentForce.z = (float)sin(this->frameTime.GetDeltaTime() * ((double)maxWindStep - (double)minWindStep)) * 10.0f;

	this->isUpdateComplete = true;
}

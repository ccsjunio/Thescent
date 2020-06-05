#pragma once
#include <IRigidbody.h>

class Integrator
{
public:
	static bool SemiEulerIntegration(IRigidbody*& rb, const float& deltaTime, const Vector3D& gravity)
	{
		std::vector<double> dampening;
		return SemiEulerIntegration(rb, deltaTime, gravity, dampening);
	}
	static bool SemiEulerIntegration(IRigidbody*& rb, const float& deltaTime, const Vector3D& gravity, const std::vector<double>& dampening)
	{
		Vector3D velocity = Vector3D();
		Vector3D acceleration = Vector3D();
				
		//Takes the current acceleration of the rigidbody and adds gravity
		acceleration = rb->GetAcceleration() + gravity;
		rb->SetAcceleration(acceleration);

		//https://gafferongames.com/post/integration_basics/
		velocity = /*Integration*/ (rb->GetVelocity() + acceleration * deltaTime);

		for (double dampValue : dampening)
		{
			velocity *= dampValue;
		}

		rb->SetVelocity(velocity);
		rb->SetPosition(rb->GetPosition() + rb->GetVelocity() * deltaTime);
		rb->SetAcceleration(Vector3D(0.0f, 0.0f, 0.0f));

		return true;
	}
};
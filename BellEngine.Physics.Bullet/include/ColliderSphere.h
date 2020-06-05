#pragma once
#include <MathLibrary.h>
#include <ICollider.h>
#include <Transform.h>
#include <MathHelper.h>
#include "IBulletColliderWrapper.h"
#include "Convert.h"

#define COLLIDER_SPHERE "Sphere"

class ColliderSphere : public ICollider, public IBulletColliderWrapper
{
protected:
	IProperties* properties;
	IProperties* transformedProperties;
	std::string name;
	void* parent;

public:
	class Properties : public ICollider::IProperties
	{
	public:
		float radius;
		btSphereShape* shape;
		/*A set of points in this collider that have been transformed*/
		std::vector<Vector3D> extremes;
		ITransform* transform;
		
		Properties() : radius(1.0f) 
		{
			transform = new ::Transform();
			shape = new btSphereShape(radius);
			GenerateExtremes();
		}
		Properties(float radius, Vector3D position)
		{
			transform = new ::Transform();
			transform->SetPosition(position);
			this->radius = radius;
			shape = new btSphereShape(radius);
			GenerateExtremes();
		}
		Properties(const float& radius_, const Vector3D& position, const Matrix4D& worldMatrix)
		{
			transform = new ::Transform();
			transform->SetPosition(position);

			float largestScale = GetLargestScaleFromWorldMatrix(worldMatrix);

			this->radius = radius_ * largestScale;
			shape = new btSphereShape(this->radius);
			GenerateExtremes();
		}
		~Properties()
		{
			if (shape)
			{
				delete(shape);
				shape = nullptr;
			}

			if (transform)
			{
				delete (transform);
				transform = nullptr;
			}
		}
		
		void SetTransform(ITransform* transform)
		{
			this->transform = transform;
		}
		const bool GetExtremes(std::vector<Vector3D>& extremes)
		{
			extremes = this->extremes;
			return !extremes.empty();
		}
		ITransform* GetTransform()
		{
			return transform;
		}
		float GetLargestScaleFromWorldMatrix(const Matrix4D& worldMatrix)
		{
			//https://stackoverflow.com/questions/34220370/apply-matrix-transformation-to-a-sphere
			Vector3D scale = MathHelper::GetScaleFromWorldMatrix(worldMatrix);

			float scale_[]{ scale.x , scale.y, scale.z };
			float maxScaleSq = 0.0f;
			for (unsigned int i = 0; i < 3; i++)
			{
				if (scale_[i] > maxScaleSq)
				{
					maxScaleSq = scale_[i];
				}
			}

			// one sqrt when you know the largest of the three
			float largestScale = std::sqrt(maxScaleSq);

			return largestScale;
		}
		void Transform(const Matrix4D& worldMatrix, IProperties* relativeTo)
		{
			transform->SetPosition(worldMatrix * Vector4D(relativeTo->GetTransform()->GetPosition(), 1.0f));

			float largestScale = GetLargestScaleFromWorldMatrix(worldMatrix);
			radius = largestScale * ((ColliderSphere::Properties*)relativeTo)->radius;
			
			float shapeRadius = largestScale * ((ColliderSphere::Properties*)relativeTo)->shape->getRadius();
			shape->setUnscaledRadius(shapeRadius);

			//Transforms extremes
			std::vector<Vector3D> ext;
			if (relativeTo->GetExtremes(ext))
			{
				for (size_t i = 0; i < extremes.size(); i++)
				{
					extremes[i] = worldMatrix * Vector4D(ext[i], 1.0f);;
				}
			}			
		}
		void GenerateExtremes()
		{
			extremes.clear();

			Vector3D c = transform->GetPosition();
			float r = radius;
			extremes.push_back(Vector3D(r, c.y, c.z));
			extremes.push_back(Vector3D(-r, c.y, c.z));
			extremes.push_back(Vector3D(c.x, r, c.z));
			extremes.push_back(Vector3D(c.x, -r, c.z));
			extremes.push_back(Vector3D(c.x, c.y, r));
			extremes.push_back(Vector3D(c.x, c.y, -r));
			extremes.push_back(c);
		}
		ICloneable* Clone()
		{
			Properties* p = new Properties();
			p->radius = this->radius;
			p->extremes = this->extremes;
			p->transform = (::Transform*)this->transform->Clone();
			
			return p;
		}
	};
	
	ColliderSphere();
	ColliderSphere(const float& radius, const Vector3D& center);
	ColliderSphere(const float& radius, const Vector3D& center, const Matrix4D& worldMatrix);
	~ColliderSphere();

	void SetParent(void* parent);
	void* GetParent();
	const bool Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength);
	const bool Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength, ParameterContainer& output);
	const bool GetContent(const std::string& name,  ParameterContainer& data);
	void Transform(const Matrix4D& worldMatrix);
	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetType();
	const unsigned long long GetTypeID();
	ICloneable* Clone();
	ICollider::IProperties* GetProperties();
	ICollider::IProperties* GetTransformedProperties();

	btCollisionShape* GetWrappedShape();
};


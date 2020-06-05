#pragma once
#include <MathLibrary.h>
#include <ICollider.h>
#include <Transform.h>
#include <MathHelper.h>

#define COLLIDER_SPHERE "Sphere"

class ColliderSphere : public ICollider
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
		/*A set of points in this collider that have been transformed*/
		std::vector<Vector3D> extremes;
		ITransform* transform;
		
		Properties() : radius(0.0f) 
		{
			transform = new ::Transform();
			GenerateExtremes();
		}
		Properties(float radius, Vector3D position)
		{
			transform = new ::Transform();
			transform->SetPosition(position);
			this->radius = radius;
			GenerateExtremes();
		}
		~Properties()
		{
			if (transform)
			{
				delete (transform);
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
		void Transform(const Matrix4D& worldMatrix, IProperties* relativeTo)
		{
			transform->SetPosition(worldMatrix * Vector4D(relativeTo->GetTransform()->GetPosition(), 1.0f));

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
			radius = largestScale * ((ColliderSphere::Properties*)relativeTo)->radius;

			//Transforms extremes
			std::vector<Vector3D> ext;
			if (relativeTo->GetExtremes(ext))
			{
				for (size_t i = 0; i < extremes.size(); i++)
				{
					extremes[i] = worldMatrix * Vector4D(ext[i], 1.0f);
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
	ColliderSphere(float radius, Vector3D center);
	~ColliderSphere();

	void SetParent(void* parent);
	void* GetParent();
	const bool Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength);
	const bool Raycast(const Vector3D& rayStart, const Vector3D& direction, const float& rayLength, ParameterContainer& output);
	const unsigned long long GetTypeID();
	const bool GetContent(const std::string& name,  ParameterContainer& data);
	void Transform(const Matrix4D& worldMatrix);
	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetType();
	ICloneable* Clone();
	ICollider::IProperties* GetProperties();
	ICollider::IProperties* GetTransformedProperties();
};


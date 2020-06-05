#pragma once
#include <MathLibrary.h>
#include <ICollider.h>
#include <Transform.h>
#include <MathHelper.h>

#define COLLIDER_CAPSULE "Capsule"

class ColliderCapsule : public ICollider
{
protected:
	IProperties* properties;
	IProperties* transformedProperties;
	std::string name;
	void* parent;

public:
	// Region R = { x | (x - [a + (b - a)*t])∧2 <= r } , 0 <= t <= 1
	class Properties : public ICollider::IProperties
	{
	public:
		/*Medial line start*/
		Vector3D pointA;
		/*Medial line end*/
		Vector3D pointB;
		/*Radius at top and bottom.*/
		float radius;
		/*A set of points in this collider that have been transformed*/
		std::vector<Vector3D> extremes;
		ITransform* transform;
		
		Properties() : radius(0.0f)
		{
			transform = new ::Transform();
			GenerateExtremes();
		}
		Properties(const float& radius, const Vector3D& pointA, const Vector3D& pointB)
		{
			transform = new ::Transform();
			this->radius = radius;
			this->pointA = pointA;
			this->pointB = pointB;
			GenerateCenter();
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
		void GenerateCenter()
		{
			Vector3D sum = pointA + pointB;
			transform->SetPosition(Vector3D(sum.x / 2, sum.y / 2, sum.z / 2));
		}
		void Transform(const Matrix4D& worldMatrix, IProperties* relativeTo)
		{
			transform->SetPosition(worldMatrix * Vector4D(relativeTo->GetTransform()->GetPosition(), 1.0f));

			pointA = worldMatrix * Vector4D(((ColliderCapsule::Properties*)relativeTo)->pointA, 1.0f);
			pointB = worldMatrix * Vector4D(((ColliderCapsule::Properties*)relativeTo)->pointB, 1.0f);

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
			radius = largestScale * ((ColliderCapsule::Properties*)relativeTo)->radius;

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

			//Dome B
			extremes.push_back(Vector3D(pointB.x, pointB.y + radius, pointB.z));
			extremes.push_back(Vector3D(pointB.x + radius, pointB.y, pointB.z));
			extremes.push_back(Vector3D(pointB.x - radius, pointB.y, pointB.z));
			extremes.push_back(Vector3D(pointB.x, pointB.y, pointB.z + radius));
			extremes.push_back(Vector3D(pointB.x, pointB.y, pointB.z - radius));
			//DomeA
			extremes.push_back(Vector3D(pointA.x, pointA.y - radius, pointA.z));
			extremes.push_back(Vector3D(pointA.x + radius, pointA.y, pointA.z));
			extremes.push_back(Vector3D(pointA.x - radius, pointA.y, pointA.z));
			extremes.push_back(Vector3D(pointA.x, pointA.y, pointA.z + radius));
			extremes.push_back(Vector3D(pointA.x, pointA.y, pointA.z - radius));

			extremes.push_back(transform->GetPosition());
		}
		ICloneable* Clone()
		{
			Properties* p = new Properties();
			p->radius = this->radius;
			p->pointA = this->pointA;
			p->pointB = this->pointB;
			p->extremes = this->extremes;
			p->transform = (::Transform*)this->transform->Clone();

			return p;
		}
	};

	ColliderCapsule(const Vector3D& pointA, const Vector3D& pointB, const float& radius);
	ColliderCapsule();
	~ColliderCapsule();
		
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
};


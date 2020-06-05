#pragma once
#include <MathLibrary.h>
#include <ICollider.h>
#include <Transform.h>

#define COLLIDER_BOX "Box"

class ColliderBox : public ICollider
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
		static const unsigned int MAX_VERT_COUNT = 8;
		Vector3D max;
		Vector3D min;
		Vector3D vertices[MAX_VERT_COUNT];
		/*A set of points in this collider that have been transformed*/
		std::vector<Vector3D> extremes;
		ITransform* transform;

		Properties() : max(0.0f), min(0.0f) 
		{
			transform = new ::Transform();
			GenerateVertices();
			GenerateCenter();
			GenerateExtremes();
		}
		Properties(Vector3D min, Vector3D max)
		{
			transform = new ::Transform();
			this->max = max;
			this->min = min;

			GenerateVertices();
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
		void Transform(const Matrix4D& worldMatrix, IProperties* relativeTo)
		{
			transform->SetPosition(worldMatrix * Vector4D(relativeTo->GetTransform()->GetPosition(), 1.0f));
			
			min = worldMatrix * Vector4D(((Properties*)relativeTo)->min, 1.0f);
			max = worldMatrix * Vector4D(((Properties*)relativeTo)->max, 1.0f);
			for (unsigned int k = 0; k < MAX_VERT_COUNT; k++)
			{
				vertices[k] = worldMatrix * Vector4D(((Properties*)relativeTo)->vertices[k], 1.0f);
			}

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
		void GenerateCenter()
		{
			Vector3D sum = Vector3D();

			for (unsigned int i = 0; i < MAX_VERT_COUNT; i++)
			{
				sum.x += vertices[i].x;
				sum.y += vertices[i].y;
				sum.z += vertices[i].z;
			}

			transform->SetPosition(Vector3D(sum.x / MAX_VERT_COUNT, sum.y / MAX_VERT_COUNT, sum.z / MAX_VERT_COUNT));
		}
		void GenerateVertices()
		{
			vertices[0] = min;
			vertices[1] = Vector3D(min.x, min.y, max.z);
			vertices[2] = Vector3D(min.x, max.y, min.z);
			vertices[3] = Vector3D(min.x, max.y, max.z);
			vertices[4] = max;
			vertices[5] = Vector3D(max.x, max.y, min.z);
			vertices[6] = Vector3D(max.x, min.y, min.z);
			vertices[7] = Vector3D(max.x, min.y, max.z);
		}
		void GenerateExtremes()
		{
			extremes.clear();

			Vector3D c = transform->GetPosition();

			for (unsigned int i = 0; i < MAX_VERT_COUNT; i++)
			{
				extremes.push_back(this->vertices[i]);
			}

			extremes.push_back(Vector3D(max.x, c.y, c.z));
			extremes.push_back(Vector3D(min.x, c.y, c.z));
			extremes.push_back(Vector3D(c.x, max.y, c.z));
			extremes.push_back(Vector3D(c.x, min.y, c.z));
			extremes.push_back(Vector3D(c.x, c.y, max.z));
			extremes.push_back(Vector3D(c.x, c.y, min.z));
			extremes.push_back(c);
		}
		ICloneable* Clone()
		{
			Properties* p = new Properties();
			for (unsigned int i = 0; i < MAX_VERT_COUNT; i++)
			{
				p->vertices[i] = this->vertices[i];
			}
			p->max = this->max;
			p->min = this->min;
			p->extremes = this->extremes;
			p->transform = (::Transform*)this->transform->Clone();

			return p;
		}
	};
	
	ColliderBox();
	ColliderBox(Vector3D max, Vector3D min);
	~ColliderBox();

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


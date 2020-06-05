#pragma once
#include <MathLibrary.h>
#include <ICollider.h>
#include <Transform.h>
#include "IBulletColliderWrapper.h"
#include "Convert.h"

#define COLLIDER_PLANE "Plane"

class ColliderPlane : public ICollider, public IBulletColliderWrapper
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
		static const unsigned int MAX_VERT_COUNT = 4;
		Vector3D max, min;		
		/*float constant;*/
		std::vector<Vector3D> vertices;// [MAX_VERT_COUNT] ;
		Vector3D normal;		
		/*A set of points in this collider that have been transformed*/
		std::vector<Vector3D> extremes;
		ITransform* transform;
		//btBoxShape* shape;
		btStaticPlaneShape* shape;

		Properties() :/* constant(0.0f),*/ normal(Vector3D())
		{
			transform = new ::Transform();
			Vector3D p = Vector3D();
			for (unsigned int i = 0; i < MAX_VERT_COUNT; i++)
			{
				vertices.push_back(p);
			}
			GenerateCenter();
			GenerateNormal();
			GenerateExtremes();

			float d = glm::dot(vertices[0], normal);
			shape = new btStaticPlaneShape(Convert::Vec3ToBtVec3(normal), d);

			//shape = new btBoxShape();
		}
		Properties(const std::vector<Vector3D>& verts) :/* constant(0.0f),*/ normal(Vector3D())
		{
			transform = new ::Transform();
			vertices = verts;
			GenerateCenter();
			GenerateNormal();
			GenerateExtremes();

			float d = glm::dot(vertices[1] - vertices[0], normal);
			shape = new btStaticPlaneShape(Convert::Vec3ToBtVec3(normal), d);
		}
		Properties(const std::vector<Vector3D>& verts, const Matrix4D& worldMatrix)
		{
			transform = new ::Transform();
			//vertices = verts;
			for (Vector3D p : verts)
			{
				vertices.push_back(worldMatrix * Vector4D(p, 1.0f));
			}

			GenerateCenter();
			GenerateNormal();
			GenerateExtremes();

			float d = glm::dot(vertices[1] - vertices[0], normal);
			shape = new btStaticPlaneShape(Convert::Vec3ToBtVec3(normal), d);
		}
		~Properties()
		{
			if (shape)
			{
				delete shape;
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
		void Transform(const Matrix4D& worldMatrix, IProperties* relativeTo)
		{
			transform->SetPosition(worldMatrix * Vector4D(relativeTo->GetTransform()->GetPosition(), 1.0f));

			for (unsigned int i = 0; i < Properties::MAX_VERT_COUNT; i++)
			{
				vertices[i] = worldMatrix * Vector4D(((Properties*)relativeTo)->vertices[i], 1.0f);
			}
			
			GenerateCenter();
			GenerateNormal();
			GenerateExtremes();
		}
		void GenerateNormal()
		{
			Vector3D vAB = vertices[1] - vertices[0];
			Vector3D vAD = vertices[3] - vertices[0];

			Vector3D cross = glm::cross(vAB, vAD);
			normal = glm::normalize(cross);
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
		void GenerateExtremes()
		{
			extremes.clear();

			for (Vector3D vert : vertices)
			{
				extremes.push_back(vert);
			}

			Vector3D c = transform->GetPosition();
			extremes.push_back(c);
		}
		ICloneable* Clone()
		{
			Properties* p = new Properties();
			p->normal = normal;
			//p->constant = constant;

			p->transform = (::Transform*)this->transform->Clone();

			for (unsigned int i = 0; i < MAX_VERT_COUNT; i++)
			{
				p->vertices[i] = this->vertices[i];
			}

			p->extremes = this->extremes;
			
			return p;
		}
	};
	
	ColliderPlane();
	ColliderPlane(const std::vector<Vector3D>& vertices);
	ColliderPlane(const std::vector<Vector3D>& vertices, const Matrix4D& worldMatrix);
	ColliderPlane(Vector3D* vertices);
	~ColliderPlane();
	
	void SetParent(void* parent);
	void* GetParent();
	/*inline float GetConstant()
	{
		return ((Properties*)properties)->constant;
	}*/
	inline Vector3D GetNormal()
	{
		return ((Properties*)transformedProperties)->normal;
	}
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


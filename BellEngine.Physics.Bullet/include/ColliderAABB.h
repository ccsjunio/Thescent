#pragma once
#include <MathLibrary.h>
#include <ColliderParallelepiped.h>
#include <Triangle3D.h>
#include <map>

#define COLLIDER_AABB "AABB"

class ColliderAABB : public ColliderBox
{
public:
	struct Box3D
	{
	public:
		unsigned long long id;
		Vector3D min;
		float length;
		std::vector<Triangle3D*> triangles;

		Box3D()
		{
			id = 0;
			min = Vector3D(0.0f, 0.0f, 0.0f);
			length = 0.0f;
		}
		Box3D(Vector3D min, float length)
		{
			id = 0;
			this->min = min;
			this->length = length;
		}
		std::vector<Vector3D> GetBoxVertices()
		{
			std::vector<Vector3D> vertices;

			Vector3D p0 = min;
			Vector3D p1 = Vector3D(min.x, min.y, min.z + length);
			Vector3D p2 = Vector3D(min.x, min.y + length, min.z);
			Vector3D p3 = Vector3D(min.x, min.y + length, min.z + length);
			Vector3D p4 = min + length;
			Vector3D p5 = Vector3D(min.x + length, min.y + length, min.z);
			Vector3D p6 = Vector3D(min.x + length, min.y, min.z);
			Vector3D p7 = Vector3D(min.x + length, min.y, min.z + length);

			vertices.push_back(p0);
			vertices.push_back(p1);
			vertices.push_back(p2);
			vertices.push_back(p3);
			vertices.push_back(p4);
			vertices.push_back(p5);
			vertices.push_back(p6);
			vertices.push_back(p7);

			return vertices;
		}

		static unsigned long long GetID(Vector3D testLocation, float length)
		{
			unsigned long long ID;		// x, y, z

			//Make sure these are rounded to the nearest 100 (length)
			unsigned long long ulX = (unsigned long long)(fabs(testLocation.x) / length);// +100;	// -300	
			unsigned long long ulY = (unsigned long long)(fabs(testLocation.y) / length);// +100;	// 700
			unsigned long long ulZ = (unsigned long long)(fabs(testLocation.z) / length);// +100;	// -2400

			//Are these negative
			if (testLocation.x < 0.0f)
			{
				ulX += 100000;		// 000,000   100,000   100300
			}
			if (testLocation.y < 0.0f)
			{
				ulY += 100000;		// 000,000   100,000
			}
			if (testLocation.z < 0.0f)
			{
				ulZ += 100000;		// 000,000   100,000
			}

			unsigned long long oneMillion = 1000000;

			// Side length of my box is 100
			//		32 bit ---> 0 to 4,000,000,000  9
			//		64 bit ---> 0 to 16   ,000,000    ,000,000   ,000,000	
			ulX *= oneMillion * oneMillion;			// 100 * 100
			ulY *= oneMillion;
			ulZ *= 1;

			ID = ulX + ulY + ulZ;

			// 300, 700, 2400 ->      300,000700,002,400
			// -300, 700, -2400 -> 100300,000700,102,400

			return ID;
		}
		unsigned long long GetID()
		{
			unsigned long long id = Box3D::GetID(min, length);
			return id;
		}
		bool IsVertInside(Vector3D point)
		{
			if (point.x < min.x)	return false;
			if (point.x > (min.x + length)) return false;
			if (point.y < min.y)	return false;
			if (point.y > (min.y + length)) return false;
			if (point.z < min.z)	return false;
			if (point.z > (min.z + length)) return false;

			return true;
		};
	};

	std::map<unsigned long long, Box3D*> boxes;
	float baseInternalBoxLength;

	ColliderAABB();
	~ColliderAABB();

	const bool GetContent(const std::string& name,  ParameterContainer& data);
	void Transform(const Matrix4D& worldMatrix);
	const std::string GetType();
	const unsigned long long GetTypeID();
	ICloneable* Clone();
};


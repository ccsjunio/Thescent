#pragma once
#include <MathLibrary.h>
#include <Vertex3D.h>
#include <vector>
#include <I3DObject.h>
#include "ICloneable.h"

class Triangle3D : public I3DObject, public ICloneable
{
public:
	Vertex3D pointA, pointB, pointC;
	Vector3D faceNormal;
	Vector3D pointANormal, pointBNormal, pointCNormal;
	Vector3D center;

	Triangle3D()
	{
		pointA = Vertex3D();
		pointB = Vertex3D();
		pointC = Vertex3D();
		center = Vector3D(0.0f, 0.0f, 0.0f);
		faceNormal = Vector3D(0.0f, 0.0f, 0.0f);
		pointANormal = Vector3D(0.0f, 0.0f, 0.0f);
		pointBNormal = Vector3D(0.0f, 0.0f, 0.0f);
		pointCNormal = Vector3D(0.0f, 0.0f, 0.0f);
	}
	Triangle3D(Vertex3D pointA, Vertex3D pointB, Vertex3D pointC)
	{
		this->pointA = pointA;
		this->pointB = pointB;
		this->pointC = pointC;

		pointANormal = Vector3D(pointA.nx, pointA.ny, pointA.nz);
		pointBNormal = Vector3D(pointB.nx, pointB.ny, pointB.nz);
		pointCNormal = Vector3D(pointC.nx, pointC.ny, pointC.nx);

		center = (pointA.GetVec3() + pointB.GetVec3() + pointC.GetVec3()) / 3.0f;

		faceNormal = (pointANormal + pointBNormal + pointCNormal) / 3.0f;
		faceNormal = glm::normalize(faceNormal);
	}

	static bool NeedToSlice(Triangle3D& t, const float maxLength)
	{
		float distAB = glm::distance(t.pointB.GetVec3(), t.pointA.GetVec3());
		float distBC = glm::distance(t.pointC.GetVec3(), t.pointB.GetVec3());
		float distCA = glm::distance(t.pointA.GetVec3(), t.pointC.GetVec3());

		return distAB > maxLength || distBC > maxLength || distCA > maxLength;
	}
	static std::vector<Triangle3D> SliceTriangle(Triangle3D t, const float maxLength)
	{
		//Slice triangle into 4 smaller triangles
		std::vector<Triangle3D> ts;
		Vector3D AB, BC, CA;

		AB = t.pointB.GetVec3() + t.pointA.GetVec3();
		auto halfAB = Vector3D(AB.x / 2, AB.y / 2, AB.z / 2);
		BC = t.pointC.GetVec3() + t.pointB.GetVec3();
		auto halfBC = Vector3D(BC.x / 2, BC.y / 2, BC.z / 2);
		CA = t.pointA.GetVec3() + t.pointC.GetVec3();
		auto halfCA = Vector3D(CA.x / 2, CA.y / 2, CA.z / 2);

		Vertex3D v1, v2, v3;
		v1.x = halfAB.x; v1.y = halfAB.y; v1.z = halfAB.z;
		v2.x = halfBC.x; v2.y = halfBC.y; v2.z = halfBC.z;
		v3.x = halfCA.x; v3.y = halfCA.y; v3.z = halfCA.z;

		Triangle3D t1 = Triangle3D(v1, v2, v3);
		Triangle3D t2 = Triangle3D(t.pointA, v1, v3);
		Triangle3D t3 = Triangle3D(v1, t.pointB, v2);
		Triangle3D t4 = Triangle3D(v3, v2, t.pointC);

		ts.push_back(t1); ts.push_back(t2); ts.push_back(t3); ts.push_back(t4);

		for (unsigned int i = 0; i < ts.size(); i++)
		{
			if (NeedToSlice(ts[i], maxLength))
			{
				auto tsAgain = SliceTriangle(ts[i], maxLength);
				ts.insert(ts.end(), tsAgain.begin(), tsAgain.end());
			}
		}

		return ts;
	}

	ICloneable* Clone()
	{
		Triangle3D* clone = new Triangle3D();

		clone->pointA = pointA;
		clone->pointB = pointB;
		clone->pointC = pointC;
		clone->center = center;
		clone->faceNormal = faceNormal;
		clone->pointANormal = pointANormal;
		clone->pointBNormal = pointBNormal;
		clone->pointCNormal = pointCNormal;
		
		return clone;
	}
};
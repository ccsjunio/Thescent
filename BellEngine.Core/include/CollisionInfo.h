/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <MathLibrary.h>
#include <string>

struct CollisionInfo
{
public:
	enum class CollisionType_e
	{
		Unknown,

		SphereTriangle,
		SphereSphere,
		SphereCube,
		CubeTriangle,
		CubeCube
	};

	CollisionInfo::CollisionType_e type;
	void* objectThatCollided;
	float overlapDistance;
	float distance;
	Vector3D collisionNormal;
	Triangle3D closestTriangle;

	CollisionInfo()
	{
		type = CollisionInfo::CollisionType_e::Unknown;
		objectThatCollided = NULL;
		overlapDistance = 0.0f;
		distance = 0.0f;
		collisionNormal = Vector3D(0.0f, 0.0f, 0.0f);
		closestTriangle = Triangle3D();
	}

	std::string GetTypeString()
	{
		switch (type)
		{
		case CollisionInfo::CollisionType_e::SphereTriangle:
			return "SphereTriangle";
		case CollisionInfo::CollisionType_e::SphereSphere:
			return "SphereSphere";
		default:
			return "";
		}
	}
};
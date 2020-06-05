///***********************************************************************
//* Project           : BellEngine
//* Author            : Felipe Bellini
//* Date created      : Oct. 30 - 2019
//* Licensed under:	: MIT
//
//************************************************************************/
//#pragma once
//#include <vector>
//#include "GameObject.h"
//#include <Level.h>
//#include <CollisionInfo.h>
//#include <ColliderAABB.h>
//#include <ColliderSphere.h>
//#include <ColliderParallelepiped.h>
//#include <ColliderCapsule.h>
//#include <ColliderPlane.h>
//
//class PhysicsManager
//{
//private:
//	int TestSphereTriangle(ColliderSphere sphere, Triangle3D* triangle, Vector3D& p);
//	static CollisionInfo Collision_SphereSphere(GameObject* gameObject0, GameObject* gameObject1);
//	static CollisionInfo Collision_SphereCube(GameObject* RigidBody0, GameObject* RigidBody1);
//	static CollisionInfo Collision_CubeCube(GameObject* RigidBody0, GameObject* RigidBody1);
//
//public:
//	PhysicsManager();
//	~PhysicsManager();
//
//	static Vector3D ClosestPtPointTriangle(Vector3D p, Triangle3D* triangle);
//	static Vector3D ClosestPtPointTriangle(Vector3D p, Triangle3D& triangle);
//	static bool CheckForCollisions(Level& level, GameObject& gameObject, CollisionInfo& collisionInfo);
//	static CollisionInfo CheckForCollisions(GameObject* rigidBody0, GameObject* rigidBody1);
//	static void GetClosestTriangleToPosition(GameObject& gameObject0, GameObject& gameObject1, Vector3D& closestPoint, Triangle3D& closestTriangle);
//	//Finds the 2D area of a triangle.
//	static float GetTriangleArea2D(float x1, float y1, float x2, float y2, float x3, float y3);
//	//Compute barycentric coordinates (u, v, w) for point p with respect to triangle (a, b, c).
//	static void FindBarycentricCoordinates(Vector3D& p, Triangle3D& triangle, float& u, float& v, float& w);
//	//Test if point p is contained in triangle.
//	static bool IsPointInTriangle(Vector3D& p, Triangle3D& t);
//};
//

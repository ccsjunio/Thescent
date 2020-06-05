///***********************************************************************
//* Project           : BellEngine
//* Author            : Felipe Bellini
//* Date created      : Oct. 30 - 2019
//* Licensed under:	: MIT
//
//************************************************************************/
//#include "PhysicsManager.h"
//
////Solution proposed and kindly shared with us by Michael Feeney
//int PhysicsManager::TestSphereTriangle(ColliderSphere sphere, Triangle3D* triangle, Vector3D& p)
//{
//	float sphereRadius = ((ColliderSphere::Properties*)sphere.properties)->radius;
//
//	// Find point P on triangle ABC closest to sphere center
//	p = ClosestPtPointTriangle(sphere.properties->transform->position, triangle);
//
//	// Sphere and triangle intersect if the (squared) distance from sphere
//	// center to point p is less than the (squared) sphere radius
//	Vector3D v = p - sphere.properties->transform->position;
//	return glm::dot(v, v) <= sphereRadius * sphereRadius;
//}
//CollisionInfo PhysicsManager::Collision_SphereSphere(GameObject* RigidBody0, GameObject* RigidBody1)
//{
//	CollisionInfo col = {};
//	ColliderSphere* sphere0 = dynamic_cast<ColliderSphere*>(RigidBody0->collider);
//	ColliderSphere* sphere1 = dynamic_cast<ColliderSphere*>(RigidBody1->collider);
//
//	//If the radius is ZERO, it means that the objects are, essentially, transparent to collisions
//	if (sphere0->radius != 0.0f && sphere1->radius != 0.0f)
//	{
//		//When the object has a 'virtual sphere' around it.
//		//To check if two objects collide, the distance between the spheres must be calculated
//		//This means that we need to do (minObjsDistance = objRadius1 + objRadius2)
//		//The MINIMUM DISTANCE between the two objects is when they're touching surfaces
//		float minObjsDistance = sphere0->radius + sphere1->radius;
//
//		//COLLISION = VECTOR_A - VECTOR_B
//		//If COLLISION is lesser or equal than the minimum distance between the two objects, then the objects are in contact
//		bool hitInX = fabs((RigidBody0->position.x + sphere0->radius) - (RigidBody1->position.x + sphere1->radius)) <= minObjsDistance;
//		bool hitInY = fabs((RigidBody0->position.y + sphere0->radius) - (RigidBody1->position.y + sphere1->radius)) <= minObjsDistance;
//		bool hitInZ = fabs((RigidBody0->position.z + sphere0->radius) - (RigidBody1->position.z + sphere1->radius)) <= minObjsDistance;
//
//		//Assign the results to the return struct
//		if (hitInX && hitInY && hitInZ)
//		{
//			col.objectThatCollided = RigidBody1;
//		}
//	}
//
//	return col;
//}
//CollisionInfo PhysicsManager::Collision_SphereCube(GameObject* RigidBody0, GameObject* RigidBody1)
//{
//	CollisionInfo col = {};
//	ColliderSphere* sphere = dynamic_cast<ColliderSphere*>(RigidBody0->collider);
//	ColliderParallelepiped* cube = dynamic_cast<ColliderParallelepiped*>(RigidBody1->collider);
//
//	if (sphere && cube)
//	{
//		if (sphere->radius != 0.0f && !cube->Ignore())
//		{
//			float sqDist = 0.0f;
//			if (sphere->center.x < cube->min.x)
//				sqDist += (cube->min.x - sphere->center.x) * (cube->min.x - sphere->center.x);
//			if (sphere->center.x > cube->max.x)
//				sqDist += (sphere->center.x - cube->max.x) * (sphere->center.x - cube->max.x);
//
//			if (sphere->center.y < cube->min.y)
//				sqDist += (cube->min.y - sphere->center.y) * (cube->min.y - sphere->center.y);
//			if (sphere->center.y > cube->max.y)
//				sqDist += (sphere->center.y - cube->max.y) * (sphere->center.y - cube->max.y);
//
//			if (sphere->center.z < cube->min.z)
//				sqDist += (cube->min.z - sphere->center.z) * (cube->min.z - sphere->center.z);
//			if (sphere->center.z > cube->max.z)
//				sqDist += (sphere->center.z - cube->max.z) * (sphere->center.z - cube->max.z);
//
//			// Sphere and AABB intersect if the(squared) distance between them is less than the (squared) sphere radius.
//			bool collide = sqDist <= sphere->radius * sphere->radius;
//			if (collide)
//			{
//				col.objectThatCollided = RigidBody1;
//				col.distance = sqrtf(sqDist);
//			}
//		}
//	}
//
//	return col;
//}
//CollisionInfo PhysicsManager::Collision_CubeCube(GameObject* RigidBody0, GameObject* RigidBody1)
//{
//	CollisionInfo col = {};
//	ColliderParallelepiped* cube0 = dynamic_cast<ColliderParallelepiped*>(RigidBody0->collider);
//	ColliderParallelepiped* cube1 = dynamic_cast<ColliderParallelepiped*>(RigidBody1->collider);
//
//	bool collide = false;
//	if (!cube0->Ignore() && !cube1->Ignore())
//	{
//		bool xCol = cube0->max.x > cube1->min.x&& cube0->min.x < cube1->max.x;
//		bool yCol = cube0->max.y > cube1->min.y&& cube0->min.y < cube1->max.y;
//		bool zCol = cube0->max.z > cube1->min.z&& cube0->min.z < cube1->max.z;
//
//		collide = xCol && yCol && zCol;
//		if (collide)
//		{
//			col.objectThatCollided = RigidBody1;
//		}
//	}
//
//	return col;
//}
////Solution proposed by Ericsson's book and kindly shared with us by Michael Feeney
//Vector3D PhysicsManager::ClosestPtPointTriangle(Vector3D p, Triangle3D* triangle)
//{
//	Vector3D ab = triangle->pointB.GetVec3() - triangle->pointA.GetVec3();
//	Vector3D ac = triangle->pointC.GetVec3() - triangle->pointA.GetVec3();
//	Vector3D bc = triangle->pointC.GetVec3() - triangle->pointB.GetVec3();
//
//	// Compute parametric position s for projection P' of P on AB,
//	// P' = A + s*AB, s = snom/(snom+sdenom)
//	float snom = glm::dot(p - triangle->pointA.GetVec3(), ab), sdenom = glm::dot(p - triangle->pointB.GetVec3(), triangle->pointA.GetVec3() - triangle->pointB.GetVec3());
//
//	// Compute parametric position t for projection P' of P on AC,
//	// P' = A + t*AC, s = tnom/(tnom+tdenom)
//	float tnom = glm::dot(p - triangle->pointA.GetVec3(), ac), tdenom = glm::dot(p - triangle->pointC.GetVec3(), triangle->pointA.GetVec3() - triangle->pointC.GetVec3());
//
//	if (snom <= 0.0f && tnom <= 0.0f) return triangle->pointA.GetVec3(); // Vertex region early out
//
//	// Compute parametric position u for projection P' of P on BC,
//	// P' = B + u*BC, u = unom/(unom+udenom)
//	float unom = glm::dot(p - triangle->pointB.GetVec3(), bc), udenom = glm::dot(p - triangle->pointC.GetVec3(), triangle->pointB.GetVec3() - triangle->pointC.GetVec3());
//
//	if (sdenom <= 0.0f && unom <= 0.0f) return triangle->pointB.GetVec3(); // Vertex region early out
//	if (tdenom <= 0.0f && udenom <= 0.0f) return triangle->pointC.GetVec3(); // Vertex region early out
//
//
//	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
//	Vector3D n = glm::cross(triangle->pointB.GetVec3() - triangle->pointA.GetVec3(), triangle->pointC.GetVec3() - triangle->pointA.GetVec3());
//	float vc = glm::dot(n, glm::cross(triangle->pointA.GetVec3() - p, triangle->pointB.GetVec3() - p));
//	// If P outside AB and within feature region of AB,
//	// return projection of P onto AB
//	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
//		return triangle->pointA.GetVec3() + snom / (snom + sdenom) * ab;
//
//	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
//	float va = glm::dot(n, glm::cross(triangle->pointB.GetVec3() - p, triangle->pointC.GetVec3() - p));
//	// If P outside BC and within feature region of BC,
//	// return projection of P onto BC
//	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
//		return triangle->pointB.GetVec3() + unom / (unom + udenom) * bc;
//
//	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
//	float vb = glm::dot(n, glm::cross(triangle->pointC.GetVec3() - p, triangle->pointA.GetVec3() - p));
//	// If P outside CA and within feature region of CA,
//	// return projection of P onto CA
//	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
//		return triangle->pointA.GetVec3() + tnom / (tnom + tdenom) * ac;
//
//	// P must project inside face region. Compute Q using barycentric coordinates
//	float u = va / (va + vb + vc);
//	float v = vb / (va + vb + vc);
//	float w = 1.0f - u - v; // = vc / (va + vb + vc)
//	return u * triangle->pointA.GetVec3() + v * triangle->pointB.GetVec3() + w * triangle->pointC.GetVec3();
//}
//
//Vector3D PhysicsManager::ClosestPtPointTriangle(Vector3D p, Triangle3D& triangle)
//{
//	Vector3D ab = triangle.pointB.GetVec3() - triangle.pointA.GetVec3();
//	Vector3D ac = triangle.pointC.GetVec3() - triangle.pointA.GetVec3();
//	Vector3D bc = triangle.pointC.GetVec3() - triangle.pointB.GetVec3();
//
//	// Compute parametric position s for projection P' of P on AB,
//	// P' = A + s*AB, s = snom/(snom+sdenom)
//	float snom = glm::dot(p - triangle.pointA.GetVec3(), ab), sdenom = glm::dot(p - triangle.pointB.GetVec3(), triangle.pointA.GetVec3() - triangle.pointB.GetVec3());
//
//	// Compute parametric position t for projection P' of P on AC,
//	// P' = A + t*AC, s = tnom/(tnom+tdenom)
//	float tnom = glm::dot(p - triangle.pointA.GetVec3(), ac), tdenom = glm::dot(p - triangle.pointC.GetVec3(), triangle.pointA.GetVec3() - triangle.pointC.GetVec3());
//
//	if (snom <= 0.0f && tnom <= 0.0f) return triangle.pointA.GetVec3(); // Vertex region early out
//
//	// Compute parametric position u for projection P' of P on BC,
//	// P' = B + u*BC, u = unom/(unom+udenom)
//	float unom = glm::dot(p - triangle.pointB.GetVec3(), bc), udenom = glm::dot(p - triangle.pointC.GetVec3(), triangle.pointB.GetVec3() - triangle.pointC.GetVec3());
//
//	if (sdenom <= 0.0f && unom <= 0.0f) return triangle.pointB.GetVec3(); // Vertex region early out
//	if (tdenom <= 0.0f && udenom <= 0.0f) return triangle.pointC.GetVec3(); // Vertex region early out
//
//
//	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
//	Vector3D n = glm::cross(triangle.pointB.GetVec3() - triangle.pointA.GetVec3(), triangle.pointC.GetVec3() - triangle.pointA.GetVec3());
//	float vc = glm::dot(n, glm::cross(triangle.pointA.GetVec3() - p, triangle.pointB.GetVec3() - p));
//	// If P outside AB and within feature region of AB,
//	// return projection of P onto AB
//	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
//		return triangle.pointA.GetVec3() + snom / (snom + sdenom) * ab;
//
//	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
//	float va = glm::dot(n, glm::cross(triangle.pointB.GetVec3() - p, triangle.pointC.GetVec3() - p));
//	// If P outside BC and within feature region of BC,
//	// return projection of P onto BC
//	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
//		return triangle.pointB.GetVec3() + unom / (unom + udenom) * bc;
//
//	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
//	float vb = glm::dot(n, glm::cross(triangle.pointC.GetVec3() - p, triangle.pointA.GetVec3() - p));
//	// If P outside CA and within feature region of CA,
//	// return projection of P onto CA
//	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
//		return triangle.pointA.GetVec3() + tnom / (tnom + tdenom) * ac;
//
//	// P must project inside face region. Compute Q using barycentric coordinates
//	float u = va / (va + vb + vc);
//	float v = vb / (va + vb + vc);
//	float w = 1.0f - u - v; // = vc / (va + vb + vc)
//	return u * triangle.pointA.GetVec3() + v * triangle.pointB.GetVec3() + w * triangle.pointC.GetVec3();
//}
//
//PhysicsManager::PhysicsManager()
//{
//}
//
//PhysicsManager::~PhysicsManager()
//{
//}
//
//bool PhysicsManager::CheckForCollisions(Level& level, GameObject& gameObject, CollisionInfo& collisionInfo)
//{
//	bool collide = false;
//
//	std::string gameObjectColliderType = gameObject.collider->GetType();
//
//	for (unsigned int i = 0; i < level.gameObjects.size(); i++)
//	{
//		GameObject* go = level.gameObjects.at(i);
//		//Ignores the current gameObject
//		if (go->guid != gameObject.guid && go->allowCollisionDetection)
//		{
//			//Only checks for collision when the object is visible and rendered
//			if (go->isVisible)
//			{
//				std::string goColliderType = go->collider->GetType();
//
//				if (goColliderType == "Parallelepiped" && gameObjectColliderType == "Sphere")
//				{
//					Vector3D closestPoint = Vector3D(0.0f, 0.0f, 0.0f);
//					Triangle3D closestTriangle;
//
//					GetClosestTriangleToPosition(gameObject, *go, closestPoint, closestTriangle);
//
//					Vector3D normalInWorld = closestTriangle.center + (closestTriangle.faceNormal * 20.0f);
//
//					//Calculates distance between the center of the gameobject and the closest point to the object being tested
//					float distance = glm::length(gameObject.position - closestPoint);
//
//					ColliderSphere* shape = (ColliderSphere*)gameObject.collider;
//					collide = distance <= shape->radius;
//					if (collide)
//					{
//						collisionInfo.type = CollisionInfo::CollisionType_e::SphereTriangle;
//						collisionInfo.objectThatCollided = go;
//						collisionInfo.overlapDistance = shape->radius - distance;
//						collisionInfo.distance = distance;
//						collisionInfo.closestTriangle = closestTriangle;
//						collisionInfo.collisionNormal = closestTriangle.faceNormal;
//						break;
//					}
//				}
//
//				if (goColliderType == "Sphere" && gameObjectColliderType == "Sphere")
//				{
//					ColliderSphere* shape0 = (ColliderSphere*)gameObject.collider;
//					ColliderSphere* shape1 = (ColliderSphere*)go->collider;
//
//					//Calculate the squared distance between the centers of both spheres
//					Vector3D vecDist(shape1->center - shape0->center);
//					float distance = glm::length(gameObject.position - go->position);
//
//					collide = distance <= shape0->radius + shape1->radius;
//					if (collide)
//					{
//						collisionInfo.type = CollisionInfo::CollisionType_e::SphereSphere;
//						collisionInfo.objectThatCollided = go;
//						collisionInfo.overlapDistance = shape0->radius - distance;
//						collisionInfo.distance = distance;
//
//						float magnitude = glm::length(gameObject.speed);
//						collisionInfo.collisionNormal = glm::normalize(go->position - gameObject.position) * magnitude;
//						break;
//					}
//				}
//
//				if (goColliderType == "Parallelepiped" && gameObjectColliderType == "Parallelepiped")
//				{
//					Vector3D closestPoint = Vector3D(0.0f, 0.0f, 0.0f);
//					Triangle3D closestTriangle;
//
//					GetClosestTriangleToPosition(gameObject, *go, closestPoint, closestTriangle);
//
//					Vector3D centreOfTriangle = (closestTriangle.pointA.GetVec3() + closestTriangle.pointB.GetVec3() + closestTriangle.pointC.GetVec3()) / 3.0f;
//					Vector3D normalInWorld = centreOfTriangle + (closestTriangle.faceNormal * 20.0f);
//
//					//Calculates distance between the center of the gameobject and the closest point to the object being tested
//					float distance = glm::length(gameObject.position - closestPoint);
//
//					ColliderParallelepiped* shape = (ColliderParallelepiped*)gameObject.collider;
//					collide = distance <= glm::length(shape->max - shape->center);
//					if (collide)
//					{
//						collisionInfo.type = CollisionInfo::CollisionType_e::CubeTriangle;
//						collisionInfo.objectThatCollided = go;
//						collisionInfo.overlapDistance = glm::length(shape->max - shape->center) - distance;
//						collisionInfo.distance = distance;
//						collisionInfo.closestTriangle = closestTriangle;
//						collisionInfo.collisionNormal = closestTriangle.faceNormal;
//						break;
//					}
//				}
//
//				if (goColliderType == "AABB" && gameObjectColliderType == "Sphere")
//				{
//					/*AABB* aabb = (AABB*)go->collisionData;
//					for (unsigned int b = 0; b < aabb->boxes.size(); b++)
//					{
//						Box3D box3d = aabb->boxes[b];
//						if (box3d.IsVertInside(gameObject.position))
//						{
//							printf("HEY\n");
//						}
//					}*/
//
//
//
//					////--------------------------------------------------------------------------------
//					//Vector3D closestPoint = Vector3D(0.0f, 0.0f, 0.0f);
//					//Triangle3D closestTriangle;
//
//					//GetClosestTriangleToPosition(gameObject, *go, closestPoint, closestTriangle);
//
//					//Vector3D normalInWorld = closestTriangle.center + (closestTriangle.faceNormal * 20.0f);
//
//					////Calculates distance between the center of the gameobject and the closest point to the object being tested
//					//float distance = glm::length(gameObject.position - closestPoint);
//
//					//Sphere* shape = (Sphere*)gameObject.collisionData;
//					//collide = distance <= shape->radius;
//					//if (collide)
//					//{
//					//	collisionInfo.type = CollisionInfo::CollisionType_e::SphereTriangle;
//					//	collisionInfo.objectThatCollided = go;
//					//	collisionInfo.overlapDistance = shape->radius - distance;
//					//	collisionInfo.distance = distance;
//					//	collisionInfo.closestTriangle = closestTriangle;
//					//	collisionInfo.collisionNormal = closestTriangle.faceNormal;
//					//	break;
//					//}
//				}
//
//			}
//		}
//	}
//
//	return collide;
//}
//
//CollisionInfo PhysicsManager::CheckForCollisions(GameObject* rigidBody0, GameObject* rigidBody1)
//{
//	CollisionInfo col = {};
//
//	return col;
//}
//
//void PhysicsManager::GetClosestTriangleToPosition(GameObject& gameObject0, GameObject& gameObject1, Vector3D& closestPoint, Triangle3D& closestTriangle)
//{
//	// Assume the closest distance is REALLY far away
//	float closestDistanceSoFar = FLT_MAX;
//
//	std::vector<Triangle3D> triangles;
//	std::vector<Vertex3D> vertices;
//	gameObject1.GetTransformedMesh(triangles, vertices);
//
//	for (unsigned int i = 0; i < triangles.size(); i++)
//	{
//		Triangle3D triangle = triangles[i];
//				
//		Vector3D curClosetPoint = ClosestPtPointTriangle(gameObject0.position, triangle);
//		
//		// Is this the closest so far?
//		float distanceNow = glm::distance(curClosetPoint, gameObject0.position);
//
//		// is this closer than the closest distance
//		if (distanceNow <= closestDistanceSoFar)
//		{
//			closestDistanceSoFar = distanceNow;
//
//			closestPoint = curClosetPoint;
//
//			closestTriangle = triangle;
//		}
//	}
//}
//
////Finds the 2D area of a triangle.
//float PhysicsManager::GetTriangleArea2D(float x1, float y1, float x2, float y2, float x3, float y3)
//{
//	return (x1 - x2) * (y2 - y3) - (x2 - x3) * (y1 - y2);
//}
////Compute barycentric coordinates (u, v, w) for point p with respect to triangle (a, b, c).
//void PhysicsManager::FindBarycentricCoordinates(Vector3D& p, Triangle3D& triangle, float& u, float& v, float& w)
//{
//	Vector3D a = triangle.pointA.GetVec3();
//	Vector3D b = triangle.pointB.GetVec3();
//	Vector3D c = triangle.pointC.GetVec3();
//
//	// Unnormalized triangle normal
//	Vector3D m = glm::cross(b - a, c - a);
//	// Nominators and one-over-denominator for u and v ratios
//	float nu, nv, ood;
//	// Absolute components for determining projection plane
//	float x = abs(m.x), y = abs(m.y), z = abs(m.z);
//	// Compute areas in plane of largest projection
//	if (x >= y && x >= z)
//	{
//		// x is largest, project to the yz plane
//		nu = GetTriangleArea2D(p.y, p.z, b.y, b.z, c.y, c.z); // Area of PBC in yz plane
//		nv = GetTriangleArea2D(p.y, p.z, c.y, c.z, a.y, a.z); // Area of PCA in yz plane
//		ood = 1.0f / m.x; // 1/(2*area of ABC in yz plane)52 Chapter 3 A Math and Geometry Primer
//	}
//	else if (y >= x && y >= z)
//	{
//		// y is largest, project to the xz plane
//		nu = GetTriangleArea2D(p.x, p.z, b.x, b.z, c.x, c.z);
//		nv = GetTriangleArea2D(p.x, p.z, c.x, c.z, a.x, a.z);
//		ood = 1.0f / -m.y;
//	}
//	else
//	{
//		// z is largest, project to the xy plane
//		nu = GetTriangleArea2D(p.x, p.y, b.x, b.y, c.x, c.y);
//		nv = GetTriangleArea2D(p.x, p.y, c.x, c.y, a.x, a.y);
//		ood = 1.0f / m.z;
//	}
//	u = nu * ood;
//	v = nv * ood;
//	w = 1.0f - u - v;
//}
////Test if point p is contained in triangle.
//bool PhysicsManager::IsPointInTriangle(Vector3D& p, Triangle3D& t)
//{
//	float u, v, w;
//	FindBarycentricCoordinates(p, t, u, v, w);
//
//	return v >= 0.0f && w >= 0.0f && (v + w) <= 1.0f;
//}

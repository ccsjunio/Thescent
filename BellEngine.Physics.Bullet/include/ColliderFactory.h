#pragma once
#include <IFactory.h>
#include <ICollider.h>
#include <IExtendedInfo.h>
#include "ColliderAABB.h"
#include "ColliderSphere.h"
#include "ColliderParallelepiped.h"
#include "ColliderCapsule.h"
#include "ColliderPlane.h"
#include <IMesh.h>

class ColliderFactory : public IFactory<ICollider>
{
private:

	
protected:
	ColliderPlane* CreatePlane(IMesh* mesh);
	ColliderPlane* GetBoundingPlane(const std::vector<Vector3D>& points);
	ColliderPlane* GetBoundingPlane(const Vector3D& pA, const Vector3D& pB, const Vector3D& pC, const Vector3D& pD, const Matrix4D& worldMatrix);
	ColliderPlane* CreatePlane(const Vector3D& pA, const Vector3D& pB, const Vector3D& pC, const Vector3D& pD);

	ColliderBox* CreateBox(IMesh* mesh);
	ColliderBox* CreateBox(const Vector3D& min, const Vector3D& max);

	ColliderSphere* CreateSphere(IMesh* mesh);
	ColliderSphere* GetBoundingSphere(IMesh* mesh, const Matrix4D& worldMatrix);
	ColliderSphere* CreateSphere(const Vector3D& center, const float& radius);
	ColliderSphere* GetBoundingSphere(const Vector3D& center, const float& radius, const Matrix4D& worldMatrix);

	ColliderAABB* CreateAABB(IMesh* mesh, const float& lengthOfBoxes);

public:
	ColliderFactory();
	~ColliderFactory();

	ICollider* Create(ParameterContainer& colliderParameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
};


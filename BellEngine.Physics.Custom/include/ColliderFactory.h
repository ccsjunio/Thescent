#pragma once
#include <IFactory.h>
#include <ICollider.h>
#include <IExtendedInfo.h>
#include <IMesh.h>
#include "ColliderAABB.h"
#include "ColliderSphere.h"
#include "ColliderBox.h"
#include "ColliderCapsule.h"
#include "ColliderPlane.h"

class ColliderFactory : public IFactory<ICollider>
{
private:

	
protected:
	ColliderCapsule* CreateCapsule(IMesh* mesh);
	ColliderCapsule* CreateCapsule(const Vector3D& start, const Vector3D& end, const float& radius);

	ColliderPlane* CreatePlane(IMesh* mesh);
	ColliderPlane* CreatePlane(const Vector3D& pA, const Vector3D& pB, const Vector3D& pC, const Vector3D& pD);

	ColliderBox* CreateBox(IMesh* mesh);
	ColliderBox* CreateBox(const Vector3D& min, const Vector3D& max);

	ColliderSphere* CreateSphere(IMesh* mesh);
	ColliderSphere* CreateSphere(const Vector3D& center, const float& radius);

	ColliderAABB* CreateAABB(IMesh* mesh, const float& lengthOfBoxes);

public:
	ColliderFactory();
	~ColliderFactory();

	ICollider* Create(ParameterContainer& colliderParameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);
};


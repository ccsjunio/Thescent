#include "ColliderAABB.h"
#include <BellEngineID.h>

ColliderAABB::ColliderAABB()
{
	baseInternalBoxLength = 0.0f;
}
ColliderAABB::~ColliderAABB()
{
	std::map<unsigned long long, Box3D*>::iterator it = boxes.begin();
	for (std::pair<unsigned long long, Box3D*> element : boxes)
	{
		if (element.second)
		{
			delete element.second;
		}
	}
	boxes.clear();
}

const bool ColliderAABB::GetContent(const std::string& name, ParameterContainer& data)
{
	return false;
}

void ColliderAABB::Transform(const Matrix4D& worldMatrix)
{

}

const std::string ColliderAABB::GetType()
{
	return COLLIDER_AABB;
}

const unsigned long long ColliderAABB::GetTypeID()
{
	return ENGINE_OBJ_ID_PHY_COLLIDERAABB;
}

ICloneable* ColliderAABB::Clone()
{
	ColliderAABB* collider = new ColliderAABB();
	collider->properties = (Properties*)properties->Clone();
	collider->transformedProperties = (Properties*)transformedProperties->Clone();
	
	collider->baseInternalBoxLength = this->baseInternalBoxLength;
	
	for (std::pair<unsigned long long, ColliderAABB::Box3D*> boxMapElement : this->boxes)
	{
		if (boxMapElement.second)
		{
			ColliderAABB::Box3D* box3D = new ColliderAABB::Box3D();
			box3D->id = boxMapElement.second->id;
			box3D->length = boxMapElement.second->length;

			for (unsigned int triangleIndex = 0; box3D->triangles.size(); triangleIndex++)
			{
				Triangle3D* originalTriangle = box3D->triangles[triangleIndex];
				if (originalTriangle)
				{
					Triangle3D* t = new Triangle3D();
					t->center = originalTriangle->center;
					t->faceNormal = originalTriangle->faceNormal;
					t->pointA = originalTriangle->pointA;
					t->pointANormal = originalTriangle->pointANormal;
					t->pointB = originalTriangle->pointB;
					t->pointBNormal = originalTriangle->pointBNormal;
					t->pointC = originalTriangle->pointC;
					t->pointCNormal = originalTriangle->pointCNormal;
					box3D->triangles.push_back(t);
				}
			}

			collider->boxes[boxMapElement.first] = box3D;
		}
	}

	return collider;
}

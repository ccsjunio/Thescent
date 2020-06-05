#include "ColliderFactory.h"
#include <StringHelpers.h>
#include <IMesh.h>
#include <MathHelper.h>

ColliderCapsule* ColliderFactory::CreateCapsule(IMesh* mesh)
{
	std::vector<Vertex3D*> vertices;
	if (mesh->GetVertices(vertices))
	{
		std::vector<Vector3D> points;
		for (Vertex3D* vertex : vertices)
		{
			points.push_back(vertex->GetVec3());
		}

		if (!points.empty())
		{
			Vector3D min = Vector3D(0.0f, 0.0f, 0.0f);
			Vector3D max = Vector3D(0.0f, 0.0f, 0.0f);
			if (MathHelper::FindMaxMinPoints(points, max, min))
			{
				//Center
				Vector3D maxDivBy2 = Vector3D(max.x / 2, max.y / 2, max.z / 2);
				Vector3D minDivBy2 = Vector3D(min.x / 2, min.y / 2, min.z / 2);
				Vector3D center = maxDivBy2 + minDivBy2;

				Vector3D tempPA = Vector3D(center.x, min.y, center.z);
				Vector3D tempPB = Vector3D(center.x, max.y, center.z);

				float radius = glm::distance(center, tempPA);

				Vector3D pStart = Vector3D(center.x, tempPA.y + radius, center.z);
				Vector3D pEnd = Vector3D(center.x, tempPB.y - radius, center.z);

				return new ColliderCapsule(pStart, pEnd, radius);
			}
		}
	}

	return nullptr;
}

ColliderCapsule* ColliderFactory::CreateCapsule(const Vector3D& start, const Vector3D& end, const float& radius)
{
	ColliderCapsule* collider = new ColliderCapsule(start, end, radius);

	return collider;
}

ColliderPlane* ColliderFactory::CreatePlane(IMesh* mesh)
{
	std::vector<Triangle3D*> triangles;
	if (!mesh->GetTriangles(triangles))
	{
		return nullptr;
	}

	ColliderPlane* collider = new ColliderPlane();

	ColliderBox* rect = CreateBox(mesh);
	((ColliderPlane::Properties*)collider->GetProperties())->vertices[0] = ((ColliderBox::Properties*)rect->GetProperties())->min;
	((ColliderPlane::Properties*)collider->GetProperties())->vertices[3] = ((ColliderBox::Properties*)rect->GetProperties())->max;

	auto n = triangles[0]->faceNormal;

	float d = glm::dot(n, ((ColliderPlane::Properties*)collider->GetProperties())->vertices[3] - ((ColliderPlane::Properties*)collider->GetProperties())->vertices[0]);

	std::vector<Vector3D> ps;
	for (unsigned int k = 0; k < 8; k++)
	{
		if (((ColliderPlane::Properties*)collider->GetProperties())->vertices[3] != ((ColliderBox::Properties*)rect->GetProperties())->vertices[k])
		{
			float dot = glm::dot(n, ((ColliderPlane::Properties*)collider->GetProperties())->vertices[3] - ((ColliderBox::Properties*)rect->GetProperties())->vertices[k]);
			if (dot == d)
			{
				std::vector<Vector3D>::iterator it = std::find(ps.begin(), ps.end(), ((ColliderBox::Properties*)rect->GetProperties())->vertices[k]);
				if (it == ps.end())
				{
					ps.push_back(((ColliderBox::Properties*)rect->GetProperties())->vertices[k]);
				}
			}
		}
	}

	d = glm::dot(n, ((ColliderPlane::Properties*)collider->GetProperties())->vertices[0] - ((ColliderPlane::Properties*)collider->GetProperties())->vertices[3]);
	for (unsigned int k = 0; k < 8; k++)
	{
		if (((ColliderPlane::Properties*)collider->GetProperties())->vertices[0] != ((ColliderBox::Properties*)rect->GetProperties())->vertices[k])
		{
			float dot = glm::dot(n, ((ColliderPlane::Properties*)collider->GetProperties())->vertices[0] - ((ColliderBox::Properties*)rect->GetProperties())->vertices[k]);
			if (dot == d)
			{
				std::vector<Vector3D>::iterator it = std::find(ps.begin(), ps.end(), ((ColliderBox::Properties*)rect->GetProperties())->vertices[k]);
				if (it == ps.end())
				{
					ps.push_back(((ColliderBox::Properties*)rect->GetProperties())->vertices[k]);
				}
			}
		}
	}

	((ColliderPlane::Properties*)collider->GetProperties())->vertices[1] = ps[0];
	((ColliderPlane::Properties*)collider->GetProperties())->vertices[2] = ps[1];

	delete rect;

	return collider;
}
ColliderPlane* ColliderFactory::CreatePlane(const Vector3D& pA, const Vector3D& pB, const Vector3D& pC, const Vector3D& pD)
{
	std::vector<Vector3D> verts;
	verts.push_back(pA);
	verts.push_back(pB);
	verts.push_back(pC);
	verts.push_back(pD);

	ColliderPlane* collider = new ColliderPlane(verts);
	return collider;
}

ColliderBox* ColliderFactory::CreateBox(IMesh* mesh)
{
	std::vector<Vertex3D*> vertices;
	if (mesh->GetVertices(vertices))
	{
		std::vector<Vector3D> points;
		for (Vertex3D* vertex : vertices)
		{
			points.push_back(vertex->GetVec3());
		}

		if (!points.empty())
		{
			Vector3D min = Vector3D(0.0f, 0.0f, 0.0f);
			Vector3D max = Vector3D(0.0f, 0.0f, 0.0f);
			if (MathHelper::FindMaxMinPoints(points, max, min))
			{
				return new ColliderBox(max, min);
			}
		}
	}

	return nullptr;
}
ColliderBox* ColliderFactory::CreateBox(const Vector3D& min, const Vector3D& max)
{
	return new ColliderBox(max, min);
}

ColliderSphere* ColliderFactory::CreateSphere(IMesh* mesh)
{
	std::vector<Vertex3D*> vertices;
	if (!mesh->GetVertices(vertices))
	{
		return nullptr;
	}

	Vector3D center = Vector3D(0.0f, 0.0f, 0.0f);
	Vector3D min = Vector3D(0.0f, 0.0f, 0.0f);
	Vector3D max = Vector3D(0.0f, 0.0f, 0.0f);

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		//Min
		if (vertices[i]->x < min.x)
		{
			min.x = vertices[i]->x;
		}
		if (vertices[i]->y < min.y)
		{
			min.y = vertices[i]->y;
		}
		if (vertices[i]->z < min.z)
		{
			min.z = vertices[i]->z;
		}
		//Max
		if (vertices[i]->x > max.x)
		{
			max.x = vertices[i]->x;
		}
		if (vertices[i]->y > max.y)
		{
			max.y = vertices[i]->y;
		}
		if (vertices[i]->z > max.z)
		{
			max.z = vertices[i]->z;
		}
	}

	//Center
	Vector3D maxDivBy2 = Vector3D(max.x / 2, max.y / 2, max.z / 2);
	Vector3D minDivBy2 = Vector3D(min.x / 2, min.y / 2, min.z / 2);
	center = maxDivBy2 + minDivBy2;

	//Max poinst inside the sphere
	Vector3D minPointInX = Vector3D(min.x, 0.0f, 0.0f);
	Vector3D maxPointInX = Vector3D(max.x, 0.0f, 0.0f);
	Vector3D minPointInY = Vector3D(0.0f, min.y, 0.0f);
	Vector3D maxPointInY = Vector3D(0.0f, max.y, 0.0f);
	Vector3D minPointInZ = Vector3D(0.0f, 0.0f, min.z);
	Vector3D maxPointInZ = Vector3D(0.0f, 0.0f, max.z);

	//Choose a point to calculate the radius
	//Radius
	//r� = (x1-x0)� + (y1-y0)� + (z1-z0)�  
	float radiusAtPowerOfTwo = ((maxPointInX.x - center.x) * (maxPointInX.x - center.x)) +
		((maxPointInX.y - center.y) * (maxPointInX.y - center.y)) +
		((maxPointInX.z - center.z) * (maxPointInX.z - center.z));
	float radius = sqrtf(radiusAtPowerOfTwo);

	return new ColliderSphere(radius, center);
}
ColliderSphere* ColliderFactory::CreateSphere(const Vector3D& center, const float& radius)
{
	return new ColliderSphere(radius, center);
}

ColliderAABB* ColliderFactory::CreateAABB(IMesh* mesh, const float& lengthOfBoxes)
{
	std::vector<Triangle3D*> triangles;
	if (!mesh->GetTriangles(triangles))
	{
		return nullptr;
	}

	ColliderAABB* aabb = new ColliderAABB();
	aabb->baseInternalBoxLength = lengthOfBoxes;

	ColliderBox* rectangle = CreateBox(mesh);

	aabb->GetProperties()->SetTransform((Transform*)rectangle->GetProperties()->Clone());
	((ColliderAABB::Properties*)aabb->GetProperties())->max = ((ColliderBox::Properties*)rectangle->GetProperties())->max;
	((ColliderAABB::Properties*)aabb->GetProperties())->min = ((ColliderBox::Properties*)rectangle->GetProperties())->min;
	for (unsigned int i = 0; i < 8; i++)
	{
		((ColliderAABB::Properties*)aabb->GetProperties())->vertices[i] = ((ColliderBox::Properties*)rectangle->GetProperties())->vertices[i];
	}

	delete rectangle;

	for (float x = ((ColliderAABB::Properties*)aabb->GetProperties())->min.x; x <= ((ColliderAABB::Properties*)aabb->GetProperties())->max.x; x += aabb->baseInternalBoxLength)
	{
		for (float y = ((ColliderAABB::Properties*)aabb->GetProperties())->min.y; y <= ((ColliderAABB::Properties*)aabb->GetProperties())->max.y; y += aabb->baseInternalBoxLength)
		{
			for (float z = ((ColliderAABB::Properties*)aabb->GetProperties())->min.z; z <= ((ColliderAABB::Properties*)aabb->GetProperties())->max.z; z += aabb->baseInternalBoxLength)
			{
				ColliderAABB::Box3D* box = new ColliderAABB::Box3D(Vector3D(x, y, z), aabb->baseInternalBoxLength);
				box->id = box->GetID();

				aabb->boxes[box->id] = box;
			}
		}
	}

	//Adds triangles to boxes
	for (Triangle3D* triangle : triangles)
	{
		// Slice the triangle if one of its sides greater than the Box3D length. 
		if (Triangle3D::NeedToSlice(*triangle, aabb->baseInternalBoxLength))
		{
			std::vector<Triangle3D> subTriangles = Triangle3D::SliceTriangle(*triangle, aabb->baseInternalBoxLength);

			for (unsigned int subTri = 0; subTri < subTriangles.size(); subTri++)
			{
				Vector3D v1 = subTriangles[subTri].pointA.GetVec3();
				Vector3D v2 = subTriangles[subTri].pointB.GetVec3();
				Vector3D v3 = subTriangles[subTri].pointC.GetVec3();

				// Get the ID of the AABB that this vertex is inside of 
				unsigned long long ID_AABB_V1 = ColliderAABB::Box3D::GetID(v1, aabb->baseInternalBoxLength);
				unsigned long long ID_AABB_V2 = ColliderAABB::Box3D::GetID(v2, aabb->baseInternalBoxLength);
				unsigned long long ID_AABB_V3 = ColliderAABB::Box3D::GetID(v3, aabb->baseInternalBoxLength);

				std::map<unsigned long long, ColliderAABB::Box3D*>::iterator itBox = aabb->boxes.find(ID_AABB_V1);
				if (itBox != aabb->boxes.end())
				{
					auto box = aabb->boxes[ID_AABB_V1];
					if (box)
					{
						auto it = std::find(box->triangles.begin(), box->triangles.end(), triangle);
						if (it == box->triangles.end())
						{
							box->triangles.push_back(triangle);
						}
					}
				}

				itBox = aabb->boxes.find(ID_AABB_V2);
				if (itBox != aabb->boxes.end())
				{
					auto box = aabb->boxes[ID_AABB_V2];
					if (box)
					{
						auto it = std::find(box->triangles.begin(), box->triangles.end(), triangle);
						if (it == box->triangles.end())
						{
							box->triangles.push_back(triangle);
						}
					}
				}

				itBox = aabb->boxes.find(ID_AABB_V3);
				if (itBox != aabb->boxes.end())
				{
					auto box = aabb->boxes[ID_AABB_V3];
					if (box)
					{
						auto it = std::find(box->triangles.begin(), box->triangles.end(), triangle);
						if (it == box->triangles.end())
						{
							box->triangles.push_back(triangle);
						}
					}
				}
			}
		}
		else
		{
			Vector3D v1 = triangle->pointA.GetVec3();
			Vector3D v2 = triangle->pointB.GetVec3();
			Vector3D v3 = triangle->pointC.GetVec3();

			// Get the ID of the AABB that this vertex is inside of 
			unsigned long long ID_AABB_V1 = ColliderAABB::Box3D::GetID(v1, aabb->baseInternalBoxLength);
			unsigned long long ID_AABB_V2 = ColliderAABB::Box3D::GetID(v2, aabb->baseInternalBoxLength);
			unsigned long long ID_AABB_V3 = ColliderAABB::Box3D::GetID(v3, aabb->baseInternalBoxLength);

			std::map<unsigned long long, ColliderAABB::Box3D*>::iterator itBox = aabb->boxes.find(ID_AABB_V1);
			if (itBox != aabb->boxes.end())
			{
				auto box = aabb->boxes[ID_AABB_V1];
				if (box)
				{
					auto it = std::find(box->triangles.begin(), box->triangles.end(), triangle);
					if (it == box->triangles.end())
					{
						box->triangles.push_back(triangle);
					}
				}
			}

			itBox = aabb->boxes.find(ID_AABB_V2);
			if (itBox != aabb->boxes.end())
			{
				auto box = aabb->boxes[ID_AABB_V2];
				if (box)
				{
					auto it = std::find(box->triangles.begin(), box->triangles.end(), triangle);
					if (it == box->triangles.end())
					{
						box->triangles.push_back(triangle);
					}
				}
			}

			itBox = aabb->boxes.find(ID_AABB_V3);
			if (itBox != aabb->boxes.end())
			{
				auto box = aabb->boxes[ID_AABB_V3];
				if (box)
				{
					auto it = std::find(box->triangles.begin(), box->triangles.end(), triangle);
					if (it == box->triangles.end())
					{
						box->triangles.push_back(triangle);
					}
				}
			}
		}
	}

	std::vector<unsigned long long> marked_for_deletion;
	auto it = aabb->boxes.begin();
	for (std::pair<unsigned long long, ColliderAABB::Box3D*> element : aabb->boxes)
	{
		if (element.second->triangles.size() == 0)
		{
			marked_for_deletion.push_back(element.first);
		}
	}

	for (unsigned int i = 0; i < marked_for_deletion.size(); i++)
	{
		auto it_del = aabb->boxes.find(marked_for_deletion[i]);
		delete (it_del->second);

		aabb->boxes.erase(it_del);
	}

	return aabb;
}

ColliderFactory::ColliderFactory()
{
}
ColliderFactory::~ColliderFactory()
{
}

ICollider* ColliderFactory::Create(ParameterContainer& colliderParameters)
{
	ICollider* collider = nullptr;

	if (colliderParameters.GetCount() != 0)
	{
		Parameter type;
		bool rc = colliderParameters.FindParameterByName("Shape", type);
		if(!rc)
		{
			return nullptr;
		}

		Parameter meshes;
		rc = colliderParameters.FindParameterByName("mesh", meshes);
		
		if (rc && meshes.valuePtr)
		{
			auto meshVectorPtr = (std::vector<IMesh*>*)meshes.valuePtr;
			auto meshVector = *meshVectorPtr;

			if (type.value == COLLIDER_AABB)
			{
				float aabbLength = 0.0f;
				Parameter boxLength;
				rc = colliderParameters.FindParameterByName("box_length", boxLength);
				if (rc)
				{
					aabbLength = atof(boxLength.value.c_str());
				}

				collider = CreateAABB(meshVector[0], aabbLength);
			}
			else if (type.value == COLLIDER_CAPSULE)
			{
				collider = CreateCapsule(meshVector[0]);
			}
			else if (type.value == COLLIDER_BOX)
			{
				collider = CreateBox(meshVector[0]);
			}
			else if (type.value == COLLIDER_PLANE)
			{
				collider = CreatePlane(meshVector[0]);
			}
			else if (type.value == COLLIDER_SPHERE)
			{
				collider = CreateSphere(meshVector[0]);
			}
			else
			{
				SetLastErrorString("Collider " + type.name + " is not supported.");
			}
		}
		else
		{
			if (type.value == COLLIDER_AABB)
			{
				SetLastErrorString(std::string(COLLIDER_AABB) + " is not supported without a mesh yet.");
			}
			else if (type.value == COLLIDER_CAPSULE)
			{
				Vector3D start, end;
				float radius;

				Parameter p;
				rc = colliderParameters.FindParameterByName("start", p);
				if (!rc) return nullptr;
				start = StringHelpers::StrToVec3(p.value);

				rc = colliderParameters.FindParameterByName("end", p);
				if (!rc) return nullptr;
				end = StringHelpers::StrToVec3(p.value);

				rc = colliderParameters.FindParameterByName("radius", p);
				if (!rc) return nullptr;
				radius = strtof(p.value.c_str(), NULL);

				collider = CreateCapsule(start, end, radius);
			}
			else if (type.value == COLLIDER_BOX)
			{
				Vector3D min, max;
				Parameter p;

				rc = colliderParameters.FindParameterByName("min", p);
				if (!rc) return nullptr;
				min = StringHelpers::StrToVec3(p.value);

				rc = colliderParameters.FindParameterByName("max", p);
				if (!rc) return nullptr;
				max = StringHelpers::StrToVec3(p.value);

				collider = CreateBox(min, max);
			}
			else if (type.value == COLLIDER_PLANE)
			{
				Vector3D pA, pB, pC, pD;
				Parameter p;

				rc = colliderParameters.FindParameterByName("P0", p);
				if (!rc) return nullptr;
				pA = StringHelpers::StrToVec3(p.value);

				rc = colliderParameters.FindParameterByName("P1", p);
				if (!rc) return nullptr;
				pB = StringHelpers::StrToVec3(p.value);

				rc = colliderParameters.FindParameterByName("P2", p);
				if (!rc) return nullptr;
				pC = StringHelpers::StrToVec3(p.value);

				rc = colliderParameters.FindParameterByName("P3", p);
				if (!rc) return nullptr;
				pD = StringHelpers::StrToVec3(p.value);

				collider = CreatePlane(pA, pB, pC, pD);
			}
			else if (type.value == COLLIDER_SPHERE)
			{
				float radius = 0.0f;
				Vector3D center = Vector3D();
				Parameter p;

				rc = colliderParameters.FindParameterByName("center", p);
				if (!rc) return nullptr;
				center = StringHelpers::StrToVec3(p.value);

				rc = colliderParameters.FindParameterByName("radius", p);
				if (!rc) return nullptr;
				radius = atof(p.value.c_str());

				collider = CreateSphere(center, radius);
			}
			else
			{
				SetLastErrorString("Collider " + type.name + " is not supported.");
			}
		}
	}
	else
	{
		SetLastErrorString("The colliderParameters must have at least one parameter, which is the type of the collider.");
	}

	return collider;
}

const bool ColliderFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return true;
}

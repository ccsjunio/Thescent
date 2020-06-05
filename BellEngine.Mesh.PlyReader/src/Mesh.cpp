/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#include "Mesh.h"
#include <sstream>
#include <BellEngineID.h>

Mesh::Mesh()
{
	this->isTransformable = false;
}

Mesh::~Mesh()
{
	Dispose();
}

const bool Mesh::GetTextures(std::vector<ITexture*>& textures)
{
	textures = this->textures;

	return !textures.empty();
}

const bool Mesh::AddTexture(ITexture* texture)
{
	bool rc = false;

	auto it = std::find(textures.begin(), textures.end(), texture);
	if (it == textures.end())
	{
		textures.push_back(texture);
		rc = true;
	}

	return rc;
}

const bool Mesh::RemoveTexture(ITexture* texture)
{
	bool rc = false;

	auto it = std::find(textures.begin(), textures.end(), texture);
	if (it != textures.end())
	{
		textures.erase(it);
		DisposePointer(texture);

		rc = true;
	}

	return rc;
}

const bool Mesh::GetTriangles(std::vector<Triangle3D*>& triangles)
{
	triangles = this->triangles;

	return !triangles.empty();
}

const bool Mesh::AddTriangle(Triangle3D* triangle)
{
	bool rc = false;

	auto it = std::find(triangles.begin(), triangles.end(), triangle);
	if (it == triangles.end())
	{
		triangles.push_back(triangle);
		rc = true;
	}

	return rc;
}

const bool Mesh::RemoveTriangle(Triangle3D* triangle)
{
	bool rc = false;

	auto it = std::find(triangles.begin(), triangles.end(), triangle);
	if (it != triangles.end())
	{
		triangles.erase(it);
		DisposePointer(triangle);
		rc = true;
	}

	return false;
}

const bool Mesh::GetVertices(std::vector<Vertex3D*>& vertices)
{
	vertices = this->vertices;

	return !vertices.empty();
}

const bool Mesh::AddVertex(Vertex3D* vertex)
{
	bool rc = false;

	auto it = std::find(vertices.begin(), vertices.end(), vertex);
	if (it == vertices.end())
	{
		vertices.push_back(vertex);
		rc = true;
	}

	return rc;
}

const bool Mesh::RemoveVertex(Vertex3D* vertex)
{
	bool rc = false;

	auto it = std::find(vertices.begin(), vertices.end(), vertex);
	if (it != vertices.end())
	{
		vertices.erase(it);
		DisposePointer(vertex);
		rc = true;
	}

	return rc;
}

Vertex3D* Mesh::FindVertexByIndex(const unsigned long long& index)
{
	Vertex3D* ptr = nullptr;

	if (index < vertices.size())
	{
		ptr = vertices[index];
	}

	return ptr;
}

void Mesh::SetPath(const std::string& path)
{
	this->path = path;
}

const std::string Mesh::GetPath()
{
	std::string s;

	s = this->path;

	return s;
}

const bool Mesh::IsTransformable()
{
	return isTransformable;
}

void Mesh::AllowTransformations(const bool& value)
{
	isTransformable = value;
}

const bool Mesh::GetTransformedMesh(const Matrix4D& worldMatrix, const bool& allowTransform, std::vector<Triangle3D>& triangles, std::vector<Vertex3D>& vertices)
{
	bool rc = false;

	if (!this->triangles.empty() && !this->vertices.empty())
	{
		triangles.clear();
		triangles.shrink_to_fit();
		vertices.clear();
		vertices.shrink_to_fit();

		if (allowTransform)
		{
			for (Triangle3D* triangle : this->triangles)
			{
				Triangle3D t = *triangle;

				// Update position of the vertex
				Vector4D vertexAsMat4 = Vector4D(t.pointA.x, t.pointA.y, t.pointA.z, 1.0f);
				Vector4D vertexWorldTransformed = worldMatrix * vertexAsMat4;
				t.pointA.x = vertexWorldTransformed.x;
				t.pointA.y = vertexWorldTransformed.y;
				t.pointA.z = vertexWorldTransformed.z;

				vertexAsMat4 = Vector4D(t.pointB.x, t.pointB.y, t.pointB.z, 1.0f);
				vertexWorldTransformed = worldMatrix * vertexAsMat4;
				t.pointB.x = vertexWorldTransformed.x;
				t.pointB.y = vertexWorldTransformed.y;
				t.pointB.z = vertexWorldTransformed.z;

				vertexAsMat4 = Vector4D(t.pointC.x, t.pointC.y, t.pointC.z, 1.0f);
				vertexWorldTransformed = worldMatrix * vertexAsMat4;
				t.pointC.x = vertexWorldTransformed.x;
				t.pointC.y = vertexWorldTransformed.y;
				t.pointC.z = vertexWorldTransformed.z;

				//Updates normal vectors

				/*Matrix4x4 matWorld_Inv_Transp = glm::inverse(glm::transpose(worldMatrix));
				Vector4D normal = Vector4D(t.pointA.nx, t.pointA.ny, t.pointA.nz, 1.0f);
				Vector4D normalWorldTransformed = matWorld_Inv_Transp * normal;
				t.pointA.nx = normalWorldTransformed.x;
				t.pointA.ny = normalWorldTransformed.y;
				t.pointA.nz = normalWorldTransformed.z;

				normal = Vector4D(t.pointB.nx, t.pointB.ny, t.pointB.nz, 1.0f);
				normalWorldTransformed = matWorld_Inv_Transp * normal;
				t.pointB.nx = normalWorldTransformed.x;
				t.pointB.ny = normalWorldTransformed.y;
				t.pointB.nz = normalWorldTransformed.z;

				normal = Vector4D(t.pointC.nx, t.pointC.ny, t.pointC.nz, 1.0f);
				normalWorldTransformed = matWorld_Inv_Transp * normal;
				t.pointC.nx = normalWorldTransformed.x;
				t.pointC.ny = normalWorldTransformed.y;
				t.pointC.nz = normalWorldTransformed.z;*/

				Matrix4D matWorld_Inv_Transp = glm::inverse(glm::transpose(worldMatrix));
				Vector4D normal = Vector4D(t.pointANormal.x, t.pointANormal.y, t.pointANormal.z, 1.0f);
				Vector4D normalWorldTransformed = matWorld_Inv_Transp * normal;
				t.pointANormal.x = normalWorldTransformed.x;
				t.pointANormal.y = normalWorldTransformed.y;
				t.pointANormal.z = normalWorldTransformed.z;

				normal = Vector4D(t.pointBNormal.x, t.pointBNormal.y, t.pointBNormal.z, 1.0f);
				normalWorldTransformed = matWorld_Inv_Transp * normal;
				t.pointBNormal.x = normalWorldTransformed.x;
				t.pointBNormal.y = normalWorldTransformed.y;
				t.pointBNormal.z = normalWorldTransformed.z;

				normal = Vector4D(t.pointCNormal.x, t.pointCNormal.y, t.pointCNormal.z, 1.0f);
				normalWorldTransformed = matWorld_Inv_Transp * normal;
				t.pointCNormal.x = normalWorldTransformed.x;
				t.pointCNormal.y = normalWorldTransformed.y;
				t.pointCNormal.z = normalWorldTransformed.z;

				normal = Vector4D(t.faceNormal.x, t.faceNormal.y, t.faceNormal.z, 1.0f);
				normalWorldTransformed = matWorld_Inv_Transp * normal;
				t.faceNormal.x = normalWorldTransformed.x;
				t.faceNormal.y = normalWorldTransformed.y;
				t.faceNormal.z = normalWorldTransformed.z;

				triangles.push_back(t);
			}

			for (Vertex3D* vertex : this->vertices)
			{
				Vertex3D v = *vertex;

				// Update position of the vertex
				Vector4D vertexAsMat4 = Vector4D(v.x, v.y, v.z, 1.0f);
				Vector4D vertexWorldTransformed = worldMatrix * vertexAsMat4;
				v.x = vertexWorldTransformed.x;
				v.y = vertexWorldTransformed.y;
				v.z = vertexWorldTransformed.z;

				//Updates normal vectors
				Matrix4D matWorld_Inv_Transp = glm::inverse(glm::transpose(worldMatrix));
				Vector4D normal = Vector4D(v.nx, v.ny, v.nz, 1.0f);
				Vector4D normalWorldTransformed = matWorld_Inv_Transp * normal;
				v.nx = normalWorldTransformed.x;
				v.ny = normalWorldTransformed.y;
				v.nz = normalWorldTransformed.z;

				vertices.push_back(v);
			}
		}
		else
		{
			for (Triangle3D* triangle : this->triangles)
			{
				Triangle3D t = *triangle;
				triangles.push_back(t);
			}

			for (Vertex3D* vertex : this->vertices)
			{
				Vertex3D v = *vertex;
				vertices.push_back(v);
			}
		}

		rc = true;
	}

	return rc;
}

std::map<float, Vector3D, std::greater<float>> Mesh::GetHighestPointsInMesh()
{
	std::map<float, Vector3D, std::greater<float>> points;

	for (auto vertex : vertices)
	{
		Vertex3D p = *vertex;

		Vector3D point = p.GetVec3();
		float distanceInY = point.y;
		points[distanceInY] = point;
	}

	return points;
}

const std::string Mesh::GetName()
{
	return this->name;
}

void Mesh::SetName(const std::string& name)
{
	this->name = name;
}

const std::string Mesh::GetType()
{
	return "Mesh";
}

const unsigned long long Mesh::GetTypeID()
{
	return ENGINE_OBJ_ID_MSH_MESH;
}

ICloneable* Mesh::Clone()
{
	Mesh* clone = new Mesh();
	clone->SetName(GetName());
	clone->SetPath(GetPath());
	clone->SetDiffuse(GetDiffuse());
	clone->SetSpecular(GetSpecular());
	clone->AllowTransformations(IsTransformable());

	for (Vertex3D* vert : vertices)
	{
		clone->AddVertex((Vertex3D*)vert->Clone());
	}

	for (Triangle3D* triangle : triangles)
	{
		clone->AddTriangle((Triangle3D*)triangle->Clone());
	}

	for (ITexture* texture : textures)
	{
		clone->AddTexture((ITexture*)texture->Clone());
	}

	return clone;
}

void Mesh::Dispose()
{
	if (!DisposeVector(triangles))
	{
		//Failed
	}

	if (!DisposeVector(vertices))
	{
		//Failed
	}

	//if (!DisposeVector(textures))
	//{
	//	//Failed
	//}

	//DisposeVector(textures);
	//rc = DisposeVector(bones);
	//if (!rc)
	//{
	//	//Failed
	//}
}

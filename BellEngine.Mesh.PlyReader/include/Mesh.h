/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <IMesh.h>

class Mesh : public IMesh
{
private:
	bool isTransformable;
	std::string path;
	std::vector<ITexture*> textures;
	std::vector<Vertex3D*> vertices;
	std::vector<Triangle3D*> triangles;
	std::string name;

public:
	Mesh();
	~Mesh();

	const bool GetTextures(std::vector<ITexture*>& textures);
	const bool AddTexture(ITexture* texture);
	const bool RemoveTexture(ITexture* texture);

	const bool GetTriangles(std::vector<Triangle3D*>& triangles);
	const bool AddTriangle(Triangle3D* triangle);
	const bool RemoveTriangle(Triangle3D* triangle);

	const bool GetVertices(std::vector<Vertex3D*>& vertices);
	const bool AddVertex(Vertex3D* vertex);
	const bool RemoveVertex(Vertex3D* vertex);
	Vertex3D* FindVertexByIndex(const unsigned long long& index);

	void SetPath(const std::string& path);
	const std::string GetPath();
	const bool IsTransformable();
	void AllowTransformations(const bool& option);
	const bool GetTransformedMesh(const Matrix4D& worldMatrix, const bool& allowTransform, std::vector<Triangle3D>& triangles, std::vector<Vertex3D>& vertices);

	std::map<float, Vector3D, std::greater<float>> GetHighestPointsInMesh();

	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetType();
	const unsigned long long GetTypeID();
	ICloneable* Clone();
	void Dispose();
};
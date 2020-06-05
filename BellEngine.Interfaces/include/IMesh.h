#pragma once
#include <vector>
#include <map>
#include <string>
#include <Triangle3D.h>
#include <ITexture.h>
#include "IIdentifiable.h"
#include "IColorable.h"
#include <array>
#include "IDisposable.h"
#include "ICloneable.h"

//#define MESH_MAX_BONES_PER_VERTEX 4

class IMesh : public IIdentifiable, public IColorable, public IDisposable, public ICloneable
{
protected:
	IMesh(){};
	
public:	
	virtual ~IMesh() {};
	
	virtual const bool GetTextures(std::vector<ITexture*>& textures) = 0;
	virtual const bool AddTexture(ITexture* texture) = 0;
	virtual const bool RemoveTexture(ITexture* texture) = 0;

	virtual const bool GetTriangles(std::vector<Triangle3D*>& triangles) = 0;
	virtual const bool AddTriangle(Triangle3D* triangle) = 0;
	virtual const bool RemoveTriangle(Triangle3D* triangle) = 0;

	virtual const bool GetVertices(std::vector<Vertex3D*>& vertices) = 0;
	virtual const bool AddVertex(Vertex3D* vertex) = 0;
	virtual const bool RemoveVertex(Vertex3D* vertex) = 0;
	virtual Vertex3D* FindVertexByIndex(const unsigned long long& index) = 0;

	virtual void SetPath(const std::string& path) = 0;
	virtual const std::string GetPath() = 0;
	virtual const bool IsTransformable() = 0;
	virtual void AllowTransformations(const bool& option) = 0;
	virtual std::map<float, Vector3D, std::greater<float>> GetHighestPointsInMesh() = 0;
	virtual const bool GetTransformedMesh(const Matrix4D& worldMatrix, const bool& allowTransform, std::vector<Triangle3D>& triangles, std::vector<Vertex3D>& vertices) = 0;
};
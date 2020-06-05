#pragma once
#include <IRender.h>
#include <ModelDef.h>
#include <Triangle3D.h>

class IClothRenderComponent : public IRender
{
protected:
	IClothRenderComponent() {};

public:
	~IClothRenderComponent() {};

	virtual const bool GetClothTriangles(const size_t& rowCount, const size_t& columnCount, const std::vector<Vector3D>& nodePositions, std::vector<Triangle3D>& triangles) = 0;
	virtual const bool GetClothMDI(const size_t& rowCount, const size_t& columnCount, const std::vector<Vector3D>& nodePositions, ModelDrawInfo& mdi) = 0;
};
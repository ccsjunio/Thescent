#pragma once
#include <MathLibrary.h>
#include <vector>

class ICloth
{
protected:
	ICloth() {};

public:
	virtual ~ICloth() {};

	virtual const bool GetRowCount(size_t& rowCount) = 0;
	virtual const bool GetColumnCount(size_t& columnCount) = 0;
	virtual const bool GetNodeCount(size_t& nodeCount) = 0;
	virtual const bool GetNodePosition(const size_t& nodeIndex, Vector3D& position) = 0;
	virtual const bool GetNodePositions(std::vector<Vector3D>& positions) = 0;
	virtual const bool GetNodeWorldMatrix(const size_t& nodeIndex, Matrix4D& transformMatrix) = 0;
	virtual const bool GetClothWorldMatrix(Matrix4D& transformMatrix) = 0;
};
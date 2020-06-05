#pragma once
#include <ICloth.h>
#include <Entity.h>

class Cloth : public ICloth, public Entity
{
private:
	bool isActive;
	std::string name;

public:
	Cloth();
	~Cloth();

	void Activate(const bool& value);
	const bool IsActive();
	const std::string GetName();
	void SetName(const std::string& name);
	const unsigned long long GetTypeID();
	const std::string GetType();
	void Dispose();
	ICloneable* Clone();
	const bool GetRowCount(size_t& rowCount);
	const bool GetColumnCount(size_t& columnCount);
	const bool GetNodeCount(size_t& nodeCount);
	const bool GetNodePosition(const size_t& nodeIndex, Vector3D& position);
	const bool GetNodePositions(std::vector<Vector3D>& positions);
	const bool GetNodeWorldMatrix(const size_t& nodeIndex, Matrix4D& transformMatrix);
	const bool GetClothWorldMatrix(Matrix4D& transformMatrix);
};


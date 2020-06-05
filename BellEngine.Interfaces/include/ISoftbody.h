#pragma once
#include "IComponent.h"
#include <MathLibrary.h>
#include "ITransformable.h"
#include "ICloneable.h"
#include "IUpdatable.h"
#include "ITransform.h"
#include "IPhysicsComponent.h"
#include "ParameterContainer.h"

class ISoftbody :
	public IPhysicsComponent,
	public ITransformable,
	public IUpdatable
{
protected:
	ISoftbody() {};

public:
	virtual ~ISoftbody() {};

	virtual const size_t GetRowCount() = 0;
	virtual const size_t GetColumnCount() = 0;
	virtual const size_t GetNodeCount() = 0;
	virtual const bool GetNodeRadius(const unsigned int& nodeIndex, float& radius) = 0;
	virtual const bool GetNodePositions(std::vector<Vector3D>& positions) = 0;
	virtual const bool GetNodePosition(const unsigned int& nodeIndex, Vector3D& position) = 0;
	virtual const bool GetNodeWorldMatrix(const size_t& nodeIndex, Matrix4D& transformMatrix) = 0;
	virtual const bool RunCommand(ParameterContainer& input, ParameterContainer& output) = 0;
};
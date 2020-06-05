#include "Cloth.h"
#include <ISoftbody.h>
#include <BellEngineID.h>

Cloth::Cloth()
{
	this->isActive = true;
}

Cloth::~Cloth()
{
	Dispose();
}

void Cloth::Activate(const bool& value)
{
	this->isActive = value;
}

const bool Cloth::IsActive()
{
	return this->isActive;
}

const std::string Cloth::GetName()
{
	return this->name;
}

void Cloth::SetName(const std::string& name)
{
	this->name = name;
}

const unsigned long long Cloth::GetTypeID()
{
	return ENGINE_OBJ_ID_SCN_CLOTH;
}

const std::string Cloth::GetType()
{
	return "Cloth";
}

void Cloth::Dispose()
{
	//DisposeVector(components);
}

ICloneable* Cloth::Clone()
{
	Cloth* clone = new Cloth();

	std::vector<IComponent*> components;
	if (GetComponents(components))
	{
		for (IComponent*& component : components)
		{
			IComponent* c = (IComponent*)component->Clone();
			if (!clone->AddComponent(c))
			{
				//Couldn't clone or add component
			}
		}
	}	

	return clone;
}

const bool Cloth::GetRowCount(size_t& rowCount)
{
	ISoftbody* softbody = GetComponent<ISoftbody*>();
	if (softbody)
	{
		rowCount = softbody->GetRowCount();
		return true;
	}

	return false;
}

const bool Cloth::GetColumnCount(size_t& columnCount)
{
	ISoftbody* softbody = GetComponent<ISoftbody*>();
	if (softbody)
	{
		columnCount = softbody->GetColumnCount();
		return true;
	}

	return false;
}

const bool Cloth::GetNodeCount(size_t& nodeCount)
{
	ISoftbody* softbody = GetComponent<ISoftbody*>();
	if (softbody)
	{
		nodeCount = softbody->GetNodeCount();
		return true;
	}

	return false;
}

const bool Cloth::GetNodePosition(const size_t& nodeIndex, Vector3D& position)
{
	ISoftbody* softbody = GetComponent<ISoftbody*>();
	if (softbody)
	{
		return softbody->GetNodePosition(nodeIndex, position);
	}

	return false;
}

const bool Cloth::GetNodePositions(std::vector<Vector3D>& positions)
{
	ISoftbody* softbody = GetComponent<ISoftbody*>();
	if (softbody)
	{
		return softbody->GetNodePositions(positions);
	}

	return false;
}

const bool Cloth::GetNodeWorldMatrix(const size_t& nodeIndex, Matrix4D& transformMatrix)
{
	ISoftbody* softbody = GetComponent<ISoftbody*>();
	if (softbody)
	{
		return softbody->GetNodeWorldMatrix(nodeIndex, transformMatrix);
	}

	return false;
}

const bool Cloth::GetClothWorldMatrix(Matrix4D& transformMatrix)
{
	ISoftbody* softbody = GetComponent<ISoftbody*>();
	if (softbody)
	{
		transformMatrix = softbody->GetWorldMatrix();
		return true;
	}

	return false;
}

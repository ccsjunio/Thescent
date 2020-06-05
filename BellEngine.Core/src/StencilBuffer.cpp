#include "StencilBuffer.h"
#include <BellEngineID.h>

StencilBuffer::StencilBuffer()
{

}
StencilBuffer::~StencilBuffer()
{
	Dispose();
}

const std::string StencilBuffer::GetName()
{
	return this->name;
}

void StencilBuffer::SetName(const std::string& name)
{
	this->name = name;
}

const unsigned long long StencilBuffer::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_STENCIL;
}

const std::string StencilBuffer::GetType()
{
	return "StencilBuffer";
}

void StencilBuffer::SetTarget(const std::string& gameObject)
{
	target = gameObject;
}

const std::string StencilBuffer::GetTarget()
{
	return target;
}

void StencilBuffer::SetParent(const std::string& parent)
{
	this->parent = parent;
}

const std::string StencilBuffer::GetParent()
{
	return parent;
}

const bool StencilBuffer::AddPassID(const int& passID)
{
	auto it = std::find(passIDs.begin(), passIDs.end(), passID);
	if (it == passIDs.end())
	{
		passIDs.push_back(passID);
		return true;
	}

	return false;
}

const bool StencilBuffer::RemovePassID(const int& passID)
{
	auto it = std::find(passIDs.begin(), passIDs.end(), passID);
	if (it != passIDs.end())
	{
		passIDs.erase(it);
		return true;
	}

	return false;
}

const std::vector<int> StencilBuffer::GetPassIDs()
{
	return passIDs;
}

void StencilBuffer::Dispose()
{
}

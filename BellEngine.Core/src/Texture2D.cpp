#include "Texture2D.h"
#include <BellEngineID.h>

Texture2D::Texture2D()
{
	this->id = 0;
	this->isActive = true;
}

Texture2D::~Texture2D()
{
}

void Texture2D::Activate(const bool& value)
{
	this->isActive = value;
}

const bool Texture2D::IsActive()
{
	return this->isActive;
}

const std::string Texture2D::GetName()
{
	return this->name;
}

void Texture2D::SetName(const std::string& name)
{
	this->name = name;
}

const std::string Texture2D::GetPath()
{
	return path;
}

void Texture2D::SetPath(const std::string& path)
{
	this->path = path;
}

const std::string Texture2D::GetType()
{
	return "Texture2D";
}

const unsigned int Texture2D::GetTextureTypeID()
{
	return 1;
}

ICloneable* Texture2D::Clone()
{
	Texture2D* clone = new Texture2D();

	clone->name = name;
	clone->path = path;

	return clone;
}

void Texture2D::SetID(const unsigned int& id)
{
	this->id = id;
}

const unsigned int Texture2D::GetID()
{
	return id;
}

const unsigned long long Texture2D::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_TEXTURE2D;
}

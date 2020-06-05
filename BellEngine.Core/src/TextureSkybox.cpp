#include "TextureSkybox.h"
#include <BellEngineID.h>

TextureSkybox::TextureSkybox()
{
	this->id = 0;
	this->isActive = true;
}

TextureSkybox::~TextureSkybox()
{
}

void TextureSkybox::SetPositiveY(const std::string& path)
{
	this->positiveY = path;
}

void TextureSkybox::SetNegativeY(const std::string& path)
{
	this->negativeY = path;
}

void TextureSkybox::SetNegativeX(const std::string& path)
{
	this->negativeX = path;
}

void TextureSkybox::SetPositiveX(const std::string& path)
{
	this->positiveX = path;
}

void TextureSkybox::SetPositiveZ(const std::string& path)
{
	this->positiveZ = path;
}

void TextureSkybox::SetNegativeZ(const std::string& path)
{
	this->negativeZ = path;
}

const std::string TextureSkybox::GetPositiveY()
{
	return positiveY;
}

const std::string TextureSkybox::GetNegativeY()
{
	return negativeY;
}

const std::string TextureSkybox::GetNegativeX()
{
	return negativeX;
}

const std::string TextureSkybox::GetPositiveX()
{
	return positiveX;
}

const std::string TextureSkybox::GetPositiveZ()
{
	return positiveZ;
}

const std::string TextureSkybox::GetNegativeZ()
{
	return negativeZ;
}

void TextureSkybox::Activate(const bool& value)
{
	this->isActive = value;
}

const bool TextureSkybox::IsActive()
{
	return this->isActive;
}

const std::string TextureSkybox::GetName()
{
	return this->name;
}

void TextureSkybox::SetName(const std::string& name)
{
	this->name = name;
}

const std::string TextureSkybox::GetType()
{
	return "TextureSkybox";
}

const unsigned int TextureSkybox::GetTextureTypeID()
{
	return 3;
}

ICloneable* TextureSkybox::Clone()
{
	return nullptr;
}

void TextureSkybox::SetID(const unsigned int& id)
{
	this->id = id;
}

const unsigned int TextureSkybox::GetID()
{
	return this->id;
}

const unsigned long long TextureSkybox::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_TEXTURESKYBOX;
}
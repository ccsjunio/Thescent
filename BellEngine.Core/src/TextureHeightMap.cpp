#include "TextureHeightMap.h"
#include <BellEngineID.h>

TextureHeightMap::TextureHeightMap()
{
	allowOffsetIncrements = false;
	swizzle = Vector2D(0.0f, 0.0f);
	offsetIncrement = Vector2D(0.0f, 0.0f);
	ratio = 0.0f;
	offset = Vector2D(0.0f, 0.0f);
	this->isActive = true;
}

TextureHeightMap::~TextureHeightMap()
{
}

void TextureHeightMap::SetID(const unsigned int& id)
{
	this->id = id;
}

const unsigned int TextureHeightMap::GetID()
{
	return this->id;
}

void TextureHeightMap::IncrementOffset()
{
	if (allowOffsetIncrements)
	{
		offset += offsetIncrement;
	}
}

Vector2D TextureHeightMap::GetOffset()
{
	return offset;
}

void TextureHeightMap::Activate(const bool& value)
{
	this->isActive = value;
}

const bool TextureHeightMap::IsActive()
{
	return this->isActive;
}

const std::string TextureHeightMap::GetName()
{
	return this->name;
}

void TextureHeightMap::SetName(const std::string& name)
{
	this->name = name;
}

const std::string TextureHeightMap::GetType()
{
	return "TextureHeightMap";
}

const unsigned long long TextureHeightMap::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_TEXTUREHEIGHMAP;
}

const unsigned int TextureHeightMap::GetTextureTypeID()
{
	return 2;
}

ICloneable* TextureHeightMap::Clone()
{
	TextureHeightMap* clone = new TextureHeightMap();

	clone->allowOffsetIncrements = allowOffsetIncrements;
	clone->name = name;
	clone->path = path;
	clone->offset = offset;
	clone->offsetIncrement = offsetIncrement;
	clone->ratio = ratio;
	clone->swizzle = swizzle;

	return clone;
}

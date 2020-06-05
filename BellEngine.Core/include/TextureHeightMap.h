#pragma once
#include <ITexture.h>
#include <MathLibrary.h>

class TextureHeightMap : public ITexture
{
private:
	Vector2D offset;
	unsigned int id;
	bool isActive;
	std::string name;

public:
	bool allowOffsetIncrements;
	Vector2D swizzle;
	Vector2D offsetIncrement;	
	float ratio;

	TextureHeightMap();
	~TextureHeightMap();

	void SetID(const unsigned int& id);
	const unsigned int GetID();

	void IncrementOffset();
	Vector2D GetOffset();
	void Activate(const bool& value);
	const bool IsActive();
	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetType();
	const unsigned long long GetTypeID();
	const unsigned int GetTextureTypeID();
	ICloneable* Clone();
};


#pragma once
#include <ITexture.h>

class Texture2D : public ITexture
{
private:
	bool isActive;
	unsigned int id;
	std::string name;

public:
	Texture2D();
	~Texture2D();

	void Activate(const bool& value);
	const bool IsActive();
	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetPath();
	void SetPath(const std::string& path);
	const std::string GetType();
	const unsigned int GetTextureTypeID();
	ICloneable* Clone();
	void SetID(const unsigned int& id);
	const unsigned int GetID();
	const unsigned long long GetTypeID();
};


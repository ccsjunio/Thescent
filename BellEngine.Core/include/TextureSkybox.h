#pragma once
#include <ITexture.h>
#include <vector>

class TextureSkybox : public ITexture
{
private:
	bool isActive;
	unsigned int id;
	std::string positiveY, negativeY, negativeX, positiveX, positiveZ, negativeZ;
	std::string name;

public:
	TextureSkybox();
	~TextureSkybox();

	void SetPositiveY(const std::string& path);
	void SetNegativeY(const std::string& path);
	void SetNegativeX(const std::string& path);
	void SetPositiveX(const std::string& path);
	void SetPositiveZ(const std::string& path);
	void SetNegativeZ(const std::string& path);
	const std::string GetPositiveY();
	const std::string GetNegativeY();
	const std::string GetNegativeX();
	const std::string GetPositiveX();
	const std::string GetPositiveZ();
	const std::string GetNegativeZ();

	void Activate(const bool& value);
	const bool IsActive();
	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetType();
	const unsigned int GetTextureTypeID();
	const unsigned long long GetTypeID();
	ICloneable* Clone();
	void SetID(const unsigned int& id);
	const unsigned int GetID();
};


#pragma once
#include <string>
#include <IIdentifiable.h>
#include "ICloneable.h"

class ITexture : public IIdentifiable, public ICloneable
{
protected:
	ITexture() {};

public:
	virtual ~ITexture() {};

	std::string path;	

	//virtual void SetPath() = 0;
	//virtual std::string GetPath() = 0;
	virtual void Activate(const bool& value) = 0;
	virtual const bool IsActive() = 0;
	virtual void SetID(const unsigned int& id) = 0;
	virtual const unsigned int GetID() = 0;
	virtual const unsigned int GetTextureTypeID() = 0;
};
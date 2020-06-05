#pragma once
#include <IIdentifiable.h>

class IShader : public IIdentifiable
{
protected:
	IShader() {};

public:
	virtual ~IShader() {};

	virtual void SetVersion(const unsigned int& value) = 0;
	virtual const unsigned int GetVersion() = 0;
	virtual void SetTypeID(const unsigned long long& typeID) = 0;
	virtual void SetContent(const std::string& value) = 0;
	virtual const std::string GetContent() = 0;
	virtual const std::string GetPath() = 0;
	virtual const unsigned int GetHandlerID() = 0;
	virtual void SetHandlerID(const unsigned int& value) = 0;
	virtual void SetPath(const std::string& value) = 0;
};

#pragma once
#include <string>
#include <IComponent.h>

class IIdentityComponent : public IComponent
{
protected:
	IIdentityComponent() {};

public:
	~IIdentityComponent() {};

	virtual void SetTypeID(const unsigned long long& typeID) = 0;
	virtual const unsigned long long GetTypeID() = 0;
	virtual void SetTypeStr(const std::string& typeStr) = 0;
	virtual const std::string GetType() = 0;
	virtual void SetName(const std::string& name) = 0;
	virtual const std::string GetName() = 0;
};
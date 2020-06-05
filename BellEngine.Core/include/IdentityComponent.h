#pragma once
#include <IIdentityComponent.h>

#ifdef __linux__ 
#include <mutex>
#elif _WIN32
#define NOMINMAX
#include <windows.h>
#else

#endif

class IdentityComponent : public IIdentityComponent
{
private:
#ifdef __linux__ 
	std::mutex mutex;
#elif _WIN32
	CRITICAL_SECTION lock;
#else

#endif

	unsigned long long typeID;
	std::string typeStr;
	std::string name;

public:
	IdentityComponent();
	~IdentityComponent();

	void SetTypeID(const unsigned long long& typeID);
	const unsigned long long GetTypeID();
	void SetTypeStr(const std::string& typeStr);
	const std::string GetType();
	void SetName(const std::string& name);
	const std::string GetName();
};


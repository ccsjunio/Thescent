#include "IdentityComponent.h"

IdentityComponent::IdentityComponent()
{
#ifdef __linux__ 

#elif _WIN32
	InitializeCriticalSection(&lock);
#endif

	typeID = 0ull;
}

IdentityComponent::~IdentityComponent()
{
#ifdef __linux__ 

#elif _WIN32
	DeleteCriticalSection(&lock);
#endif
}

void IdentityComponent::SetTypeID(const unsigned long long& typeID)
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	this->typeID = typeID;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}

const unsigned long long IdentityComponent::GetTypeID()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	unsigned long long i = typeID;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return i;
}

void IdentityComponent::SetTypeStr(const std::string& typeStr)
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	this->typeStr = typeStr;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}

const std::string IdentityComponent::GetType()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	std::string s = typeStr;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return s;
}

const std::string IdentityComponent::GetName()
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	std::string s = name;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return s;
}

void IdentityComponent::SetName(const std::string& name)
{
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	typeStr = name;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}

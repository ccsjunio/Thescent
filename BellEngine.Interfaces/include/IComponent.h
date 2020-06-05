#pragma once
#include "IIdentifiable.h"
#include "ICloneable.h"
#include "IDisposable.h"
#include <vector>

#ifdef __linux__ 
#include <mutex>
#elif _WIN32
#define NOMINMAX
#include <windows.h>
#else

#endif

/*Defines an engine component, which should be attached to entities.*/
class IComponent : public IIdentifiable, public ICloneable, public IDisposable
{
protected:
#ifdef __linux__ 
	/*Cross-Thread protection. Crossplatform.*/
	std::mutex mutex;
#elif _WIN32
	/*Cross-Thread protection. Windows only (faster).*/
	CRITICAL_SECTION lock;
#else

#endif
	/*Holds references to this component's parents.*/
	std::vector<void*> parents;

	/*Abstract constructor.*/
	IComponent() 
	{
#ifdef __linux__ 

#elif _WIN32
		InitializeCriticalSection(&lock);
#endif
	};

public:
	/*Abstract destructor.*/
	~IComponent() 
	{
#ifdef __linux__ 

#elif _WIN32
		DeleteCriticalSection(&lock);
#endif
	};

	/*States wheter the component is active or not.*/
	virtual const bool IsActive() = 0;
	/*Activates or deactivates the component.*/
	virtual void Activate(const bool& option = true) = 0;
	/*Inserts a parent in the component.*/
	template<typename T> 
	inline const bool AddParent(T parent)
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		bool rc = false;
		if (parent)
		{
			auto it = std::find(parents.begin(), parents.end(), parent);
			if (it == parents.end())
			{
				parents.push_back(parent);
				rc = true;
			}
		}

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return rc;
	}
	/*Removes a parent from the component.*/
	template<typename T> 
	inline const bool RemoveParent(T parent)
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		bool rc = false;
		if (parent)
		{
			auto it = std::find(parents.begin(), parents.end(), parent);
			if (it != parents.end())
			{
				parents.erase(it);
				rc = true;
			}
		}

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return rc;
	}
	/*Returns a parent of the component.*/
	template<typename T> 
	inline T GetParent()
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		T ptr = nullptr;
		for (auto p : this->parents)
		{
			T ptr_ = static_cast<T>(p);
			if (ptr_)
			{
				ptr = ptr_;
				break;
			}
		}

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return ptr;
	}
	/*Returns all parents of the component that match a specified type.*/
	template<typename T> 
	inline const bool GetParents(std::vector<T>& parents)
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		T ptr = nullptr;
		for (auto p : this->parents)
		{
			ptr = dynamic_cast<T>(p);
			if (ptr)
			{
				parents.push_back(ptr);
			}
		}

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return !parents.empty();
	}
	/*Returns all parents of the component.*/
	inline const bool GetParents(std::vector<void*> parents)
	{
#ifdef __linux__ 
		mutex.lock();
#elif _WIN32
		EnterCriticalSection(&lock);
#endif

		parents = this->parents;

#ifdef __linux__ 
		mutex.unlock();
#elif _WIN32
		LeaveCriticalSection(&lock);
#endif

		return !parents.empty();
	}
};
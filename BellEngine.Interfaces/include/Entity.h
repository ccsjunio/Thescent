#pragma once
#include <IIdentifiable.h>
#include <ICloneable.h>
#include <IDisposable.h>
#include <IExtendedInfo.h>
#include <IComponent.h>
#include <ISelectable.h>

#ifdef __linux__ 
#include <mutex>
#elif _WIN32
#define NOMINMAX
#include <windows.h>
#else

#endif

/*Defines an general purpose ECS-object for the engine.*/
class Entity :
	public IIdentifiable, 
	public ICloneable, 
	public IDisposable,
	public IExtendedInfo, 
	public ISelectable
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
	/*Holds all parents of this entity.*/
	std::vector<void*> parents;
	/*Holds all components in this entity.*/
	std::vector<IComponent*> components;
	/*The name of the entity.*/
	std::string name;
	/*The type name of the entity.*/
	std::string typeStr;
	/*The type ID of the entity.*/
	unsigned long long typeID;
	/*A global unique ID for all entities.*/
	static unsigned long long globalID;
	/*The unique ID of the entity.*/
	unsigned long long uniqueId;

public:
	/*Constructor.*/
	Entity();
	/*Destructor.*/
	~Entity();

	/*Return the unique identifier for this entity.*/
	unsigned long long GetUniqueID();
	/*Inserts a parent for the entity.*/
	template<class T> 
	const bool AddParent(T*& ptr);
	/*Remove a parent from the entity.*/
	template<class T> 
	const bool RemoveParent(T*& ptr);
	/*Gets a parent for the entity.*/
	template<typename T> 
	T GetParent();
	/*Inserts a component on the entity.*/
	template<class T> 
	const bool AddComponent(T*& ptr);	
	/*Removes a component on the entity.*/
	template<class T> 
	const bool RemoveComponent(T*& ptr, const bool& deletePtr = true);
	/*Inserts a component on the entity.*/
	template<typename T> 
	T GetComponent();
	/*Returns all components of a given type.*/
	template<typename T> 
	std::vector<T>* GetComponents();
	/*Returns all components of a given type.*/
	template<typename T> 
	const bool GetComponents(std::vector<T>& output);
	/*Checks if the entity has a component matching the given type.*/
	template<typename T> 
	const bool HasComponent();
	/*Returns a component matching a given type.*/
	IComponent* GetComponentByName(const std::string& name);
	/*Returns a component matching a given type.*/
	IComponent* GetComponent(const unsigned long long& typeID);
	/*Gets all components matching the given type. Returns success or failure.*/
	const bool GetComponents(std::vector<IComponent*>& v, const unsigned long long& typeID);
	/*Checks if the entity has a component matching the given type.*/
	const bool HasComponent(const unsigned long long& typeID);
	/*Returns a component matching the given type.*/
	IComponent* GetComponentByTypeName(const std::string& typeStr);
	/*Returns all components matching the given type.*/
	const bool GetComponents(std::vector<IComponent*>& v, const std::string& typeStr);
	/*Checks if the entity has a component matching the given type.*/
	const bool HasComponent(const std::string& typeStr);
	/*Sets the type id for the entity.*/
	void SetTypeID(const unsigned long long& typeID);
	/*Returns the type id for the entity.*/
	const unsigned long long GetTypeID();
	/*Sets the type name for the entity.*/
	void SetTypeStr(const std::string& typeStr);
	/*Returns the type name of the entity.*/
	const std::string GetType();
	/*Sets a name for the entity.*/
	void SetName(const std::string& name);
	/*Returns the name of this entity.*/
	const std::string GetName();
	/*Creates a clone of this instance.*/
	ICloneable* Clone();
	/*Releases resources.*/
	void Dispose();
};
/*Inserts a parent for the entity.*/
template<class T> 
inline const bool Entity::AddParent(T*& ptr)
{
	if (!this) return false;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	bool rc = false;
	/*Pointer is not nullptr*/
	if (ptr)
	{
		/*Check if component is already in the vector.*/
		auto it = std::find(parents.begin(), parents.end(), ptr);
		if (it == parents.end())
		{
			/*Add component to the vector*/
			parents.push_back(ptr);

			/*Return success.*/
			rc = true;
		}
		else
		{
			SetLastErrorString("Could not find specified component.");
		}
	}
	else
	{
		SetLastErrorString("Pointer is nullptr.");
	}
	
#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return rc;
}
/*Remove a parent from the entity.*/
template<class T> 
inline const bool Entity::RemoveParent(T*& ptr)
{
	if (!this) return false;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	bool rc = false;
	/*Pointer is not nullptr*/
	if (ptr)
	{
		/*Check if parent is already in the vector.*/
		auto it = std::find(parents.begin(), parents.end(), ptr);
		if (it != parents.end())
		{
			/*Remove component from the vector*/
			parents.erase(it);
			/*Resizes the vector*/
			parents.shrink_to_fit();
			/*Return success.*/
			rc = true;
		}
		else
		{
			SetLastErrorString("Could not find specified parent.");
		}
	}
	else
	{
		SetLastErrorString("Pointer is nullptr.");
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return rc;
}
/*Gets a parent for the entity.*/
template<typename T>
inline T Entity::GetParent()
{
	if (!this) return nullptr;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	T _ptr = nullptr;
	for (void*& parent : parents)
	{
		T ptr = static_cast<T>(parent);
		if (ptr)
		{
			_ptr = ptr;
			break;
		}
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return _ptr;
}
/*Inserts a component on the entity.*/
template<class T> 
inline const bool Entity::AddComponent(T*& ptr)
{
	if (!this) return false;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	bool rc = false;
	/*Pointer is not nullptr*/
	if (ptr)
	{
		/*Pointer is IComponent*/
		IComponent* component = dynamic_cast<IComponent*>(ptr);
		if (component)
		{
			/*Check if component is already in the vector.*/
			auto it = std::find(components.begin(), components.end(), component);
			if (it == components.end())
			{
				/*Add component to the vector*/
				components.push_back(component);

				/*Return success.*/
				rc = true;
			}
			else
			{
				SetLastErrorString("Could not find specified component.");
			}
		}
		else
		{
			SetLastErrorString("Pointer is not a valid IComponent.");
		}
	}
	else
	{
		SetLastErrorString("Pointer is nullptr.");
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return rc;
}
/*Removes a component on the entity.*/
template<class T> 
inline const bool Entity::RemoveComponent(T*& ptr, const bool& deletePtr)
{
	if (!this) return false;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	bool rc = false;
	/*Pointer is not nullptr*/
	if (ptr)
	{
		/*Pointer is IComponent*/
		IComponent* component = dynamic_cast<IComponent*>(ptr);
		if (component)
		{
			/*Check if component is already in the vector.*/
			auto it = std::find(components.begin(), components.end(), component);
			if (it != components.end())
			{
				/*Remove component from the vector*/
				components.erase(it);
				/*Resizes the vector*/
				components.shrink_to_fit();

				/*Check if should delete the pointer also.*/
				if (deletePtr)
				{
					if (ptr)
					{
						delete (ptr);
						ptr = nullptr;
					}
				}

				/*Return success.*/
				rc = true;
			}
			else
			{
				SetLastErrorString("Could not find specified component.");
			}
		}
		else
		{
			SetLastErrorString("Pointer is not a valid IComponent.");
		}
	}
	else
	{
		SetLastErrorString("Pointer is nullptr.");
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return rc;
}
/*Gets a component for the entity.*/
template<typename T> 
inline T Entity::GetComponent()
{
	if (!this) return nullptr;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	T _ptr = nullptr;
	for (auto& component : components)
	{
		T ptr = dynamic_cast<T>(component);
		if (ptr)
		{
			_ptr = ptr;
			break;
		}
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return _ptr;
}
/*Returns all components of a given type.*/
template<typename T> 
inline std::vector<T>* Entity::GetComponents()
{
	if (!this) return nullptr;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	std::vector<T>* ptr = &components;
	
#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return ptr;
}
/*Returns all components of a given type.*/
template<typename T> 
inline const bool Entity::GetComponents(std::vector<T>& output)
{
	if (!this) return false;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	output.clear();
	output.shrink_to_fit();

	for (auto& component : components)
	{
		T ptr = dynamic_cast<T>(component);
		if (ptr)
		{
			output.push_back(ptr);
		}
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return !output.empty();
}
/*Checks if the entity has a component matching the given type.*/
template<typename T> 
inline const bool Entity::HasComponent()
{
	if (!this) return nullptr;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	bool rc = false;
	for (auto& component : components)
	{
		T ptr = dynamic_cast<T>(component);
		if (ptr)
		{
			rc = true;
			break;
		}
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return rc;
}

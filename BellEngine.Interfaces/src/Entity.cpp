#include "Entity.h"
#include <StringHelpers.h>

/*The unique ID of the entity.*/
unsigned long long Entity::globalID = 0;
/*Constructor.*/
Entity::Entity()
{
#ifdef __linux__ 

#elif _WIN32
	InitializeCriticalSection(&lock);
#endif

	this->typeID = 0ull;

	this->uniqueId = globalID;
	if (this->uniqueId == ULLONG_MAX - 1)
	{
		globalID = 0;
	}
	else
	{
		globalID++;
	}	
}
/*Destructor*/
Entity::~Entity()
{
	Dispose();

#ifdef __linux__ 

#elif _WIN32
	DeleteCriticalSection(&lock);
#endif
}
/*Return the unique identifier for this entity.*/
unsigned long long Entity::GetUniqueID()
{
	if (!this) return 0ull;

	return this->uniqueId;
}
/*Returns a component matching a given type.*/
IComponent* Entity::GetComponentByName(const std::string& name)
{
	if (!this) return nullptr;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	IComponent* ptr = nullptr;
	for (IComponent*& component : components)
	{
		if (component->GetName() == name)
		{
			ptr = component;
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
/*Returns a component matching a given type.*/
IComponent* Entity::GetComponent(const unsigned long long& typeID)
{
	if (!this) return nullptr;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	IComponent* ptr = nullptr;
	for (IComponent*& component : components)
	{
		if (component->GetTypeID() == typeID)
		{
			ptr = component;
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
/*Gets all components matching the given type. Returns success or failure.*/
const bool Entity::GetComponents(std::vector<IComponent*>& v, const unsigned long long& typeID)
{
	if (!this) return false;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	for (IComponent*& component : components)
	{
		if (component->GetTypeID() == typeID)
		{
			v.push_back(component);
		}
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return !v.empty();
}
/*Checks if the entity has a component matching the given type.*/
const bool Entity::HasComponent(const unsigned long long& typeID)
{
	if (!this) return false;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	bool rc = false;
	for (IComponent*& component : components)
	{
		if (component->GetTypeID() == typeID)
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
/*Returns a component matching a given type.*/
IComponent* Entity::GetComponentByTypeName(const std::string& typeStr)
{
	if (!this) return nullptr;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	IComponent* ptr = nullptr;
	for (IComponent*& component : components)
	{
		if (component->GetType() == typeStr)
		{
			ptr = component;
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
/*Gets all components matching the given type. Returns success or failure.*/
const bool Entity::GetComponents(std::vector<IComponent*>& v, const std::string& typeStr)
{
	if (!this) return false;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	for (IComponent*& component : components)
	{
		if (component->GetType() == typeStr)
		{
			v.push_back(component);
		}
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return !v.empty();
}
/*Checks if the entity has a component matching the given type.*/
const bool Entity::HasComponent(const std::string& typeStr)
{
	if (!this) return false;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	bool rc = false;
	for (IComponent*& component : components)
	{
		if (component->GetType() == typeStr)
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
/*Sets the type id for the entity.*/
void Entity::SetTypeID(const unsigned long long& id)
{
	if (!this) return;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	this->typeID = id;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}
/*Returns the type id for the entity.*/
const unsigned long long Entity::GetTypeID()
{
	if (!this) return 0ull;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	unsigned long long i = typeID;;

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return i;
}
/*Sets the type name for the entity.*/
void Entity::SetTypeStr(const std::string& typeStr)
{
	if (!this) return;

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
/*Returns the type name of the entity.*/
const std::string Entity::GetType()
{
	if (!this) return "";

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
/*Sets a name for the entity.*/
void Entity::SetName(const std::string& name)
{
	if (!this) return;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	this->name = name;
	
#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}
/*Returns the name of this entity.*/
const std::string Entity::GetName()
{
	if (!this) return "";

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
/*Creates a clone of this instance.*/
ICloneable* Entity::Clone()
{
	if (!this) return nullptr;

#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	Entity* clone = nullptr;
	clone = new Entity();
	clone->name = name + "_" + StringHelpers::GenerateRandomString(6);
	clone->typeID = typeID;
	clone->typeStr = typeStr;

	/*Tries to clone all components*/
	for (IComponent* component : components)
	{
		if (component)
		{
			ICloneable* clonedComponent = component->Clone();
			if (clone->AddComponent(clonedComponent))
			{
				/*Garantees Entity clone is a parent for all the cloned components.*/
				if (((IComponent*)clonedComponent)->AddParent(clone))
				{
					/*OK*/
				}
				else
				{
#ifdef DEBUG
					printf("Could not add parent.\n");
#endif // DEBUG
				}
			}
			else
			{
#ifdef DEBUG
				printf("Could not add component because: %s\n", GetLastErrorString().c_str());
#endif // DEBUG
			}

			continue;
		}
	}

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif

	return clone;
}
/*Releases resources.*/
void Entity::Dispose()
{
	if (!this) return
#ifdef __linux__ 
	mutex.lock();
#elif _WIN32
	EnterCriticalSection(&lock);
#endif

	DisposeVector(components);

#ifdef __linux__ 
	mutex.unlock();
#elif _WIN32
	LeaveCriticalSection(&lock);
#endif
}

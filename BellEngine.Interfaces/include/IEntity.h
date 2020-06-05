#pragma once
#include <vector>
#include "IComponent.h"
#include "IExtendedInfo.h"

class IEntity :
	public IComponent,
	public IExtendedInfo
{
protected:
	std::vector<IComponent*> components;

protected:
	IEntity() {};

public:
	virtual ~IEntity() {};

	template<class T>
	const bool AddComponent(T* ptr)
	{
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
					return true;
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
		
		return false;
	}
	
	template<class T>
	const bool RemoveComponent(T* ptr, const bool& deletePtr = true)
	{
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
					return true;
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

		return false;
	}
	
	template<typename T>
	inline T GetComponent()
	{
		for (auto& component : components)
		{
			T ptr = static_cast<T>(component);
			if (ptr)
			{
				return ptr;
			}
		}

		return nullptr;
	}

	template<class T>
	std::vector<T*>* GetComponents()
	{
		std::vector<T*>* ptr;

		ptr = &components;

		return ptr;
	}

	/*template<class T>
	std::vector<T*> GetComponents()
	{
		std::vector<T*> v;

		for (IComponent* component : components)
		{
			T* ptr = dynamic_cast<T*>(component);
			if (ptr)
			{
				v.push_back(ptr);
			}
		}

		return v;
	}*/

	template<class T>
	const bool GetComponents(std::vector<T*>& v)
	{
		v.clear();
		v.shrink_to_fit();

		for (IComponent* component : components)
		{
			T* ptr = dynamic_cast<T*>(component);
			if (ptr)
			{
				v.push_back(ptr);
			}
		}

		return !v.empty();
	}

	template<class T>
	const bool HasComponent()
	{
		for (IComponent* component : components)
		{
			T* ptr = dynamic_cast<T*>(component);
			if (ptr)
			{
				return true;
			}
		}

		return false;
	}
};
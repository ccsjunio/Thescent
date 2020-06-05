#pragma once
#include <vector>
#include <atomic>

/*The CyclicContainer is a custom container created to support Object Pools.*/
template<typename T>
class CyclicContainer
{
private:
	/*Underlying container of the Cyclic Container*/
	std::vector<T> internalContainer;
	/*False = Moves container Forward, True = Moves containter backward*/
	std::atomic<bool> direction;

public:
	/*Constructor.*/
	CyclicContainer()
	{
		SetDirection(false);
		Reserve(0ull);
	}	
	/*Destructor.*/
	~CyclicContainer()
	{
		Clear();
	}
	
	/*Adds an item to the container.*/
	const bool Add(T& value)
	{
		auto it = std::find(internalContainer.begin(), internalContainer.end(), value);
		if (it == internalContainer.end())
		{
			internalContainer.push_back(value);
			return true;
		}
	
		return false;
	}
	/*Removes an item from the container.*/
	const bool Remove(T& value)
	{
		auto it = std::find(internalContainer.begin(), internalContainer.end(), value);
		if (it != internalContainer.end())
		{
			internalContainer.erase(it);
			return true;
		}

		return false;
	}
	/*Sets the rotation direction of the container. False = Moves container Forward, True = Moves containter backward*/
	void SetDirection(const bool& value)
	{
		this->direction.store(value);
	}
	/*Rotates the stack and returns the item at the top.*/
	T Next()
	{
		if (!internalContainer.empty())
		{
			if (this->direction.load())
			{
				std::rotate(internalContainer.begin(), internalContainer.begin() - 1, internalContainer.end());
			}
			else
			{
				std::rotate(internalContainer.begin(), internalContainer.begin() + 1, internalContainer.end());
			}

			return internalContainer[0];
		}

		return nullptr;
	}
	/*Rotates the stack and returns the item at the bottom.*/
	T Last()
	{
		if (!internalContainer.empty())
		{
			if (!this->direction.load())
			{
				std::rotate(internalContainer.begin(), internalContainer.begin() - 1, internalContainer.end());
			}
			else
			{
				std::rotate(internalContainer.begin(), internalContainer.begin() + 1, internalContainer.end());
			}

			return internalContainer[internalContainer.size() - 1];
		}

		return nullptr;
	}
	/*Gets all items in the container.*/
	const bool GetItems(std::vector<T>& value)
	{
		value = internalContainer;
		return !value.empty();
	}
	/*Reserve a size for the underlying container.*/
	void Reserve(const size_t& default_size)
	{
		internalContainer.reserve(default_size);
	}
	/*Clears the contents of the underlying container.*/
	void Clear()
	{
		internalContainer.clear();
		internalContainer.shrink_to_fit();
	}
	/*Returns the number of elements in the container.*/
	const size_t Count()
	{
		return internalContainer.size();
	}
};
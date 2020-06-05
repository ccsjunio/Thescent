/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <vector>
#include <map>
#include <atomic>
class IDisposable
{
protected:
	std::atomic<bool> disposing;
	IDisposable() {};

public:
	virtual ~IDisposable() {};
	virtual void Dispose() = 0;

	void SetDisposing(const bool& value)
	{
		if (this)
		{
			disposing.store(value);
		}
	}
	const bool IsDisposing()
	{
		if (!this) return true;

		return disposing.load();
	}
	template<typename T> const bool DisposePointer(T*& pointer)
	{
		if (this)
		{
			if (pointer)
			{
				delete (pointer);
				pointer = nullptr;

				return true;
			}
		}

		return false;
	}
	template<typename K, typename T> const bool DisposeMap(std::map<K, T*>& map)
	{
		if (!this) return false;

		unsigned int timesFailed = 0;

		for (auto& element : map)
		{
			if (!DisposePointer(element.second))
			{
				timesFailed++;
			}
		}

		map.clear();

		return timesFailed == 0;
	}
	template<typename T> const bool DisposeVector(std::vector<T*>& vector)
	{
		if (!this) return false;

		unsigned int timesFailed = 0;

		for (auto& ptr : vector)
		{
			if (!DisposePointer(ptr))
			{
				timesFailed++;
			}
		}

		vector.clear();
		vector.shrink_to_fit();

		return timesFailed == 0;
	}
	template<typename K, typename T> const bool DisposeVector(std::vector<std::pair<K, T*>>& vector)
	{
		if (!this) return false;

		unsigned int timesFailed = 0;

		for (auto& pair : vector)
		{
			if (!DisposePointer(pair.second))
			{
				timesFailed++;
			}
		}

		vector.clear();
		vector.shrink_to_fit();

		return timesFailed == 0;
	}
	template<typename K, typename T> const bool DisposeVector(std::vector<std::pair<K*, T>>& vector)
	{
		if (!this) return false;

		unsigned int timesFailed = 0;

		for (auto& pair : vector)
		{
			if (!DisposePointer(pair.first))
			{
				timesFailed++;
			}
		}

		vector.clear();
		vector.shrink_to_fit();

		return timesFailed == 0;
	}
	template<typename K, typename T> const bool DisposeVector(std::vector<std::pair<K*, T*>>& vector)
	{
		if (!this) return false;

		unsigned int timesFailed = 0;

		for (auto& pair : vector)
		{
			if (!DisposePointer(pair.first) || !DisposePointer(pair.second))
			{
				timesFailed++;
			}
		}

		vector.clear();
		vector.shrink_to_fit();

		return timesFailed == 0;
	}
};
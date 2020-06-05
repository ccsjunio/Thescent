#pragma once
#include <string>
#include <map>
#include <MathLibrary.h>
#include <IIdentifiable.h>

class ISound : public IIdentifiable
{
protected:
	int m_UniqueID;
	static int m_NextUniqueID;
	std::string fileName;

	ISound() : m_UniqueID(0) {};

public:	
	enum class Source_e
	{
		Sampled,
		Streamed
	};

	virtual ~ISound() {};

	void SetFileName(const std::string& fileName)
	{
		this->fileName = fileName;
	}
	std::string GetFileName()
	{
		return this->fileName;
	}
	virtual std::string GetStateString() = 0;
	virtual int GetSourceState() = 0;
	virtual int GetSourceType() = 0;
	int GetUniqueID()
	{
		return m_UniqueID;
	}
};


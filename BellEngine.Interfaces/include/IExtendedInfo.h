#pragma once
#include <string>

class IExtendedInfo
{
protected:
	std::string lastErrorStr;
	long lastErrorID;

	IExtendedInfo() : lastErrorID(0), lastErrorStr("") {};

public:
	virtual ~IExtendedInfo() {};

	void SetLastErrorString(const std::string& errorMsg)
	{
		this->lastErrorStr = errorMsg;
	}
	const std::string GetLastErrorString()
	{
		return lastErrorStr;
	}
	const std::string GetLastErrorString(const bool& clearAfterRead)
	{
		std::string errorStr = this->lastErrorStr;

		if (clearAfterRead)
		{
			this->lastErrorStr = "";
		}

		return errorStr;
	}
	void SetLastErrorID(const long& errorID)
	{
		lastErrorID = errorID;
	}
	const long GetLastErrorID()
	{
		return lastErrorID;
	}
	void ClearErrors()
	{
		lastErrorID = 0;
		lastErrorStr = "";
	}
};
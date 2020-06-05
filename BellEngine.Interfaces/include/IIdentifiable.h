/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <string>

class IIdentifiable
{
protected:
	IIdentifiable() {};

public:
	virtual ~IIdentifiable() {};

	virtual const unsigned long long GetTypeID() = 0;
	virtual const std::string GetType() = 0;
	virtual const std::string GetName() = 0;
	virtual void SetName(const std::string& name) = 0;
};
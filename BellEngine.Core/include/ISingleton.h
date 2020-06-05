/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#include <cstdarg>

#pragma once
class ISingleton
{
protected:
	ISingleton() {}

public:
	virtual ~ISingleton() {};

	static void* GetInstance();
	static void* GetInstance(unsigned int numberOfArguments, ...);
};
/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <cstdarg>
#include "IDisposable.h"

class Message : public IDisposable
{
public:
	std::string header;
	std::vector<void*> parameters;

	Message(std::string header, unsigned int numberOfArguments, ...);
	~Message();

	void Dispose();
};


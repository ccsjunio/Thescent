#pragma once
#include "ParameterContainer.h"

class IFileReader
{
protected:
	IFileReader() {};

public:
	virtual ~IFileReader() {};

	virtual const bool ReadToParameterContainer(ParameterContainer& input, ParameterContainer& output) = 0;
};
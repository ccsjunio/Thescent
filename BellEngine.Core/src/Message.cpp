/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#include "Message.h"

Message::Message(std::string header, unsigned int numberOfArguments, ...)
{
	va_list arguments;

	va_start(arguments, numberOfArguments);

	for (unsigned int i = 0; i < numberOfArguments; i++)
	{
		parameters.push_back(va_arg(arguments, void*));
	}
	va_end(arguments);

	this->header = header;
}

Message::~Message()
{
	Dispose();
}

void Message::Dispose()
{
	header = "";
	parameters.clear();
	parameters.shrink_to_fit();
}

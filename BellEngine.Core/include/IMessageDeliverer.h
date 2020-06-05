/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include "Message.h"
class IMessageDeliverer
{
public:
	virtual ~IMessageDeliverer() {};
	virtual void Send(Message message) = 0;
};
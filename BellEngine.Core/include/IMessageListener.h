/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include "Message.h"
class IMessageListener
{
public:
	virtual ~IMessageListener() {};
	virtual void ReceiveMessage(Message message) = 0;
};
/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include "Message.h"
#include "IMessageDeliverer.h"
#include "IMessageListener.h"

class ICommunicable : public IMessageListener, public IMessageDeliverer
{
public:
	virtual ~ICommunicable() {};

	virtual void SendMessage(Message message) = 0;
	virtual void ReceiveMessage(Message message) = 0;
};
/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once
#include <map>
#include <string>
#include "ErrorCodes.h"

class BaseKeyBindings
{
protected:
	std::map<std::string, int> keyBindingMap;

public:
	BaseKeyBindings();
	~BaseKeyBindings();

	void RegisterKeyBind(std::string actionBound, int key);
	int GetKey(std::string actionBound, int &key);
	int GetAction(int key, std::string &actionBound);
	virtual void LoadKeys() = 0;
};


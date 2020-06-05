/***********************************************************************
* Project           : BellEngine
* Author            : Felipe Bellini
* Date created      : Oct. 30 - 2019
* Licensed under:	: MIT

************************************************************************/
#include "BaseKeyBindings.h"

BaseKeyBindings::BaseKeyBindings()
{
}

BaseKeyBindings::~BaseKeyBindings()
{
	keyBindingMap.clear();
}

void BaseKeyBindings::RegisterKeyBind(std::string actionBound, int key)
{
	keyBindingMap[actionBound] = key;
}

int BaseKeyBindings::GetKey(std::string actionBound, int& key)
{
	bool found = false;

	std::map<std::string, int>::iterator it = keyBindingMap.begin();
	for (std::pair<std::string, int> element : keyBindingMap)
	{
		if (element.first == actionBound)
		{
			key = element.second;
			found = true;
			break;
		}
	}

	return found ? SUCCESS : FAILURE;
}

int BaseKeyBindings::GetAction(int key, std::string& actionBound)
{
	bool found = false;

	std::map<std::string, int>::iterator it = keyBindingMap.begin();
	for (std::pair<std::string, int> element : keyBindingMap)
	{
		if (element.second == key)
		{
			actionBound = element.first;
			found = true;
			break;
		}
	}

	return found ? SUCCESS : FAILURE;
}

void BaseKeyBindings::LoadKeys()
{

}
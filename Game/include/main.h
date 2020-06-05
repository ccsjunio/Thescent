/***********************************************************************
* Project           : Game
* Author            : Felipe Bellini
* Date created      : Oct. 12 - 2019
* Licensed under:	: MIT

************************************************************************/
#pragma once

#ifdef __linux__ 

#elif _WIN32
	//Disables windows console
	#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#else

#endif

#include <GLCommon.h>
#include <FrameTime.h>
#include "GameWorld.h"

int main();
void UpdateFrameTime(FrameTime& frameTime);
static void error_callback(int error, const char* description);
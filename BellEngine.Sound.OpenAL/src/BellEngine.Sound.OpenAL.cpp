// BellEngine.Sound.OpenAL.cpp : Defines the functions for the static library.
//

#include "SystemSound.h"

#define DLLEXP  __declspec(dllexport)

extern "C"
{
    DLLEXP ISystemSound* __cdecl GetSystem()
    {
        return new SystemSound();
    }
}
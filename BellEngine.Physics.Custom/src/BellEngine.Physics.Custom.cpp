// BellEngine.Physics.Custom.cpp : Defines the functions for the static library.
//
#include "SystemPhysics.h"

#define DLLEXP  __declspec(dllexport)

extern "C"
{
    DLLEXP ISystemPhysics* __cdecl GetSystem()
    {
        return new SystemPhysics();
    }
}
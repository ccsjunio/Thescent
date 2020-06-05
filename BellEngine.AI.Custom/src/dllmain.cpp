// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "SystemAI.h"

#define DLLEXP  __declspec(dllexport)

extern "C"
{
    DLLEXP ISystemAI* __cdecl GetSystem()
    {
        return new SystemAI();
    }
}

//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//                     )
//{
//    switch (ul_reason_for_call)
//    {
//    case DLL_PROCESS_ATTACH:
//    case DLL_THREAD_ATTACH:
//    case DLL_THREAD_DETACH:
//    case DLL_PROCESS_DETACH:
//        break;
//    }
//    return TRUE;
//}


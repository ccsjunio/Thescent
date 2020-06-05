// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "LevelFactory.h"
#include "windows.h"
#include <MathLibrary.h>
#include "SystemDataPersistency.h"

#define DLLEXP  __declspec(dllexport)

extern "C"
{
    DLLEXP ISystemDataPersistency* __cdecl GetSystem()
    {
        ISystemDataPersistency* ptr = new SystemDataPersistency();
        if (!ptr)
        {
            printf("Could not create a SystemDataPersistency.\n");
        }

        return ptr;
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
//

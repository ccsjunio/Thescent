#include "SystemFactory.h"
#include <StringHelpers.h>

#define PHYSICS_CUSTOM_DLL		"BellEngine.Physics.Custom.dll"
#define PHYSICS_BULLET_DLL		"BellEngine.Physics.Bullet.dll"
#define SOUND_OPENAL_DLL		"BellEngine.Sound.OpenAL.dll"
#define MESH_ASSIMP_DLL			"BellEngine.Mesh.Assimp.dll"
#define MESH_PLYREADER_DLL		"BellEngine.Mesh.PlyReader.dll"
#define AI_CUSTOM_DLL			"BellEngine.AI.Custom.dll"
#define PERSISTENCY_XML_DLL		"Game.Persistency.XML.dll"
#define SCRIPTING_LUA_DLL		"BellEngine.Scripting.Lua.dll"

SystemFactory::SystemFactory()
{
    systemMesh = nullptr;
    systemUserControl = nullptr;
    systemGUI = nullptr;
    systemSound = nullptr;
    systemScene = nullptr;    
    systemVideo = nullptr;
    systemPhysics = nullptr;
    systemAI = nullptr;
    systemDataPersistency = nullptr;
    systemScriptingLua = nullptr;
    persistencyHInstance = NULL;
    aiHInstance = NULL;
    physicsHInstance = NULL;
    soundHInstance = NULL;
    luaScriptingHInstance = NULL;
}

SystemFactory::~SystemFactory()
{
    Dispose();
}

ISystem* SystemFactory::Create(ParameterContainer& parameters)
{
	return nullptr;
}

const bool SystemFactory::Initialize(ParameterContainer& input, ParameterContainer& output)
{
	return true;
}

ISystemAI* SystemFactory::GetAISystem(AISystem_e systemtype)
{
    std::string systemtypeStr;

    switch (systemtype)
    {
    case SystemFactory::AISystem_e::Custom:
        systemtypeStr = AI_CUSTOM_DLL;
        break;    
    default:
        systemtypeStr = AI_CUSTOM_DLL;
        break;
    }

    wchar_t* buf = StringHelpers::GetWCharBuffer(systemtypeStr);
    aiHInstance = ::LoadLibrary(buf);
    delete[] buf;

    if (aiHInstance)
    {
        GetSystemAI getSystem = reinterpret_cast<GetSystemAI>(::GetProcAddress(aiHInstance, "GetSystem"));
        if (getSystem)
        {
            systemAI = getSystem();
            printf("Using %s to perform AI.\n\n", systemtypeStr.c_str());
        }
        else
        {
            printf("Unable to load %s's AI!\n", systemtypeStr.c_str());
        }
    }
    else
    {
        printf("Unable to load %s!\n", systemtypeStr.c_str());
    }

    return systemAI;
}

const bool SystemFactory::DisposeAISystem()
{
    bool rc = DisposePointer(systemAI);
    if (!rc) return false;

    if (!FreeLibrary(aiHInstance))
    {
        printf("FreeLibrary failed!\n");
        return false;
    }

    return true;
}

ISystemPhysics* SystemFactory::GetPhysicsSystem(PhysicsSystem_e systemType)
{
    std::string systemtypeStr;
   
    switch (systemType)
    {
    case SystemFactory::PhysicsSystem_e::Custom:
        systemtypeStr = PHYSICS_CUSTOM_DLL;
        break;
    case SystemFactory::PhysicsSystem_e::Bullet:
        systemtypeStr = PHYSICS_BULLET_DLL;
        break;
    default:
        systemtypeStr = PHYSICS_CUSTOM_DLL;
        break;
    }

    wchar_t* buf = StringHelpers::GetWCharBuffer(systemtypeStr);
    physicsHInstance = ::LoadLibrary(buf);
    delete[] buf;

    if (physicsHInstance)
    {
        GetSystemPhysics getSystem = reinterpret_cast<GetSystemPhysics>(::GetProcAddress(physicsHInstance, "GetSystem"));
        if (getSystem)
        {
            systemPhysics = getSystem();
            printf("Using %s to perform physics.\n\n", systemtypeStr.c_str());
        }
        else
        {
            printf("Unable to load %s's GetSystem!\n", systemtypeStr.c_str());
        }
    }
    else
    {
        printf("Unable to load %s!\n", systemtypeStr.c_str());
    }

	return systemPhysics;
}

bool SystemFactory::DisposePhysicsSystem()
{
    bool rc = DisposePointer(systemPhysics);
    if (!rc) return false;

    if (!FreeLibrary(physicsHInstance))
    {
        printf("FreeLibrary failed!\n");
        return false;
    }

    return true;
}

SystemVideo* SystemFactory::GetVideoSystem()
{
    systemVideo = (SystemVideo*)GameSystemVideo::GetInstance();

    return systemVideo;
}

bool SystemFactory::DisposeVideoSystem()
{
    bool rc = DisposePointer(systemVideo);
    
    return rc;
}

GameSystemScene* SystemFactory::GetSceneSystem()
{
    systemScene = new GameSystemScene();

    return systemScene;
}

bool SystemFactory::DisposeSceneSystem()
{
    bool rc = DisposePointer(systemScene);

    return rc;
}

ISystemSound* SystemFactory::GetSoundSystem(SoundSystem_e systemType)
{
    std::string systemtypeStr;

    switch (systemType)
    {
    case SystemFactory::SoundSystem_e::OpenAL:
        systemtypeStr = SOUND_OPENAL_DLL;
        break;  
    default:
        systemtypeStr = SOUND_OPENAL_DLL;
        break;
    }

    wchar_t* buf = StringHelpers::GetWCharBuffer(systemtypeStr);
    soundHInstance = ::LoadLibrary(buf);
    delete[] buf;

    if (soundHInstance)
    {
        GetSystemSound getSystem = reinterpret_cast<GetSystemSound>(::GetProcAddress(soundHInstance, "GetSystem"));
        if (getSystem)
        {
            systemSound = getSystem();
            printf("Using %s to perform sound.\n\n", systemtypeStr.c_str());
        }
        else
        {
            printf("Unable to load %s's GetSystem!\n", systemtypeStr.c_str());
        }
    }
    else
    {
        printf("Unable to load %s!\n", systemtypeStr.c_str());
    }

    return systemSound;
}

bool SystemFactory::DisposeSoundSystem()
{
    bool rc = DisposePointer(systemSound);
    if (!rc) return false;

    if (!FreeLibrary(soundHInstance))
    {
        printf("FreeLibrary failed!\n");
        return false;
    }

    return true;
}

ISystemGUI* SystemFactory::GetGUISystem()
{
    systemGUI = new GameSystemGUI();

    return systemGUI;
}

bool SystemFactory::DisposeGUISystem()
{
    bool rc = DisposePointer(systemGUI);

    return rc;
}

ISystemMesh* SystemFactory::GetMeshSystem(MeshSystem_e systemType)
{
    std::string systemtypeStr;

    switch (systemType)
    {
    case SystemFactory::MeshSystem_e::Assimp:
        systemtypeStr = MESH_ASSIMP_DLL;
        break;
    case SystemFactory::MeshSystem_e::PlyReader:
        systemtypeStr = MESH_PLYREADER_DLL;
        break;
    default:
        systemtypeStr = MESH_ASSIMP_DLL;
        break;
    }

    wchar_t* buf = StringHelpers::GetWCharBuffer(systemtypeStr);
    meshHInstance = ::LoadLibrary(buf);
    delete[] buf;

    if (meshHInstance)
    {
        GetSystemMesh getSystem = reinterpret_cast<GetSystemMesh>(::GetProcAddress(meshHInstance, "GetSystem"));
        if (getSystem)
        {
            systemMesh = getSystem();
            printf("Using %s to handle meshes.\n\n", systemtypeStr.c_str());
        }
        else
        {
            printf("Unable to load %s's GetSystem!\n", systemtypeStr.c_str());
        }
    }
    else
    {
        printf("Unable to load %s!\n", systemtypeStr.c_str());
    }

    return systemMesh;
}

bool SystemFactory::DisposeMeshSystem()
{
    DisposePointer(systemMesh);

    if (!FreeLibrary(meshHInstance))
    {
        printf("FreeLibrary failed!\n");
        return false;
    }

    return true;
}

ISystemUserControl* SystemFactory::GetUserControlSystem()
{
    systemUserControl = (ISystemUserControl*)GameSystemUserControl::GetInstance();

    return systemUserControl;
}

bool SystemFactory::DisposeUserControlSystem()
{
    return DisposePointer(systemUserControl);
}

ISystemDataPersistency* SystemFactory::GetDataPersistencySystem(PersistencySystem_e systemType)
{
    std::string systemtypeStr;

    switch (systemType)
    {
    case SystemFactory::PersistencySystem_e::XML:
        systemtypeStr = PERSISTENCY_XML_DLL;
        break;
    default:
        systemtypeStr = PERSISTENCY_XML_DLL;
        break;
    }

    wchar_t* buf = StringHelpers::GetWCharBuffer(systemtypeStr);
    persistencyHInstance = ::LoadLibrary(buf);
    delete[] buf;

    if (persistencyHInstance)
    {
        GetPersistencySystem getSystem = reinterpret_cast<GetPersistencySystem>(::GetProcAddress(persistencyHInstance, "GetSystem"));
        if (getSystem)
        {
            systemDataPersistency = getSystem();
            printf("Using %s to perform data persistency.\n\n", systemtypeStr.c_str());
        }
        else
        {
            printf("Unable to load %s's data persistency!\n", systemtypeStr.c_str());
        }
    }
    else
    {
        printf("Unable to load %s!\n", systemtypeStr.c_str());
    }

    return systemDataPersistency;
}

const bool SystemFactory::DisposePersistencySystem()
{
    DisposePointer(systemDataPersistency);

    if (!FreeLibrary(persistencyHInstance))
    {
        printf("FreeLibrary failed!\n");
        return false;
    }

	return true;
}

ISystemScripting* SystemFactory::GetLuaScriptingSystem()
{
    wchar_t* buf = StringHelpers::GetWCharBuffer(SCRIPTING_LUA_DLL);
    luaScriptingHInstance = ::LoadLibrary(buf);
    delete[] buf;

    if (luaScriptingHInstance)
    {
        GetScriptingSystemLua getSystem = reinterpret_cast<GetScriptingSystemLua>(::GetProcAddress(luaScriptingHInstance, "GetSystem"));
        if (getSystem)
        {
            systemScriptingLua = getSystem();
            printf("Using %s to perform sound.\n\n", SCRIPTING_LUA_DLL);
        }
        else
        {
            printf("Unable to load %s's GetSystem!\n", SCRIPTING_LUA_DLL);
        }
    }
    else
    {
        printf("Unable to load %s!\n", SCRIPTING_LUA_DLL);
    }

    return systemScriptingLua;
}

const bool SystemFactory::DisposeLuaScriptingSystem()
{
    DisposePointer(systemScriptingLua);

    if (!FreeLibrary(luaScriptingHInstance))
    {
        printf("FreeLibrary failed!\n");
        return false;
    }

    return true;
}

void SystemFactory::Dispose()
{
    DisposePhysicsSystem();
    DisposeVideoSystem();
    DisposeSceneSystem();
    DisposeSoundSystem();
    DisposeGUISystem();
    DisposeUserControlSystem();
    DisposeMeshSystem();
    DisposeAISystem();
    DisposePersistencySystem();
    DisposeLuaScriptingSystem();
}

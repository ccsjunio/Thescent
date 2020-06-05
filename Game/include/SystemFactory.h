#pragma once
#include <IFactory.h>
#include <IDisposable.h>
#include <ISystem.h>
#include <ISystemPhysics.h>
#include <ISystemSound.h>
#include <ISystemMesh.h>
#include <ISystemAI.h>
#include <ISystemDataPersistency.h>
#include <ISystemScripting.h>
#include <GameSystemScene.h>
#include <GameSystemUserControl.h>
#include <GameSystemGUI.h>
#include <GameSystemVideo.h>
#include <windows.h>

class SystemFactory : public IFactory<ISystem>, public IDisposable
{
private:
	HINSTANCE physicsHInstance = NULL;
	typedef ISystemPhysics* (__cdecl* GetSystemPhysics)();
	
	HINSTANCE soundHInstance = NULL;
	typedef ISystemSound* (__cdecl* GetSystemSound)();
	
	HINSTANCE meshHInstance = NULL;
	typedef ISystemMesh* (__cdecl* GetSystemMesh)();
	
	HINSTANCE aiHInstance = NULL;
	typedef ISystemAI* (__cdecl* GetSystemAI)();

	HINSTANCE persistencyHInstance = NULL;
	typedef ISystemDataPersistency* (__cdecl* GetPersistencySystem)();

	HINSTANCE luaScriptingHInstance = NULL;
	typedef ISystemScripting* (__cdecl* GetScriptingSystemLua)();
	
public:
	enum class MeshSystem_e
	{
		Assimp,
		PlyReader
	};
	enum class PhysicsSystem_e
	{
		Custom,
		Bullet
	};
	enum class AISystem_e
	{
		Custom
	};
	enum class SoundSystem_e
	{
		OpenAL
	};
	enum class PersistencySystem_e
	{
		XML
	};

	ISystemPhysics* systemPhysics;
	ISystemMesh* systemMesh;
	ISystemGUI* systemGUI;
	ISystemUserControl* systemUserControl;
	ISystemSound* systemSound;
	SystemVideo* systemVideo;
	GameSystemScene* systemScene;
	ISystemAI* systemAI;
	ISystemDataPersistency* systemDataPersistency;
	ISystemScripting* systemScriptingLua;
	
	SystemFactory();
	~SystemFactory();
	
	ISystem* Create(ParameterContainer& parameters);
	const bool Initialize(ParameterContainer& input, ParameterContainer& output);

	ISystemAI* GetAISystem(AISystem_e systemtype = AISystem_e::Custom);
	const bool DisposeAISystem();

	ISystemPhysics* GetPhysicsSystem(PhysicsSystem_e systemtype = PhysicsSystem_e::Custom);
	bool DisposePhysicsSystem();

	SystemVideo* GetVideoSystem();
	bool DisposeVideoSystem();

	GameSystemScene* GetSceneSystem();
	bool DisposeSceneSystem();

	ISystemSound* GetSoundSystem(SoundSystem_e systemType = SoundSystem_e::OpenAL);
	bool DisposeSoundSystem();

	ISystemGUI* GetGUISystem();
	bool DisposeGUISystem();

	ISystemMesh* GetMeshSystem(MeshSystem_e systemType = MeshSystem_e::Assimp);
	bool DisposeMeshSystem();

	ISystemUserControl* GetUserControlSystem();
	bool DisposeUserControlSystem();

	ISystemDataPersistency* GetDataPersistencySystem(PersistencySystem_e systemType = PersistencySystem_e::XML);	
	const bool DisposePersistencySystem();

	ISystemScripting* GetLuaScriptingSystem();
	const bool DisposeLuaScriptingSystem();

	void Dispose();
};


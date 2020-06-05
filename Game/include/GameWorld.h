#pragma once
#include "ICommunicable.h"
#include "ISingleton.h"
#include <atomic>
#include <GameSystemVideo.h>
#include <GameConfig.h>
#include <ISystemSound.h>
#include <ISystemMesh.h>
#include <ISystemPhysics.h>
#include <GameSystemScene.h>
#include <ISystemUserControl.h>
#include <ISystemGUI.h>
#include <IWorld.h>
#include <IDisposable.h>
#include <Level.h>
#include <ISystemDataPersistency.h>
#include "StateSaver.h"
#include "SystemFactory.h"
#include "GameVertexAnimationSystem.h"
#include "Player.h"
#include "IEnemy.h"
#include "Reactor.h"
#include "Hostage.h"
#include <IShaderContainer.h>
#include <IFileReader.h>

class GameWorld : public ISingleton, public IWorld, public IDisposable
{
private:
	GLFWwindow* window;
	std::vector<Level*> levels;
	ISystemMesh* systemMesh;
	ISystemGUI* systemGUI;
	ISystemUserControl* systemUserControl;
	ISystemSound* systemSound;
	GameSystemVideo* systemVideo;
	GameSystemScene* systemScene;
	ISystemAI* systemAI;
	StateSaver* stateSaver;
	ISystemPhysics* systemPhysics;
	ISystemDataPersistency* systemDataPersistency;
	ISystemScripting* systemScriptingLua;
	SystemFactory* systemFactory;
	GameConfig* gameConfig;
	IFactory<Level>* levelFactory;
	IFileReader* fileReader;
	std::atomic<bool> canUpdate, isUpdateComplete;
	std::vector<std::string> levelPaths;
	Level* currentLevel;
	Player* player;
	Reactor* reactor;
	Hostage* hostage;
	Scene* theMainScene;
	std::vector<Entity*> disposedEntities;
	std::vector<IEnemy*> enemies;
	std::atomic<bool> isRoom01Available, isRoom02Available, isShuttingDown, isPaused, isReadyToRender, disableGameplay;
	
	void OnWindowResized(const unsigned int& width, const unsigned int& height);
	void OnExternalMessage(const unsigned int& flags, const std::string& text, const double& duration, const unsigned int& renderAs = 0u);
	void OnPhysicsCollisionCallback(const ISystemPhysics::Collision_s& info);
	void OnFileLoadCallback(const ISystemDataPersistency::OnFileLoad_s& info);
	void OnGUIEvent(const ISystemGUI::GUIEvent_s& info);
	void OnKeyboardKeyPressCallback(const ISystemUserControl::KeyPress_s& info);
	void OnMouseKeyPressCallback(const ISystemUserControl::MouseKey_s& info);
	void OnMouseMoveCallback(const ISystemUserControl::MouseMove_s& info);

	static GameWorld* world;

	GameWorld();
	~GameWorld();

	void CreateCharacters();
	void ToggleNightVisionHint(const bool& value);
	IEnemy* FindEnemyByNameMatch(const std::string& name);	
	void WaitForIUpdatableLastUpdate(IUpdatable* system, const double& timeout = 0.0, const bool& showMsgs = false);
	const bool InitializeLuaScriptingSystem();
	void InitializeLevelFactory();
	void InitializeLevel(const std::string& levelName);
	void InitializeVideoSystem();
	void Dispose();
	const bool MoveToNextLevel();
	const bool MoveToPreviousLevel();
	Level* FindLevelByID(unsigned int id);
	void LoadFile(const std::string& command, const std::string& filePath);
	void OnLevelLoaded();

	ParameterContainer fileLoadBuffer;
	std::atomic<bool> killDataPersistencyThread;
	void DataPersistencyThreadFunc();
	std::atomic<bool> killPhysicsThread;
	void PhysicsThreadFunc();
	std::atomic<bool> killAIThread;
	void AIThreadFunc();
	std::atomic<bool> killSoundThread;
	void SoundThreadFunc();
	std::atomic<bool> killLuaThread;
	void LuaThreadFunc();
public:
	GameConfig* GetGameConfiguration();

	const bool IsShuttingDown();
	const bool IsUpdateComplete();
	Level* GetCurrentLevel();
	const bool CanUpdate();
	void SetWindow(GLFWwindow* window);
	void Initialize();
	void ChangeLevel();
	void ChangeLevel(const std::string& levelName);
	void ChangeLevel(unsigned int id);
	void Update(const FrameTime& deltaTime);
	ISystem* GetSystem(const std::string& systemName);
	void* GetPointer(const std::string& name);

	static void* GetInstance();
	static void* GetInstance(unsigned int numberOfArguments, ...);
	static void StaticDispose();
};
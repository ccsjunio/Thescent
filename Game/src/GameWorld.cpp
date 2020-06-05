#include "GameWorld.h"
#include <EngineFolders.h>
#include <algorithm>
#include <exception>
#include <IRigidbody.h>
#include <iostream>
#include <IRender.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <StringHelpers.h>
#include <GameCollisionResponse.h>
#include "Enemy.h"
#include "Boss.h"
#include "../Shaders/include/ShaderContainerFactory.h"
#include <GameWindowFlags.h>
#include <ThreadID.h>
#include "PhysicsFlags.h"

#define NS(s) ((s) * (1000000000.0))
#define MS(s) ((s) * (1000.0))
constexpr auto FIXED_DT = 1.0 / 60.0;

/*Since there can only be one gameworld, the world is a singleton.*/
GameWorld* GameWorld::world = nullptr;	//SINGLETON POINTER

/*Checks if the world is shutting down.*/
const bool GameWorld::IsShuttingDown()
{
	return this->isShuttingDown.load();
}

/*Checks if the world has finished it's last update.*/
const bool GameWorld::IsUpdateComplete()
{
	return this->isUpdateComplete;
}

/*Returns the active level in the game.*/
Level* GameWorld::GetCurrentLevel()
{
	return this->currentLevel;
}

/*Checks if the world can be updated.*/
const bool GameWorld::CanUpdate()
{
	return canUpdate;
}

/*Sets the OpenGL window on the video system.*/
void GameWorld::SetWindow(GLFWwindow* window)
{
	this->window = window;

	if (systemVideo)
	{
		systemVideo->SetWindow(window);
		systemVideo->Initialize();
	}
}

/*Disposes the world.*/
void GameWorld::Dispose()
{
	if (this)
	{		
		/*Disable main thread update loop*/
		canUpdate = false;

		/*Take down data persistency thread*/
		killDataPersistencyThread.store(true);
		systemDataPersistency->Activate(false);

		/*Take down ai thread.*/
		killSoundThread.store(true);
		systemSound->Activate(false);
		printf("Waiting for sound system to complete last update...\n");
		WaitForIUpdatableLastUpdate(systemSound, 5000.0, true);

		/*Take down lua thread.*/
		killLuaThread.store(true);
		systemScriptingLua->Activate(false);
		printf("Waiting for Lua scripting system to complete last update...\n");
		WaitForIUpdatableLastUpdate(systemScriptingLua, 5000.0, true);

		/*Take down physics thread.*/
		killPhysicsThread.store(true);
		systemPhysics->Activate(false);
		printf("Waiting for physics system to complete last update...\n");
		WaitForIUpdatableLastUpdate(systemPhysics, 5000.0, true);
					
		/*Take down ai thread.*/
		killAIThread.store(true);
		systemAI->Activate(false);
		printf("Waiting for ai system to complete last update...\n");
		WaitForIUpdatableLastUpdate(systemAI, 5000.0, true);

		DisposePointer(hostage);
		DisposePointer(player);
		DisposeVector(enemies);
		DisposePointer(reactor);

		systemVideo->ClearFrameBuffers();
		DisposePointer(levelFactory);
		
		DisposePointer(this->fileReader);

		DisposePointer(stateSaver);

		//Clear levels
		DisposeVector(levels);
		
		DisposePointer(gameConfig);

		DisposePointer(systemFactory);
	}
}

const bool GameWorld::MoveToNextLevel()
{
	bool rc = false;

	if (levelPaths.size() != 0)
	{
		canUpdate = false;

		std::rotate(levelPaths.begin(), levelPaths.begin() + 1, levelPaths.end());

		if (currentLevel)
		{
			delete (currentLevel);
		}

		ParameterContainer parameters;
		parameters.Add(Parameter("string", "filepath", levelPaths[0], false));
		currentLevel = levelFactory->Create(parameters);

		if (currentLevel)
		{
			InitializeVideoSystem();
			canUpdate = true;
			rc = true;
		}
	}

	return rc;
}

const bool GameWorld::MoveToPreviousLevel()
{
	bool rc = false;

	if (levelPaths.size() != 0)
	{
		canUpdate = false;

		std::rotate(levelPaths.begin(), levelPaths.end() - 1, levelPaths.end());

		if (currentLevel)
		{
			delete (currentLevel);
		}

		ParameterContainer parameters;
		parameters.Add(Parameter("string", "filepath", levelPaths[0], false));
		currentLevel = levelFactory->Create(parameters);

		if (currentLevel)
		{
			InitializeVideoSystem();
			canUpdate = true;
			rc = true;
		}
	}

	return rc;
}

Level* GameWorld::FindLevelByID(unsigned int id)
{
	Level* ptr = nullptr;
	if (levels.size() > 0)
	{
		for (unsigned int i = 0; i < levels.size(); i++)
		{
			Level* _ptr = levels[i];
			if (_ptr->id == id)
			{
				ptr = _ptr;
				break;
			}
		}
	}

	return ptr;
}

void GameWorld::LoadFile(const std::string& command, const std::string& filePath)
{
	this->fileLoadBuffer.Add("string", command, filePath);
}

void GameWorld::OnLevelLoaded()
{
	/*Scene1 is the main scene in the game.*/
	theMainScene = currentLevel->FindSceneByName("Scene1");
	if (theMainScene)
	{
		systemScene->InitializeScene01(theMainScene);

		/*Initialize player for Scene1*/
		player->SetCamera(theMainScene->GetCamera());
		Scene* deposit_scene = systemScene->GetDepositScene();
		if (deposit_scene)
		{
			Entity* proj = deposit_scene->FindEntityByName("PROJECTILE_BLASTER");
			player->SetupWeapon(Player::Weapon_s::Type_e::Blaster, proj, Vector3D(-1.0f, 0.0f, -1.0f), 20.0f);
		}

		/*Initialize reactor*/
		auto reactorEntity = theMainScene->FindEntityByName("REACTOR");
		if (reactorEntity)
		{
			reactor->SetEntity(reactorEntity);
		}

		/*Initialize hostage*/
		auto hostageEntity = theMainScene->FindEntityByName("HOSTAGE");
		if (hostageEntity)
		{
			hostage->SetEntity(hostageEntity);
		}

		/*Initialize enemies*/
		auto enemyEntity = theMainScene->FindEntityByName("ENEMY_TURRET_TOP1");
		if (enemyEntity)
		{
			IEnemy* enemy = FindEnemyByNameMatch(enemyEntity->GetName());
			if (enemy) ((Enemy*)enemy)->SetEntity(enemyEntity);
		}
		enemyEntity = theMainScene->FindEntityByName("ENEMY_DRONE0_UNIT1");
		if (enemyEntity)
		{
			IEnemy* enemy = FindEnemyByNameMatch(enemyEntity->GetName());
			if (enemy) ((Enemy*)enemy)->SetEntity(enemyEntity);
		}
		enemyEntity = theMainScene->FindEntityByName("ENEMY_DRONE1_UNIT1");
		if (enemyEntity)
		{
			IEnemy* enemy = FindEnemyByNameMatch(enemyEntity->GetName());
			if (enemy) ((Enemy*)enemy)->SetEntity(enemyEntity);
		}

		IEnemy* enemy = FindEnemyByNameMatch("ENEMY_BOSS");
		if (enemy)
		{
			auto body = systemScene->GetDepositScene()->FindEntityByName("ENEMY_BOSS_BODY");
			auto top = systemScene->GetDepositScene()->FindEntityByName("ENEMY_BOSS_TURRET_TOP");
			auto bottom = systemScene->GetDepositScene()->FindEntityByName("ENEMY_BOSS_TURRET_BOTTOM");
			((Boss*)enemy)->SetEntity(body, top, bottom);
		}
	}

	/*Fires Sound thread*/
	std::thread soundthread(&GameWorld::SoundThreadFunc, world);
	soundthread.detach();

	/*Fires physics thread*/
	std::thread physicsThread(&GameWorld::PhysicsThreadFunc, world);
	physicsThread.detach();

	/*Fires AI thread*/
	std::thread aiThread(&GameWorld::AIThreadFunc, world);
	aiThread.detach();

	/*Fires Lua thread*/
	std::thread luaThread(&GameWorld::LuaThreadFunc, world);
	luaThread.detach();

	//Play game soundtrack
	auto scene_entity = theMainScene->FindEntityByName("SCENE_ENTITY");
	if (scene_entity)
	{
		auto sc = scene_entity->GetComponentByName("ThemeSong");
		if (sc) sc->Activate(true);
	}
}

/*Encapsulates the data persistency thread.*/
void GameWorld::DataPersistencyThreadFunc()
{		
	std::vector<Parameter> params;
	while (!killDataPersistencyThread)
	{
		/*Checks the file buffer for a known load command, loads the file and clears the buffer.*/
		if (this->fileLoadBuffer.GetParameters(params))
		{
			for (Parameter& p : params)
			{
				if (p.name == "load_level")
				{
					this->systemVideo->Activate(false);

					std::string levelPath = p.value;

					ParameterContainer level_parameters;
					level_parameters.Add("string", "filepath", levelPath);
					if (!this->levelFactory->Create(level_parameters))
					{
						printf("Thread could not read level file.\n");
					}
				}
			}

			this->fileLoadBuffer.Clear();
		}
	}
}

/*Encapsulates the physics thread.*/
void GameWorld::PhysicsThreadFunc()
{
	FrameTime threadTime;

	double ft = 0.0;
	double currentTime = 0.0;
	while (!killPhysicsThread.load())
	{	
		currentTime = float(clock()) / CLOCKS_PER_SEC;

		/*If true, zeroes the time, simulating a frozen state!*/
		if (this->isPaused)
		{
			threadTime.SetDeltaTime(0.0);
			threadTime.SetRunTime(0.0);
		}
		else
		{
			//Gets the delta time	
			ft = currentTime - threadTime.GetRunTime();
			threadTime.SetDeltaTime(std::max(ft, FIXED_DT));
			threadTime.SetRunTime(currentTime);
		}

		try 
		{
			if (currentLevel)
			{
				for (auto& scene : *currentLevel->GetScenes())
				{
					systemPhysics->SetScene(scene);
					systemPhysics->Update(threadTime);
				}
			}
		}
		catch (...)
		{
			//Something went wrong -- LOG
		}

		/*Consume unused time when the frame time was faster than expected*/
		if (ft < FIXED_DT)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds((long long)NS(FIXED_DT - ft)));
		}
	}
}

/*Encapsulates the AI thread.*/
void GameWorld::AIThreadFunc()
{
	FrameTime threadTime;

	double ft = 0.0;
	double currentTime = 0.0;
	while (!killAIThread.load())
	{
		currentTime = float(clock()) / CLOCKS_PER_SEC;

		/*If true, zeroes the time, simulating a frozen state!*/
		if (this->isPaused)
		{
			threadTime.SetDeltaTime(0.0);
			threadTime.SetRunTime(0.0);
		}
		else
		{
			//Gets the delta time	
			ft = currentTime - threadTime.GetRunTime();			
			threadTime.SetDeltaTime(std::max(ft, FIXED_DT));
			threadTime.SetRunTime(currentTime);
		}
				
		try
		{
			if (currentLevel)
			{
				for (auto& scene : *currentLevel->GetScenes())
				{
					systemAI->SetScene(scene);
					systemAI->Update(threadTime);
				}
			}
		}
		catch (...)
		{
			//Something went wrong -- LOG
		}

		/*Consume unused time when the frame time was faster than expected*/
		if (ft < FIXED_DT)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds((long long)NS(FIXED_DT - ft)));
		}
	}
}

/*Encapsulates the sound thread.*/
void GameWorld::SoundThreadFunc()
{
	FrameTime threadTime;

	double ft = 0.0;
	double currentTime = 0.0;
	while (!killSoundThread.load())
	{
		currentTime = float(clock()) / CLOCKS_PER_SEC;

		/*If true, zeroes the time, simulating a frozen state!*/
		if (this->isPaused)
		{
			threadTime.SetDeltaTime(0.0);
			threadTime.SetRunTime(0.0);
		}
		else
		{
			//Gets the delta time	
			ft = currentTime - threadTime.GetRunTime();
			threadTime.SetDeltaTime(std::max(ft, FIXED_DT));
			threadTime.SetRunTime(currentTime);
		}
				
		try
		{
			if (currentLevel)
			{
				for (auto& scene : *currentLevel->GetScenes())
				{
					systemSound->SetScene(scene);
					systemSound->Update(threadTime);
				}
			}
		}
		catch (...)
		{
			//Something went wrong -- LOG
		}

		/*Consume unused time when the frame time was faster than expected*/
		if (ft < FIXED_DT)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds((long long)NS(FIXED_DT - ft)));
		}
	}
}

/*Encapsulates the Lua scripting system thread.*/
void GameWorld::LuaThreadFunc()
{
	FrameTime threadTime;

	double ft = 0.0;
	double currentTime = 0.0;
	while (!killSoundThread.load())
	{
		currentTime = float(clock()) / CLOCKS_PER_SEC;

		/*If true, zeroes the time, simulating a frozen state!*/
		if (this->isPaused)
		{
			threadTime.SetDeltaTime(0.0);
			threadTime.SetRunTime(0.0);
		}
		else
		{
			//Gets the delta time	
			ft = currentTime - threadTime.GetRunTime();
			threadTime.SetDeltaTime(std::max(ft, FIXED_DT));
			threadTime.SetRunTime(currentTime);
		}

		try
		{
			if (currentLevel)
			{
				for (auto& scene : *currentLevel->GetScenes())
				{
					systemScriptingLua->SetScene(scene);
					systemScriptingLua->Update(threadTime);
				}
			}
		}
		catch (...)
		{
			//Something went wrong -- LOG
		}
		
		/*Consume unused time when the frame time was faster than expected*/
		if (ft < FIXED_DT)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds((long long)NS(FIXED_DT - ft)));
		}
	}
}

/*Singleton initializer for the world*/
void* GameWorld::GetInstance()
{
	if (!world)
	{
		world = new GameWorld();
	}

	return world;
}

/*Singleton initializer for the world*/
void* GameWorld::GetInstance(unsigned int numberOfArguments, ...)
{
	va_list arguments;

	va_start(arguments, numberOfArguments);

	std::vector<void*> parameters;
	parameters.resize(numberOfArguments);

	for (unsigned int i = 0; i < numberOfArguments; i++)
	{
#pragma warning( push )
#pragma warning( disable : 6285 )
		auto ptr = va_arg(arguments, void*);
#pragma warning( pop )
		parameters[i] = ptr;
	}
	va_end(arguments);

	if (!world)
	{
		world = new GameWorld();
	}

	return world;
}

/*Disposes the world.*/
void GameWorld::StaticDispose()
{
	if (world)
	{	
		delete (world);
		world = nullptr;
	}
}

void GameWorld::ChangeLevel()
{
	/*bool ok = false;
	canUpdate = false;

	Level* level = GetNextLevel();
	if (level)
	{
		currentLevel = level;
		ok = true;
	}
	else 
	{
		level = GetPreviousLevel();

		if (level)
		{
			currentLevel = level;
			ok = true;
		}
	}

	if (ok)
	{
		systemAnimation->SetLevel(currentLevel);
		InitializeVideoSystem();
	}	

	canUpdate = true;*/
}

void GameWorld::ChangeLevel(const std::string& levelName)
{
	//InitializeLevel(levelName);
	////InitializeParticleSystem();
	//InitializeVideoSystem();
	//InitializeAnimationSystem();
	////LoadAnimations();

	Level* ptr = nullptr;
	if (levels.size() > 0)
	{
		for (unsigned int i = 0; i < levels.size(); i++)
		{
			Level* _ptr = levels[i];
			if (_ptr->GetName() == levelName)
			{
				ptr = _ptr;
				break;
			}
		}
	}

	if (ptr)
	{
		currentLevel = ptr;
		InitializeVideoSystem();
	}
	else
	{
		printf("Could not fin level %s", levelName.c_str());
	}
}

void GameWorld::ChangeLevel(unsigned int id)
{
	Level* ptr = FindLevelByID(id);
	if (ptr)
	{
		currentLevel = ptr;
		InitializeVideoSystem();
	}
	else
	{
		printf("Could not fin level ID=%d", id);
	}
}

/*Window resize event from the VideoSystem.*/
void GameWorld::OnWindowResized(const unsigned int& width, const unsigned int& height)
{
	if (this->systemGUI)
	{
		ISystemGUI::ScreenProperties_s properties;
		properties.screenWidth = width;
		properties.screenHeight = height;
		this->systemGUI->SetScreenProperties(properties);
	}
}

/*Sends messages to the GUI.*/
void GameWorld::OnExternalMessage(const unsigned int& flags, const std::string& text, const double& duration, const unsigned int& renderAs)
{
	if (systemGUI)
	{
		systemGUI->PushMessage(flags, text, duration, renderAs);
	}
}

/*Filters collision on the Physics System.*/
void GameWorld::OnPhysicsCollisionCallback(const ISystemPhysics::Collision_s& info)
{
	if (info.a && info.b)
	{		
		std::string aName = info.a->GetName();
		std::string bName = info.b->GetName();

		/*Collision between two projectiles*/
		if (aName.find("CLONED_PROJECTILE") != std::string::npos && bName.find("CLONED_PROJECTILE") != std::string::npos)
		{
			//Destroy projectiles at the end of the main thread update
			Entity *pA = info.a, *pB = info.b;
			GameCollisionResponse::DisableEntity(pA);
			GameCollisionResponse::DisableEntity(pB);
		}
		else if (aName.find("CLONED_PROJECTILE") != std::string::npos)
		{
			if (bName == "PLAYER_COLLISIONSPHERE")
			{
				IEnemy* enemy = FindEnemyByNameMatch(aName);
				if (enemy)
				{
					GameCollisionResponse::OnPlayerHit(player, enemy);
				}
			}
			else if (bName.find("ENEMY") != std::string::npos)
			{
				IEnemy* enemy = FindEnemyByNameMatch(bName);
				if (enemy)
				{
					GameCollisionResponse::OnEnemyHit(aName, player, enemy, reactor, systemScriptingLua, theMainScene);
				}
			}
			else if (bName == "REACTOR")
			{
				Scene* depositScene = systemScene->GetDepositScene();
				GameCollisionResponse::OnReactorHit(player, reactor, systemScriptingLua, theMainScene,depositScene);
				printf("Reactor health = %.2f\n", reactor->GetHealth());
			}

			//Destroy projectile at the end of the main thread update
			Entity* pA = info.a;
			GameCollisionResponse::DisableEntity(pA);
			//SynchronizedDisposeEntity(THREAD_ID_PHYSICS, info.a);
		}
		else if (bName.find("CLONED_PROJECTILE") != std::string::npos)
		{
			if (aName == "PLAYER_COLLISIONSPHERE")
			{
				IEnemy* enemy = FindEnemyByNameMatch(bName);
				if (enemy)
				{
					GameCollisionResponse::OnPlayerHit(player, enemy);
				}
			}
			else if (aName.find("ENEMY") != std::string::npos)
			{
				IEnemy* enemy = FindEnemyByNameMatch(aName);
				if (enemy)
				{
					GameCollisionResponse::OnEnemyHit(bName, player, enemy, reactor, systemScriptingLua, theMainScene);
				}
			}
			else if (aName == "REACTOR")
			{
				Scene* depositScene = systemScene->GetDepositScene();
				GameCollisionResponse::OnReactorHit(player, reactor, systemScriptingLua, theMainScene, depositScene);
				printf("Reactor health = %.2f\n", reactor->GetHealth());
			}

			//Destroy projectile at the end of the frame
			Entity *pB = info.b;
			GameCollisionResponse::DisableEntity(pB);
			//SynchronizedDisposeEntity(THREAD_ID_PHYSICS, info.b);
		}

		//Player opened door 01
		else if ((aName == "DOOR_01" && bName == "PLAYER_COLLISIONSPHERE") || (aName == "PLAYER_COLLISIONSPHERE" && bName == "DOOR_01"))
		{
			if (!isRoom01Available.load())
			{
				printf("Player opened door 1! Area 1 is available!\n");

				/*Tells the world that the door has been opened*/
				isRoom01Available.store(GameCollisionResponse::OnDoor01Opened(systemScriptingLua, theMainScene));				
			}
		}
		//Player tries to open door 02
		else if ((aName == "DOOR_02" && bName == "PLAYER_COLLISIONSPHERE") || (aName == "PLAYER_COLLISIONSPHERE" && bName == "DOOR_02"))
		{
			if (!isRoom02Available.load())
			{
				printf("Access denied! To open door 02, find the button.\n");

				GameCollisionResponse::OnDoor02AccessDenied(theMainScene);
			}
		}
		//Player is pressing button
		else if ((aName == "KEY" && bName == "PLAYER_COLLISIONSPHERE") || (aName == "PLAYER_COLLISIONSPHERE" && bName == "KEY"))
		{
			if (!isRoom02Available.load())
			{
				printf("Player pressed button! Area 2 is available!\n");

				/*Tells the world that the door has been opened*/
				isRoom02Available.store(GameCollisionResponse::OnDoor02Opened(systemScriptingLua, theMainScene));
			}
		}
		//Player is pressing button
		else if ((aName == "ANTICLOAK_PLANE" && bName == "PLAYER_COLLISIONSPHERE") || (aName == "PLAYER_COLLISIONSPHERE" && bName == "ANTICLOAK_PLANE"))
		{
			if (!player->GetItemState(1))
			{
				printf("Player acquired anticloak vision!\n");

				player->SetItemState(1, true);
				auto sceneEntity = theMainScene->FindEntityByName("SCENE_ENTITY");
				if (sceneEntity)
				{
					IComponent* sc = sceneEntity->GetComponentByName("Anticloak");
					if (sc) sc->Activate(sc);
				}
				
				OnExternalMessage(GUIMSG_ITEMACQUIRED, "1", 0.0);

				Entity* pA = info.a, * pB = info.b;
				if (aName == "ANTICLOAK_PLANE") GameCollisionResponse::DisableEntity(pA);// SynchronizedDisposeEntity(THREAD_ID_PHYSICS, info.a);
				if (bName == "ANTICLOAK_PLANE") GameCollisionResponse::DisableEntity(pB);// SynchronizedDisposeEntity(THREAD_ID_PHYSICS, info.b);
			}
		}
		//Player is saving hostage
		else if ((aName == "CAGE" && bName == "PLAYER_COLLISIONSPHERE") || (aName == "PLAYER_COLLISIONSPHERE" && bName == "CAGE"))
		{
			if (!hostage->WasRescued())
			{				
				if (GameCollisionResponse::OnHostageRescued(hostage))
				{
					printf("The Hostage was rescued!\n");

					/*Once the hostage is rescued, adds the boss into the scene*/
					Scene* deposit_scene = systemScene->GetDepositScene();
					if (!GameCollisionResponse::OnBossAppearing(deposit_scene, theMainScene))
					{
						printf("Could not add boss entities!\n");
					}

					//Destroy cage and hostage
					Entity* pA = hostage->GetEntity(), * pB = theMainScene->FindEntityByName("CAGE");
					GameCollisionResponse::DisableEntity(pA);
					GameCollisionResponse::DisableEntity(pB);
					/*SynchronizedDisposeEntity(THREAD_ID_PHYSICS, hostage->GetEntity());
					SynchronizedDisposeEntity(THREAD_ID_PHYSICS, theMainScene->FindEntityByName("CAGE"));*/
				}
			}
		}
		//Player acquired What-The-What Cannon
		else if ((aName == "PROJECTILE_WTW" && bName == "PLAYER_COLLISIONSPHERE") || (aName == "PLAYER_COLLISIONSPHERE" && bName == "PROJECTILE_WTW"))
		{
			auto deposit_scene = systemScene->GetDepositScene();
			if (GameCollisionResponse::OnWTWCannonEnable(player, theMainScene, deposit_scene))
			{
				//Toggle cannon
				ISystemUserControl::KeyPress_s kp;
				kp.action = "selected_weapon_2";
				OnKeyboardKeyPressCallback(kp);
			}
		}
		//Player acquired What-The-What Cannon
		else if ((aName == "REACTOR" && bName == "PLAYER_COLLISIONSPHERE") || (aName == "PLAYER_COLLISIONSPHERE" && bName == "REACTOR"))
		{
			Scene* depositScene = systemScene->GetDepositScene();
			GameCollisionResponse::OnPlayerReactorContact(player, reactor, systemScriptingLua, theMainScene, depositScene);
		}
		//Player is saving hostage
		else if ((aName == "EXIT_CUBE" && bName == "PLAYER_COLLISIONSPHERE") || (aName == "PLAYER_COLLISIONSPHERE" && bName == "EXIT_CUBE"))
		{
			auto deposit_scene = systemScene->GetDepositScene();
			auto cockpitScene = currentLevel->FindSceneByName("Scene0");
			if (GameCollisionResponse::OnPlayerEscaping(systemScriptingLua, theMainScene, cockpitScene, deposit_scene))
			{
				//Disable GUI stuff
				unsigned long long flags = 0ull;
				flags |= GUIMSG_HIDESHIPPANEL;
				OnExternalMessage(flags, "1", 0);
								
				//Toggle off anticloak
				{
					if (this->player->GetItemState(1))
					{
						systemVideo->SetFBOPassState("MainSceneFramebuffer", 4, false);
						player->SetAnticloakState(false);
					}
				}
				
				//Disable gameplay
				this->disableGameplay.store(true);
				//Congratulations, you finished Thescent
			}
		}
	}
}

/*Callback fired when a file is read. Used to make sure the level file was loaded.*/
void GameWorld::OnFileLoadCallback(const ISystemDataPersistency::OnFileLoad_s& info)
{
	if (info.eventDescription == "level")
	{
		Level* level = (Level*)info.fileObject;
		levels.push_back(level);
		currentLevel = level;
	}
}

/*Callback fired by the GUI System.*/
void GameWorld::OnGUIEvent(const ISystemGUI::GUIEvent_s& info)
{
	if (info.eventName == "toggle_input_capture")
	{
		this->isPaused.store(false);
		systemUserControl->ToggleInputCapture(true);
		systemGUI->HandleInput(false);
		OnExternalMessage(GUIMSG_TOGGLEMENU, "0", 0.0);
	}
	else if(info.eventName == "disable_night_vision_hint")
	{
		ToggleNightVisionHint(false);
	}
	else if (info.eventName == "exit_game")
	{
		this->isShuttingDown.store(true);
	}
	else if (info.eventName == "save_state")
	{
		std::string filePath = this->stateSaver->SaveState(currentLevel);
		printf("Current state exported to %s!\n", filePath.c_str());
	}
	else if (info.eventName == "toggle_colliders_on")
	{
		this->systemVideo->ToggleDebugRendering(true);
		printf("%s collider rendering!\n", "Enabled");
	}
	else if (info.eventName == "toggle_colliders_off")
	{
		this->systemVideo->ToggleDebugRendering(false);
		printf("%s collider rendering!\n", "Disabled");
	}
	else if (info.eventName == "move_next_level")
	{
		this->MoveToNextLevel();
	}
	else if (info.eventName == "move_previous_level")
	{
		this->MoveToPreviousLevel();
	}
	else
	{

	}
}

/*UserControlSystem's key press event.*/
void GameWorld::OnKeyboardKeyPressCallback(const ISystemUserControl::KeyPress_s& info)
{
	if (info.action == "toggle_input_capture")
	{
		if (systemUserControl->IsCapturingInput())
		{
			this->isPaused.store(false);
			this->systemGUI->HandleInput(false);
			OnExternalMessage(GUIMSG_TOGGLEMENU, "0", 0.0);
		}
		else
		{
			this->isPaused.store(true);
			this->systemGUI->HandleInput(true);
			OnExternalMessage(GUIMSG_TOGGLEMENU, "1", 0.0);
		}
	}

	else if (info.action == "player_move_forward")
	{
		if (this->disableGameplay.load()) return;

		player->UpdatePosition(Player::PlayerMovement_e::Forward, gameConfig->controlOptions.movementStep);
	}
	else if (info.action == "player_move_backward")
	{
		if (this->disableGameplay.load()) return;

		player->UpdatePosition(Player::PlayerMovement_e::Backward, gameConfig->controlOptions.movementStep);
	}
	else if (info.action == "player_move_straferight")
	{
		if (this->disableGameplay.load()) return;

		player->UpdatePosition(Player::PlayerMovement_e::StrafeRight, gameConfig->controlOptions.movementStep);
	}
	else if (info.action == "player_move_strafeleft")
	{
		if (this->disableGameplay.load()) return;

		player->UpdatePosition(Player::PlayerMovement_e::StrafeLeft, gameConfig->controlOptions.movementStep);
	}
	else if (info.action == "player_move_halt")
	{
		if (this->disableGameplay.load()) return;

		player->UpdatePosition(Player::PlayerMovement_e::Halt, gameConfig->controlOptions.movementStep);
	}
	else if (info.action == "player_move_up")
	{
		if (this->disableGameplay.load()) return;

		player->UpdatePosition(Player::PlayerMovement_e::HooverUp, gameConfig->controlOptions.movementStep);
	}
	else if (info.action == "player_move_down")
	{
		if (this->disableGameplay.load()) return;

		player->UpdatePosition(Player::PlayerMovement_e::HooverDown, gameConfig->controlOptions.movementStep);
	}
	else if (info.action == "player_night_vision")
	{
		if (this->disableGameplay.load()) return;

		if (this->player->GetItemState(1))
		{
			bool currentPassState = false;
			if (systemVideo->GetFBOPassState("MainSceneFramebuffer", 4, currentPassState))
			{
				systemVideo->SetFBOPassState("MainSceneFramebuffer", 4, !currentPassState);
				player->SetAnticloakState(!currentPassState);

				/*Affects the boss*/
				Boss* boss = (Boss*)FindEnemyByNameMatch("ENEMY_BOSS");
				if (boss) boss->ToggleVisibility(player->GetAnticloakState());
			}
		}
	}
	else if (info.action == "selected_weapon_1")
	{
		if (this->disableGameplay.load()) return;

		/*Sends a message to the GUI to change the weapon texture*/
		unsigned int flags = 0u;
		flags |= GUIMSG_CHANGEWEAPON;
		OnExternalMessage(flags, "0", 0.0);
		/*Change plauer weapon*/
		player->SelectWeapon(0);
	}
	else if (info.action == "selected_weapon_2")
	{
		if (this->disableGameplay.load()) return;

		if (player->FindWeaponByProjectileName("PROJECTILE_WTW"))
		{
			/*Sends a message to the GUI to change the weapon texture*/
			unsigned int flags = 0u;
			flags |= GUIMSG_CHANGEWEAPON;
			OnExternalMessage(flags, "1", 0.0);
			/*Change plauer weapon*/
			player->SelectWeapon(1);
		}
	}
}

/*UserControlSystem's mouse click event.*/
void GameWorld::OnMouseKeyPressCallback(const ISystemUserControl::MouseKey_s& info)
{
	if (info.KeyID == ISystemUserControl::MouseKey_s::MouseKey_e::Left)
	{
		if (this->disableGameplay.load()) return;

		player->FireWeapon(enemies);
	}
}

/*UserControlSystem's mouse move event.*/
void GameWorld::OnMouseMoveCallback(const ISystemUserControl::MouseMove_s& info)
{
	if (player)
	{
		player->UpdateEye(info.xoffset, info.yoffset, (float)info.sensitivity);
	}
}

/*The game world constructor.*/
GameWorld::GameWorld()
{
	world = this;

	std::string gameConfigPath = EngineFolders::GetConfigPath() + "game.cfg";

	this->disableGameplay.store(false);
	this->isPaused.store(true);
	this->isShuttingDown.store(false);
	this->killPhysicsThread = false;
	this->canUpdate = false;
	this->currentLevel = nullptr;
	this->stateSaver = new StateSaver();
	this->gameConfig = new GameConfig(gameConfigPath);
	
	this->systemFactory = new SystemFactory();

	this->systemDataPersistency = this->systemFactory->GetDataPersistencySystem((SystemFactory::PersistencySystem_e)gameConfig->dataPersistencyOptions.engine);
	if (this->systemDataPersistency)
	{
		this->systemDataPersistency->BindOnFileLoadCallback(std::bind(&GameWorld::OnFileLoadCallback, this, std::placeholders::_1));
		this->systemDataPersistency->Activate(this->gameConfig->dataPersistencyOptions.isActive);

		ParameterContainer input, output;
		input.Add(Parameter("std::string", "what", "levelfactory", false));
		this->levelFactory = (IFactory<Level>*)this->systemDataPersistency->GetPointer(input, output);

		input.Clear(); output.Clear();

		input.Add("std::string", "what", "shaderfilereader", false);
		this->fileReader = (IFileReader*)this->systemDataPersistency->GetPointer(input, output);
	}

	this->systemPhysics = this->systemFactory->GetPhysicsSystem((SystemFactory::PhysicsSystem_e)gameConfig->physicsOptions.engine);
	if (this->systemPhysics)
	{
		this->systemPhysics->SetFlags(DISABLEDEFAULTRESPONSE_SphereToSphere | DISABLEDEFAULTRESPONSE_BoxToBox | DISABLEDEFAULTRESPONSE_SphereToCapsule | DISABLEDEFAULTRESPONSE_SphereToBox);
		this->systemPhysics->BindCollisionCallback(std::bind(&GameWorld::OnPhysicsCollisionCallback, this, std::placeholders::_1));
		this->systemPhysics->Activate(this->gameConfig->physicsOptions.isActive);
	}

	this->systemMesh = this->systemFactory->GetMeshSystem((SystemFactory::MeshSystem_e)gameConfig->meshOptions.engine);
	if (this->systemMesh)
	{
		this->systemMesh->Activate(this->gameConfig->meshOptions.isActive);
	}

	this->systemSound = this->systemFactory->GetSoundSystem((SystemFactory::SoundSystem_e)gameConfig->soundOptions.engine);
	if (this->systemSound)
	{
		this->systemSound->Activate(gameConfig->soundOptions.isActive);
	}

	this->systemAI = this->systemFactory->GetAISystem((SystemFactory::AISystem_e)gameConfig->aiOptions.engine);
	if (this->systemAI)
	{
		this->systemAI->Activate(gameConfig->aiOptions.isActive);
	}

	this->systemGUI = this->systemFactory->GetGUISystem();
	if (this->systemGUI)
	{
		this->systemGUI->BindGUICallback(std::bind(&GameWorld::OnGUIEvent, this, std::placeholders::_1));
		this->systemGUI->Activate(this->gameConfig->guiOptions.isActive);
	}

	this->systemUserControl = this->systemFactory->GetUserControlSystem();
	if (this->systemUserControl)
	{
		this->systemUserControl->BindMouseMoveCallback(std::bind(&GameWorld::OnMouseMoveCallback, this, std::placeholders::_1));
		this->systemUserControl->BindKeyboardPressCallback(std::bind(&GameWorld::OnKeyboardKeyPressCallback, this, std::placeholders::_1));
		this->systemUserControl->BindMouseClickCallback(std::bind(&GameWorld::OnMouseKeyPressCallback, this, std::placeholders::_1));
		this->systemUserControl->Activate(this->gameConfig->controlOptions.isActive);
		this->systemUserControl->ToggleInputCapture(false);
	}

	this->systemVideo = (GameSystemVideo*)this->systemFactory->GetVideoSystem();
	if (systemVideo)
	{
		ParameterContainer input, output;
		input.Add("string", "FileType", "ShaderContainer");
		input.Add("string", "filepath", EngineFolders::GetConfigPath() + "shader_programs.xml");
		if (this->fileReader->ReadToParameterContainer(input, output))
		{
			IFactory<IShaderContainer>* scFact = new ShaderContainerFactory();
			auto sc = scFact->Create(output);
			this->systemVideo->SetShaderContainer(sc);

			DisposePointer(scFact);
		}	
		this->systemVideo->BindOnWindowResizeEvent(std::bind(&GameWorld::OnWindowResized, this, std::placeholders::_1, std::placeholders::_2));
		this->systemVideo->Activate(this->gameConfig->videoOptions.isActive);
	}

	//systemAnimation = (GameSystemAnimationLua*)systemFactory->GetLuaAnimationSystem();
	this->systemScene = this->systemFactory->GetSceneSystem();
	if (systemScene)
	{
		ParameterContainer input;
		input.Add("IWorld", "world", this);
		input.Add("IAISystem", "aisystem", systemAI);
		this->systemScene->Initialize(input);
		//systemScene->BindOnExternalMessage(OnExternalMessage);
	}
	
	this->systemScriptingLua = systemFactory->GetLuaScriptingSystem();
	if (this->systemScriptingLua)
	{
		InitializeLuaScriptingSystem();
		this->systemScriptingLua->Activate(this->gameConfig->luaScriptingOptions.isActive);
	}

	CreateCharacters();
}

/*The game world destructor.*/
GameWorld::~GameWorld()
{
	Dispose();
}

void GameWorld::CreateCharacters()
{
	this->hostage = new Hostage();
	this->reactor = new Reactor(100.0f);
	this->player = new Player();
	this->player->BindOnExternalMessage(std::bind(&GameWorld::OnExternalMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	this->player->SetHealth(100.0f);
	this->player->addDebugLine = std::bind(&GameSystemVideo::AddDebugLine, systemVideo, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	this->player->drawDebugObjects = std::bind(&GameSystemVideo::DrawDebugObjects, systemVideo);
	this->enemies.push_back(new Enemy("ENEMY_TURRET_TOP1", 1.0f, 100.0f));
	this->enemies.push_back(new Enemy("ENEMY_DRONE0_UNIT1", 2.0f, 90.0f));
	this->enemies.push_back(new Enemy("ENEMY_DRONE1_UNIT1", 3.0f, 80.0f));
	this->enemies.push_back(new Boss("ENEMY_BOSS", 10.0f, 2000.0f));
}

void GameWorld::ToggleNightVisionHint(const bool& value)
{
	if (value)
	{
		this->isPaused.store(true);
		systemUserControl->ToggleInputCapture(false);
		systemGUI->HandleInput(true);
		OnExternalMessage(GUIMSG_TOGGLENIGHTVISIONHINT, "1", 0.0);
	}
	else
	{
		this->isPaused.store(false);
		systemUserControl->ToggleInputCapture(true);
		systemGUI->HandleInput(false);
		OnExternalMessage(GUIMSG_TOGGLENIGHTVISIONHINT, "0", 0.0);
	}
}

/*Finds an enemy from the vector of available enemies.*/
IEnemy* GameWorld::FindEnemyByNameMatch(const std::string& name)
{
	auto it = std::find_if(enemies.begin(), enemies.end(), [name](IEnemy* e) { return name.find(e->GetName()) != std::string::npos; });
	if (it != enemies.end())
	{
		return *it;
	}

	return nullptr;
}

/*Waits for a system to finish it's last update. This blocks the current thread.*/
void GameWorld::WaitForIUpdatableLastUpdate(IUpdatable* system, const double& timeout, const bool& showMsgs)
{
	double delta(0.0), lastTime(0.0), currentTime(0.0);	
	currentTime = float(clock()) / CLOCKS_PER_SEC;
	if (timeout == 0.0)
	{
		while (!system->IsUpdateComplete())
		{
			lastTime= float(clock()) / CLOCKS_PER_SEC;
		}

		delta = currentTime - lastTime;

		if(showMsgs) printf("Waited for %.10fs\n", delta);
	}
	else
	{
		bool timeoutReached = false;
		double time_acc = 0.0;
		while (!system->IsUpdateComplete() && !timeoutReached)
		{
			currentTime = float(clock()) / CLOCKS_PER_SEC;
			delta = currentTime - lastTime;
			time_acc += delta;
			lastTime = currentTime;

			timeoutReached = time_acc > timeout;			
		}

		if (showMsgs)
		{
			if (timeoutReached)
			{
				printf("Waited for %.10fs. Timeout of %.10fs reached.\n", delta, timeout);
			}
			else
			{
				printf("Waited for %.10fs\n", delta);
			}
		}
	}
}

/*Starts the lua scripting system.*/
const bool GameWorld::InitializeLuaScriptingSystem()
{
	//ParameterContainer input, output;
	//input.Add("std::string", "command", "SetFunctionPointer");
	//input.Add("std::string", "functionName", "l_GO_FollowCurve");
	//input.Add("pointer", "fn", GameSystemAnimationLua::l_SYS_Printf);
	//if (!systemScriptingLua->ProcessMessage(input, output))
	//{
	//	return false;
	//}

	//input.Clear();



	return true;
}

/*Initializes the world.*/
void GameWorld::Initialize()
{
	canUpdate = false;

	bool err = systemSound->Initialize();
	if (!err)
	{
		std::string errStr = systemSound->GetLastErrorString();
		std::cout << errStr << std::endl;
	}

	InitializeLevelFactory();

	//Get all levels
	/*std::string chapterDef = EngineFolders::GetConfigPath() + "ChapterDefinition.xml";
	err = levelFactory->GetLevels(chapterDef, levelPaths);
	if (!err)
	{
		std::string errStr = levelFactory->GetLastErrorString();
		std::cout << errStr << std::endl;

		return;
	}*/

	if (gameConfig->dataPersistencyOptions.engine == (int)SystemFactory::PersistencySystem_e::XML)
	{
		levelPaths.push_back(EngineFolders::GetConfigPath() + "level.xml");
	}
	else
	{
		throw std::exception("Only XML is supported at this time!");
	}

	//systemVertexAnimation = new GameVertexAnimationSystem();

	isReadyToRender.store(false);
	std::thread dataPersistencyThread(&GameWorld::DataPersistencyThreadFunc, this);
	dataPersistencyThread.detach();

	LoadFile("load_level", levelPaths[0]);

	/*ParameterContainer parameters;
	parameters.Add(Parameter("string", "filepath", , false));
	Level* level = levelFactory->Create(parameters);*/
	
	ParameterContainer input;
	input.Add("GLFWwindow", "window", window);
	input.Add("GameConfig", "gameconfig", gameConfig);
	if (!systemUserControl->Initialize(input))
	{
		std::cout << "Failed to initialize UserControl system." << std::endl;
		return;
	}
	input.Clear();

	input.Add("GLFWwindow", "window", window);
	input.Add("IWorld", "world", this);
	if (!systemGUI->Initialize(input))
	{
		std::cout << systemGUI->GetLastErrorString() << std::endl;
		return;
	}
	systemGUI->HandleInput(true);
	OnExternalMessage(GUIMSG_TOGGLEMENU, "1", 0.0);

	canUpdate = true;	
}

/*Shares systems with the level factory.*/
void GameWorld::InitializeLevelFactory()
{
	ParameterContainer input, output;
	input.Add("ISystemAI", "systemAI", systemAI);
	input.Add("ISystemMesh", "systemMesh", systemMesh);
	input.Add("ISystemPhysics", "systemPhysics", systemPhysics);
	input.Add("ISystemSound", "systemSound", systemSound);
	input.Add("ISystemScripting", "systemScriptingLua", systemScriptingLua);

	bool rc = levelFactory->Initialize(input, output);
	if (!rc)
	{
		std::cout << "LevelFactory error: " + levelFactory->GetLastErrorString() << std::endl;
	}
}

/*Creates the level from the file.*/
void GameWorld::InitializeLevel(const std::string& levelName)
{
	//Loads the level from a file.
	std::string levelPath = EngineFolders::GetConfigPath() + levelName;

	ParameterContainer parameters;
	parameters.Add(Parameter("string", "filepath", levelPath, false));
	currentLevel = levelFactory->Create(parameters);
}

/*Starts the video system.*/
void GameWorld::InitializeVideoSystem()
{
	//systemVideo->SetWindowTitle(currentLevel->GetName());
	systemVideo->SetWindowSize(gameConfig->videoOptions.WindowWidth, gameConfig->videoOptions.WindowHeight);
	systemVideo->SetupLevel(currentLevel);
	systemVideo->ClearFrameBuffers();
	for (IGraphicsBuffer*& fbo : *currentLevel->GetFrameBuffers())
	{
		systemVideo->RegisterFrameBuffer(fbo);
	}
}

/*REturns a pointer to the game configuration.*/
GameConfig* GameWorld::GetGameConfiguration()
{
	return gameConfig;
}

/*Returns a system in the game.*/
ISystem* GameWorld::GetSystem(const std::string& systemName)
{
	if (systemName == "sound")
	{
		return systemSound;
	}
	else if (systemName == "gui")
	{
		return systemGUI;
	}
	else if (systemName == "usercontrol")
	{
		return systemUserControl;
	}
	else if (systemName == "physics")
	{
		return systemPhysics;
	}
	else if (systemName == "video")
	{
		return systemVideo;
	}
	else if (systemName == "scene")
	{
		return systemScene;
	}
	else if (systemName == "luascripting")
	{
		return systemScriptingLua;
	}
	else if (systemName == "ai")
	{
		return systemAI;
	}
	else
	{
		return nullptr;
	}
}

/*Gets a poiter inside the world.*/
void* GameWorld::GetPointer(const std::string& name)
{
	if (name == "GetCurrentLevel")
	{
		return currentLevel;
	}

	return nullptr;
}

/*Calls updates on all sub-systems in the game.*/
void GameWorld::Update(const FrameTime& frameTime)
{
	if (!canUpdate.load())
	{
		return;
	}
	this->isUpdateComplete.store(false);

	/*Stops time when the game is paused.*/
	this->isPaused ? SetFrameTime(FrameTime()) :  SetFrameTime(frameTime);
	
	/*Clamps timestep to 60fps*/
	this->frameTime.SetDeltaTime(std::max(this->frameTime.GetDeltaTime(), FIXED_DT));

	/*Initializes VS only once*/
	if (currentLevel && !isReadyToRender.load())
	{
		InitializeVideoSystem();

		systemVideo->Activate(true);
		isReadyToRender.store(true);
		OnExternalMessage(GUIMSG_LOADING, "0", 0.0);
		OnLevelLoaded();
	}

	if (currentLevel)
	{
			//PLAYER UPDATE
		{
			player->Update(this->frameTime);
		}

		/*HOSTAGE UPDATE*/
		{
			hostage->SetPlayerPosition(this->player->GetPosition());
			hostage->Update(this->frameTime);
		}

		//REACTOR UPDATE
		{
			reactor->Update(this->frameTime);
		}

		//ENEMY UPDATE
		{
		/*	Vector3D playerPosition = player->GetPosition();
			for (auto enemy : enemies)
			{
				enemy->SetPlayerPosition(playerPosition);
				enemy->Update(this->frameTime);
			}*/
		}

		for (const auto& scene : *currentLevel->GetScenes())
		{
			try
			{
				//USER CONTROL UPDATE
				{
					systemUserControl->SetScene(scene);
					systemUserControl->Update(this->frameTime);
				}
			}
			catch (...)
			{
				//Something went wrong -- LOG
			}

			try
			{
				//SCENE UPDATE
				{
					systemScene->SetScene(scene);
					systemScene->Update(this->frameTime);
				}
			}
			catch (...)
			{
				//Something went wrong -- LOG
			}

			try
			{
				//VIDEO UPDATE
				{
					systemVideo->SetScene(scene);
					systemVideo->Update(this->frameTime);
				}
			}
			catch (...)
			{
				//Something went wrong -- LOG
			}
		}
	}

	//GUI UPDATE
	{
		try
		{
			systemGUI->Update(this->frameTime);
		}
		catch (...)
		{
			//Something went wrong -- LOG
		}
	}

	this->isUpdateComplete.store(true);
}
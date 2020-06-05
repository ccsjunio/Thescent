#pragma once
#include <ISystemScripting.h>
#include "Player.h"
#include "IEnemy.h"
#include "Reactor.h"
#include "Hostage.h"
#include <functional>

//const bool GameWorld::MarkEntityToBeDisposed(Entity* entity)
using MarkEntityToBeDisposedFunctionType = const bool(Entity*);

class GameCollisionResponse
{
public:
	static void DisableEntity(Entity*& projectile);
	static void OnPlayerHit(Player*& player, IEnemy*& enemy);
	static const bool OnPlayerEscaping(ISystemScripting*& systemScripting, Scene*& mainscene, Scene*& cockpitScene, Scene*& depositScene);
	static const bool OnPlayerReactorContact(Player*& player, Reactor*& reactor, ISystemScripting*& systemScripting, Scene*& mainscene, Scene*& depositScene);
	static const bool OnEnemyHit(const std::string& projectileName, Player*& player, IEnemy*& enemy, Reactor*& reactor, ISystemScripting*& systemScripting, Scene*& mainscene);
	static const bool OnDoor01Opened(ISystemScripting*& systemScripting, Scene*& mainscene);
	static const bool OnDoor02Opened(ISystemScripting*& systemScripting, Scene*& mainscene);
	static const bool OnDoor02AccessDenied(Scene*& mainscene);
	static const bool OnReactorHit(Player*& player, Reactor*& reactor, ISystemScripting*& systemScripting, Scene*& mainscene, Scene*& depositScene);
	static const bool OnHostageRescued(Hostage*& hostage);
	static const bool OnBossAppearing(Scene*& deposit_scene, Scene*& mainScene);
	static const bool OnWTWCannonEnable(Player*& player, Scene*& mainScene, Scene*& depositScene);
};


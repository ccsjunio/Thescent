#include "GameCollisionResponse.h"
#include <IRender.h>
#include "Enemy.h"
#include "Boss.h"
#include <ThreadID.h>

void GameCollisionResponse::DisableEntity(Entity*& entity)
{
	if (entity)
	{
		std::vector<IComponent*> components;
		if (entity->GetComponents(components))
		{
			for (auto& component : components)
			{
				component->Activate(false);

				/*IRigidbody* rb = dynamic_cast<IRigidbody*>(component);
				if (rb)
				{
					rb->SetPosition(Vector3D(0.0, 5000.0, 0.0));
				}*/
			}
		}
		entity->SetDisposing(true);

	/*	if (entity->GetName().find("CLONED_PROJECTILE") != std::string::npos)
		{
			auto rb = entity->GetComponent<IRigidbody*>();
			if (rb)
			{
				rb->SetPosition(Vector3D(0.0, 5000.0, 0.0));
			}
		}*/

	/*	auto vr = entity->GetComponent<IRender*>();
		if (vr) vr->SetVisibility(false);*/
	}
}

/*Causes the player to take damage.*/
void GameCollisionResponse::OnPlayerHit(Player*& player, IEnemy*& enemy)
{
	player->TakeDamage(enemy->GetDamage());

	printf("Player health %.2f\n", player->GetHealth());
}

const bool GameCollisionResponse::OnPlayerEscaping(ISystemScripting*& systemScripting, Scene*& mainscene, Scene*& cockpitScene, Scene*& depositScene)
{
	//Remove exit trigger -- really a physics ghost object
	auto exit_trigger = mainscene->FindEntityByName("EXIT_CUBE");
	if (exit_trigger)
	{
		depositScene->Append(exit_trigger);
		mainscene->Remove(exit_trigger, false);
	}
	else return false;

	/*Puts thescent model into the mainscene*/
	auto thescent_ship = depositScene->FindEntityByName("THESCENT_SHIP");
	if (thescent_ship)
	{
		mainscene->Append(thescent_ship);
		depositScene->Remove(thescent_ship, false);
	}
	else return false;

	//Disable cockpit
	auto thescent_cockpit = cockpitScene->FindEntityByName("DescentCockpitQuad");
	if (thescent_cockpit)
	{
		IRender* vr =thescent_cockpit->GetComponent<IRender*>();
		if (vr) vr->SetVisibility(false);
	}
	else return false;
	auto thescent_crosshair = cockpitScene->FindEntityByName("DescentCrosshairQuad");
	if (thescent_crosshair)
	{
		IRender* vr = thescent_crosshair->GetComponent<IRender*>();
		if (vr) vr->SetVisibility(false);
	}
	else return false;

	//Move camera away
	auto player_sphere = mainscene->FindEntityByName("PLAYER_COLLISIONSPHERE");
	if (player_sphere)
	{
		auto playerRB = player_sphere->GetComponent<IRigidbody*>();
		if(playerRB)
		{
			playerRB->ToggleCollisionDetection(false);
			playerRB->SetMass(0.0f);

			playerRB->SetPosition(Vector3D(-100.0f, 80.0f, 400.0f));

			Quaternion initialRotation = playerRB->GetRotation();
			Vector3D normalizedDistance = glm::normalize(Vector3D(0.0, 10, 180.0) - playerRB->GetPosition());
			Quaternion rotationDirection = glm::quatLookAt(normalizedDistance, playerRB->GetUp());
			Quaternion slerp = glm::slerp(initialRotation, rotationDirection, 1.0f);
			playerRB->SetRotation(slerp);
		}
	}
	else return false;

	/*Fire Scripted animation for the ship leaving cutscene*/
	if (!systemScripting->PlayAnimation("ThescentEscape", mainscene->GetName(), true))
	{
		return false;
	}

	return true;
}

const bool GameCollisionResponse::OnPlayerReactorContact(Player*& player, Reactor*& reactor, ISystemScripting*& systemScripting, Scene*& mainscene, Scene*& depositScene)
{
	/*If the reactor is alive, ddeals damage*/
	if (reactor->IsAlive())
	{
		player->TakeDamage(FLT_MAX);
		reactor->TakeDamage(FLT_MAX);

		/*Check if the reactor was destroyed*/
		if (!reactor->IsAlive())
		{
			/*Animates the reactor destruction*/
			Entity* entity = reactor->GetEntity();
			if (entity)
			{
				IRender* vr = entity->GetComponent<IRender*>();
				if (vr)
				{
					ShaderFeature* sf = vr->GetShaderFeature("BASIC_VERTEX_DRAW");
					if (sf)
					{
						sf->parameters.SetValue("ENABLE_OFFSET_ALONG_NORMAL", "1");
					}
				}

				/*Find sound component and activates it*/
				IComponent* c = entity->GetComponentByName("ReactorDown");
				if (c) c->Activate(true);

				/*Fire Scripted animation to blow up the entry*/
				systemScripting->PlayAnimation("BlowEntry", mainscene->GetName(), true);

				//Add exit trigger -- really a physics ghost object
				auto exit_trigger = depositScene->FindEntityByName("EXIT_CUBE");
				if (exit_trigger)
				{
					mainscene->Append(exit_trigger);
					depositScene->Remove(exit_trigger, false);
				}
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

/*Returns if the enemy died or not*/
const bool GameCollisionResponse::OnEnemyHit(const std::string& projectileName, Player*& player, IEnemy*& enemy, Reactor*& reactor, ISystemScripting*& systemScripting, Scene*& mainscene)
{
	float damage = 0.0f;
	if (player->GetWeaponDamageByProjectileName(projectileName, damage))
	{
		if (enemy && enemy->IsAlive())
		{
			enemy->TakeDamage(damage);
			printf("Enemy %s health %.2f\n", enemy->GetName().c_str(), enemy->GetHealth());
			if (!enemy->IsAlive())
			{
				/*Play the explosion sound*/
				Entity* scene_entity = mainscene->FindEntityByName("SCENE_ENTITY");
				if (scene_entity)
				{
					/*Find sound component and activates it*/
					IComponent* c = scene_entity->GetComponentByName("EnemyExplosion");
					if (c) c->Activate(true);
				}

				/*If the enemy dies, disposes the enemy's entities*/
				Enemy* en = dynamic_cast<Enemy*>(enemy);
				if (en)
				{
					/*Dispose entity*/
					auto entity = en->GetEntity();
					DisableEntity(entity);
				}

				else
				{
					Boss* boss = dynamic_cast<Boss*>(enemy);
					if (boss)
					{
						/*Disposes entities*/
						auto body = boss->GetEntity("BODY");
						auto top = boss->GetEntity("TOP");
						auto bottom = boss->GetEntity("BOTTOM");
						DisableEntity(body);
						DisableEntity(top);
						DisableEntity(bottom);

						/*If the boss is dead, open gate to reactor*/
						systemScripting->PlayAnimation("OpenDoorBars", mainscene->GetName(), true);

						/*Allow reactor to take damage*/
						reactor->AllowDamage(true);
					}
				}

				return true;
			}
		}
	}

	return false;
}

/*Fires the door 2 animation.*/
const bool GameCollisionResponse::OnDoor01Opened(ISystemScripting*& systemScripting, Scene*& mainscene)
{
	if (!mainscene) return false;

	systemScripting->PlayAnimation("OpenDoor1", mainscene->GetName(), true);

	/*Play the door opening sound*/
	Entity* scene_entity = mainscene->FindEntityByName("SCENE_ENTITY");
	if (scene_entity)
	{
		/*Find sound component and activates it*/
		IComponent* c = scene_entity->GetComponentByName("HatchOpen");
		if (c) c->Activate(true);
	}

	return true;
}

/*Fires the door 2 animation.*/
const bool GameCollisionResponse::OnDoor02Opened(ISystemScripting*& systemScripting, Scene*& mainscene)
{
	if (!mainscene) return false;

	systemScripting->PlayAnimation("OpenDoor2", mainscene->GetName(), true);

	/*Play the door opening sound*/
	Entity* scene_entity = mainscene->FindEntityByName("SCENE_ENTITY");
	if (scene_entity)
	{
		/*Find sound component and activates it*/
		IComponent* c = scene_entity->GetComponentByName("HatchOpen");
		if (c) c->Activate(true);
	}

	/*Disable collision detection on the door*/
	Entity* door = mainscene->FindEntityByName("DOOR_02");
	IRigidbody* rb = door->GetComponent<IRigidbody*>();
	rb->ToggleCollisionDetection(false);

	return true;
}

const bool GameCollisionResponse::OnDoor02AccessDenied(Scene*& mainscene)
{
	/*Play the door opening sound*/
	Entity* scene_entity = mainscene->FindEntityByName("SCENE_ENTITY");
	if (scene_entity)
	{
		/*Find sound component and activates it*/
		IComponent* c = scene_entity->GetComponentByName("Denied");
		if (c)
		{
			c->Activate(true);
			return true;
		}
	}

	return false;
}

/*Returns true if the reactor was destroyed.*/
const bool GameCollisionResponse::OnReactorHit(Player*& player, Reactor*& reactor, ISystemScripting*& systemScripting, Scene*& mainscene, Scene*& depositScene)
{
	/*Get the damage done by the player*/
	float damage = 0.0f;
	if (player->GetCurrentWeaponDamage(damage))
	{
		/*If the reactor is alive, ddeals damage*/
		if (reactor->IsAlive())
		{
			reactor->TakeDamage(damage);

			/*Check if the reactor was destroyed*/
			if (!reactor->IsAlive())
			{
				/*Animates the reactor destruction*/
				Entity* entity = reactor->GetEntity();
				if (entity)
				{
					IRender* vr = entity->GetComponent<IRender*>();
					if (vr)
					{
						ShaderFeature* sf = vr->GetShaderFeature("BASIC_VERTEX_DRAW");
						if (sf)
						{
							sf->parameters.SetValue("ENABLE_OFFSET_ALONG_NORMAL", "1");
						}
					}

					/*Find sound component and activates it*/
					IComponent* c = entity->GetComponentByName("ReactorDown");
					if (c) c->Activate(true);

					/*Fire Scripted animation to blow up the entry*/
					systemScripting->PlayAnimation("BlowEntry", mainscene->GetName(), true);

					//Add exit trigger -- really a physics ghost object
					auto exit_trigger = depositScene->FindEntityByName("EXIT_CUBE");
					if (exit_trigger)
					{
						mainscene->Append(exit_trigger);
						depositScene->Remove(exit_trigger, false);
					}
				}

				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

/*Returns if the hostage was successfully rescued*/
const bool GameCollisionResponse::OnHostageRescued(Hostage*& hostage)
{
	hostage->Rescue();

	return hostage->WasRescued();
}

/*Moves the boss's entities from the deposit scene to the main scene.*/
const bool GameCollisionResponse::OnBossAppearing(Scene*& deposit_scene, Scene*& mainScene)
{
	if (!deposit_scene || !mainScene) return false;

	/*Moves the boss's entities from one scene to another.*/
	Entity* bossBody = deposit_scene->FindEntityByName("ENEMY_BOSS_BODY");
	if (!bossBody) return false;

	Entity* bossTop = deposit_scene->FindEntityByName("ENEMY_BOSS_TURRET_TOP");
	if (!bossTop) return false;

	Entity* bossBottom = deposit_scene->FindEntityByName("ENEMY_BOSS_TURRET_BOTTOM");
	if (!bossBottom) return false;

	deposit_scene->Remove(bossBody, false);
	deposit_scene->Remove(bossTop, false);
	deposit_scene->Remove(bossBottom, false);

	mainScene->Append(bossBody);
	mainScene->Append(bossTop);
	mainScene->Append(bossBottom);

	/*Plays the boss's roar*/
	IComponent* c = bossBody->GetComponentByName("BossRoar");
	if (c)
	{
		c->Activate(true);
	}
	else
	{
		return false;
	}

	/*Move player to the last position of entity vector - for transparency*/
	Entity* playerColl = mainScene->FindEntityByName("PLAYER_COLLISIONSPHERE");
	mainScene->Remove(playerColl, false);
	mainScene->Append(playerColl);

	return true;
}

/*Creates the WTW cannon.*/
const bool GameCollisionResponse::OnWTWCannonEnable(Player*& player, Scene*& mainScene, Scene*& depositScene)
{
	/*Get weapon projectile*/
	Entity* proj = mainScene->FindEntityByName("PROJECTILE_WTW");

	if (proj)
	{
		/*Modify object*/
		IRigidbody* rb = proj->GetComponent<IRigidbody*>();
		if (rb)
		{
			rb->ToggleCollisionDetection(true);
			rb->SetMass(2.0f);
		}

		ILight* lt = proj->GetComponent<ILight*>();
		if (lt)
		{
			lt->SetVisibility(true);
		}

		/*Move projectile to the deposit scene*/
		depositScene->Append(proj);
		/*And remove it from mainscene*/
		mainScene->Remove(proj, false);

		/*Create the weapon for the player using the clone*/
		player->SetupWeapon(Player::Weapon_s::Type_e::WTWCannon, proj, Vector3D(0.0f, 0.0f, -3.0f), 200.0f);

		/*If the setup was a success, the weapon will be returned*/
		Player::Weapon_s* weapon = player->FindWeaponByProjectileName(proj->GetName());
		if (weapon)
		{
			/*If the weapon is not available, make it available*/
			if (!weapon->available)
			{
				weapon->available = true;
			}

			/*Play voice over*/
			Entity* main_scene_entity = mainScene->FindEntityByName("SCENE_ENTITY");
			if (main_scene_entity)
			{
				auto c = main_scene_entity->GetComponentByName("WTWCannon");
				if (c) c->Activate(true);
			}

			return true;			
		}
	}

	return false;
}

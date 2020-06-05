#include "Player.h"
#include <BellEngineID.h>
#include <IRender.h>
#include <StringHelpers.h>
#include <ISoundComponent.h>
#include "GameWindowFlags.h"
#include "Enemy.h"
#include "Boss.h"

constexpr auto PROJECTILE_BLASTER_IMPULSE =	60.0f;
constexpr auto PROJECTILE_CANNON_IMPULSE =	40.0f;
constexpr auto PLAYER_RANGE =				60.0f; /*Auto-Aim Raycast*/
constexpr auto SOUND_FX_NIGHTVISION =		"NightVision";
constexpr auto SOUND_FX_WEAPON_BLASTER =	"BlasterSound";
constexpr auto SOUND_FX_WEAPON_WTWCANNON =	"WTWCannonBlast";

/*Position of the player in 3D space.*/
const Vector3D Player::GetPosition()
{
	return camera->GetRigidbody()->GetPosition();
}
/*Sets the state of the player's items.*/
void Player::SetItemState(const unsigned int& itemID, const bool& state)
{
	switch (itemID)
	{
	case 1: this->canUseAnticloak.store(state); break;
	default: return;
	}
}
/*Returns the if an item is available or not.*/
const bool Player::GetItemState(const unsigned int& itemID)
{
	switch (itemID)
	{
	case 1: return this->canUseAnticloak.load();
	default: return false;
	}
}
/*Sets the state of the anticloacking device.*/
void Player::SetAnticloakState(const bool& state)
{
	this->isAnticloakEnabled.store(state);

	if (!ToggleSoundFX(SOUND_FX_NIGHTVISION, true))
	{
		printf("Could not play NightVision sound FX!\n");
	}
}
/*Gets the state of the anticloacking device.*/
const bool Player::GetAnticloakState()
{
	return this->isAnticloakEnabled.load();
}
/*Turns on/off a sound effect.*/
const bool Player::ToggleSoundFX(const std::string& soundComponentName, const bool& play)
{
	IComponent* c = camera->GetTarget()->GetComponentByName(soundComponentName);
	if (c)
	{
		ISoundComponent* sc = dynamic_cast<ISoundComponent*>(c);
		if (sc)
		{
			sc->Activate(play);
			return true;
		}
	}

	return false;
}
/*Event handling for external game world messages.*/
void Player::BindOnExternalMessage(std::function<void(const unsigned int&, const std::string&, const double&, const unsigned int& renderAs)> fn)
{
	if (!fn) return;

	this->onExternalMessage = std::bind(fn, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
}
/*Finds the closes enemy near the player.*/
IEnemy* Player::FindClosestEnemyInRange(const float& radius, const std::vector<IEnemy*>& enemies, Vector3D& enemyPosition)
{
	IRigidbody* playerRB = camera->GetRigidbody();

	if (playerRB)
	{
		float closestDistance = FLT_MAX;
		float currentDistance = 0.0f;
		IEnemy* closestEnemy = nullptr;
		IRigidbody* enemyRB = nullptr;
		Vector3D playerPosition = playerRB->GetPosition();

		for (IEnemy* enemy : enemies)
		{
			/*Ignores dead enemies - these will contain nullptr entities as we delete them at the end of the main thread*/
			if (enemy->IsAlive())
			{
				Entity* enemyEntity = nullptr;
				Enemy* enemy_ = dynamic_cast<Enemy*>(enemy);
				if (enemy_)
				{
					enemyEntity = enemy_->GetEntity();
				}
				else
				{
					Boss* boss = dynamic_cast<Boss*>(enemy);
					if (boss)
					{
						enemyEntity = boss->GetEntity("ENEMY_BOSS");
					}
					else
					{
						continue;
					}
				}

				/*Check if player is facing the enemy*/
				if (camera->IsFacingEntity(enemyEntity))
				{
					enemyRB = enemyEntity->GetComponent<IRigidbody*>();
					/*Raycast to the enemy to make sure it is within the AIM of the player*/
					if (enemyRB->GetCollider()->Raycast(playerPosition, playerRB->GetForwardInWorld(), radius))
					{
						enemyPosition = enemyRB->GetPosition();
						currentDistance = glm::distance(playerPosition, enemyPosition);

						/*Ignores enemies outside the radius around the player*/
						if (currentDistance < radius)
						{
							/*Figures out if the current enemy is the closes one*/
							if (currentDistance < closestDistance)
							{
								/*if (addDebugLine)
								{
									Vector3D rayEnd = playerPosition + playerRB->GetForwardInWorld() * radius;
									addDebugLine(playerPosition, rayEnd, Vector4D(1.0f, 0.0f, 0.0f, 1.0f), 5.0f);
								}*/

								closestDistance = currentDistance;
								closestEnemy = enemy;
							}
						}
					}
				}
			}
		}

		if (!closestEnemy) enemyPosition = Vector3D();

		return closestEnemy;
	}

	return nullptr;
}
/*Fires the laser blaster.*/
const bool Player::FireLaserBlaster(const std::vector<IEnemy*>& enemies)
{
	Player::Weapon_s* weapon = weapons[selectedWeapon];
	if (weapon)
	{
		Entity* clone = (Entity*)weapon->projectileBuffer.Next();
		if (clone)
		{	
			/*Wake entity*/			
			std::vector<IComponent*> components;
			if (clone->GetComponents(components))
			{
				for (auto& component : components)
				{
					component->Activate(true);
				}
			}

			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				vr->SetVisibility(true);
			}

			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				/*Figure out which side of the ship is going to fire the projectile.*/
				Vector3D startPos = weapon->startbarrelPosition;
				startPos.x *= weapon->switchShootingSide ? -1.0f : 1.0f;
				weapon->switchShootingSide = !weapon->switchShootingSide;

				/*Transform the starting point for the trajectory, which is the position of the barrel.*/
				Vector3D transformedbarrelPosition = camera->GetRigidbody()->GetWorldMatrix() * Vector4D(startPos, 1.0f);
				rb->SetRotation(camera->GetRigidbody()->GetRotation());
				rb->SetPosition(transformedbarrelPosition);
				rb->ToggleCollisionDetection(true);

				/*Find the direction of the projectile and velocity.*/
				Vector3D direction, enemyPosition;
				/*Find closest enemy in range of the weapon*/
				if (FindClosestEnemyInRange(PLAYER_RANGE, enemies, enemyPosition))
				{
					direction = glm::normalize(enemyPosition - transformedbarrelPosition);
				}
				else
				{
					/*No enemy found, fire at a point in space.*/
					Vector3D farpoint = camera->GetRigidbody()->GetWorldMatrix() * Vector4D(crosshair.pointAheadOfPlayer, 1.0f);
					direction = glm::normalize(farpoint - transformedbarrelPosition);
				}

				/*Find the direction of the projectile and velocity.*/
				rb->SetVelocity(direction * PROJECTILE_BLASTER_IMPULSE);

				/*Rotate projectile towards direction of movement*/
				rb->SetRotation(glm::quatLookAt(direction, rb->GetUp()));

				rb->Transform();
			}

			if (rb && vr)
			{
				clone->SetDisposing(false);
				ToggleSoundFX(SOUND_FX_WEAPON_BLASTER, true);

				return true;
			}
			else
			{
				//DisposePointer(clone);
			}
		}
	}

	return false;
}
/*Fires the WTW Cannon.*/
const bool Player::FireWTWCannon(const std::vector<IEnemy*>& enemies)
{
	Weapon_s* weapon = weapons[selectedWeapon];
	if (weapon)
	{
		Entity* clone = (Entity*)weapon->projectileBuffer.Next();
		if (clone)
		{			
			/*Wake entity*/
			std::vector<IComponent*> components;
			if (clone->GetComponents(components))
			{
				for (auto& component : components)
				{
					component->Activate(true);
				}
			}

			IRender* vr = clone->GetComponent<IRender*>();
			if (vr)
			{
				vr->SetVisibility(true);
			}

			IRigidbody* rb = clone->GetComponent<IRigidbody*>();
			if (rb)
			{
				//rb->SetParent(clone);

				/*Figure out which side of the ship is going to fire the projectile.*/
				Vector3D startPos = weapon->startbarrelPosition;

				/*Transform the starting point for the trajectory, which is the position of the barrel.*/
				Vector3D transformedbarrelPosition = camera->GetRigidbody()->GetWorldMatrix() * Vector4D(startPos, 1.0f);
				rb->SetRotation(camera->GetRigidbody()->GetRotation());
				rb->SetPosition(transformedbarrelPosition);
				rb->ToggleCollisionDetection(true);

				/*Find the direction of the projectile and velocity.*/
				Vector3D direction, enemyPosition;
				/*Find closest enemy in range of the weapon*/
				if (FindClosestEnemyInRange(PLAYER_RANGE, enemies, enemyPosition))
				{
					direction = glm::normalize(enemyPosition - transformedbarrelPosition);
				}
				else
				{
					/*No enemy found, fire at a point in space.*/
					Vector3D farpoint = camera->GetRigidbody()->GetWorldMatrix() * Vector4D(crosshair.pointAheadOfPlayer, 1.0f);
					direction = glm::normalize(farpoint - transformedbarrelPosition);
				}

				/*Find the direction of the projectile and velocity.*/
				rb->SetVelocity(direction * PROJECTILE_CANNON_IMPULSE);

				/*Rotate projectile towards direction of movement*/
				rb->SetRotation(glm::quatLookAt(direction, rb->GetUp()));

				rb->Transform();
			}

			if (rb && vr)
			{
				//currentScene->Append(clone);
				clone->SetDisposing(false);
				ToggleSoundFX(SOUND_FX_WEAPON_WTWCANNON, true);

				return true;
			}
			else
			{
				//DisposePointer(clone);
			}
		}
	}

	return false;
}
/*Constructor.*/
Player::Player()
{
	lastMouseXPosition = lastMouseYPosition = 0.0;
	SetHealth(100.0f);
	camera = nullptr;
	isUpdateComplete = false;
	selectedWeapon = 0;
}
/*Destructor.*/
Player::~Player()
{
	Dispose();
}
/*Returns if the last update cycle has been completed.*/
const bool Player::IsUpdateComplete()
{
	return this->isUpdateComplete;
}
/*Returns the current health of the player.*/
const float Player::GetHealth()
{
	return this->health.load();
}
/*Sets the current health of the player.*/
void Player::SetHealth(const float& health)
{
	this->health.store(health);
}
/*Causes damage to the player.*/
void Player::TakeDamage(const float& damage)
{
	this->health.store(this->health.load() - damage);

	if (this->health.load() < 0.0f)
	{
		this->health.store(0.0f);
	}
}
/*Sets up a player weapon.*/
void Player::SetupWeapon(const Player::Weapon_s::Type_e& type, Entity*& projectile, const Vector3D& barrelOffsetToPlayer, const float& damage)
{
	Weapon_s* weapon = new Weapon_s(type, false, projectile, /*camera->GetRigidbody()->GetPosition() +*/ barrelOffsetToPlayer, damage);
	weapons.push_back(weapon);
}
/*Selects a game weapon.*/
const bool Player::SelectWeapon(const unsigned int& id)
{
	if (id < weapons.size())
	{
		selectedWeapon = id;
		return true;
	}

	return false;
}
/*Fires the current selected weapon towards a group of enemies*/
const bool Player::FireWeapon(const std::vector<IEnemy*>& enemies)
{
	//FIRE
	switch (selectedWeapon)
	{
		/*Blaster*/
	case 0: default: return FireLaserBlaster(enemies);
	case 1: return FireWTWCannon(enemies);
	}

	return false;
}
/*Returns the current weapon's damage.*/
const bool Player::GetCurrentWeaponDamage(float& damage)
{
	return GetWeaponDamage(selectedWeapon, damage);
}
/*Gets the damage of any weapon.*/
const bool Player::GetWeaponDamage(const unsigned int& id, float& damage)
{
	if (weapons.size() > id)
	{
		damage = weapons[id]->damage;
		return true;
	}

	return false;
}
/*Gets the damage of a weapon by using the projectile name.*/
const bool Player::GetWeaponDamageByProjectileName(const std::string& name, float& damage)
{
	Player::Weapon_s* weapon = FindWeaponByProjectileName(name);
	if (weapon)
	{
		damage = weapon->damage;
		return true;
	}

	damage = 0.0f;
	return false;
}
/*Finds a weapon by the projectile name.*/
Player::Weapon_s* Player::FindWeaponByProjectileName(const std::string& name)
{
	auto it = std::find_if(weapons.begin(), weapons.end(), [name](Player::Weapon_s* w) { return name.find(w->projectile->GetName()) != std::string::npos; });
	if (it != weapons.end())
	{
		return *it;
	}

	return nullptr;
}
/*Moves the player in 3D space.*/
void Player::UpdatePosition(const Player::PlayerMovement_e& action, const float& movementStep)
{
	camera->SetMoveFactor(movementStep);

	switch (action)
	{
	case PlayerMovement_e::Halt: camera->GetRigidbody()->ClampVelocity(); break;
	case PlayerMovement_e::StrafeLeft: camera->StrafeLeft(); break;
	case PlayerMovement_e::StrafeRight: camera->StrafeRight(); break;
	case PlayerMovement_e::Forward: camera->MoveForward(); break;
	case PlayerMovement_e::Backward: camera->MoveBackward(); break;
	case PlayerMovement_e::HooverUp: camera->MoveUp(); break;
	case PlayerMovement_e::HooverDown: camera->MoveDown(); break;
	default:
		break;
	}
}
/*Moves the camera eye in 2D space.*/
void Player::UpdateEye(const double& x, const double& y, const float& sensitivity)
{
	float deltaX = (float)(lastMouseXPosition - x);
	float deltaY = (float)(lastMouseYPosition - y);

	IRigidbody* cameraTarget = ((CameraLockedAt*)camera)->GetRigidbody();
	if (cameraTarget)
	{
		Quaternion currentRotation = cameraTarget->GetRotation();
		Quaternion yaw = MathHelper::EulerAnglesToQuaternion(Vector3D(deltaY * sensitivity, 0.0f, 0.0f));		
		Quaternion pitch = MathHelper::EulerAnglesToQuaternion(Vector3D(0.0f, deltaX * sensitivity, 0.0f));
		
		currentRotation = pitch * currentRotation * yaw;

		cameraTarget->SetRotation(currentRotation);
	}

	lastMouseXPosition = x;
	lastMouseYPosition = y;
}
/*Sets up the camera used by the current player*/
void Player::SetCamera(ICamera* camera)
{
	if (camera->GetTypeID() == ENGINE_OBJ_ID_GPH_CAMERALOCKEDAT)
	{
		this->camera = (CameraLockedAt*)camera;

		Quaternion initialRotation = this->camera->GetRigidbody()->GetRotation();
		Vector3D normalizedDistance = glm::normalize(Vector3D(0.0f, 0.5f, -300.0f) - this->camera->GetRigidbody()->GetPosition());
		Quaternion rotationDirection = glm::quatLookAt(normalizedDistance, this->camera->GetRigidbody()->GetUp());
		this->camera->GetRigidbody()->SetRotation(rotationDirection);
	}
	else
	{
		printf("Could not bind camera to player.");
	}
}
/*Updates the player.*/
void Player::Update(const FrameTime& frameTime)
{
	this->isUpdateComplete = false;

	SetFrameTime(frameTime);

	/*Makes sure the first weapon is always enabled*/
	if (!weapons.empty())
	{
		if (!weapons[0]->available) weapons[0]->available = true;
	}

	if (drawDebugObjects)
	{
		drawDebugObjects();
	}

	if (onExternalMessage)
	{
		/*Health messages*/
		unsigned int flags = 0u;
		flags |= GUIMSG_HEALTH;
		char buf[255];
		sprintf_s(buf, 255, "%03.0f", this->health.load());
		onExternalMessage(flags, buf, 0.0, 0);
	}

	this->isUpdateComplete = true;
}
/*Releases resources.*/
void Player::Dispose()
{
	DisposeVector(weapons);
}

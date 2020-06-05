#pragma once
#include <Entity.h>
#include <CameraLockedAt.h>
#include <IUpdatable.h>
#include <IDisposable.h>
#include <Scene.h>
#include <atomic>
#include "IEnemy.h"
#include <Entity.h>
#include <functional>
#include <CyclicContainer.h>

/*Defines the main character of the game, or the player.*/
class Player : public IUpdatable, public IDisposable
{
private:
	/*Defines a virtual crosshair ahead of the player*/
	struct Crosshair_s
	{
		const Vector3D pointAheadOfPlayer = Vector3D(0.0f, 0.0f, -10.f);
	};

	std::function<void(const unsigned int&, const std::string&, const double&, const unsigned int&)> onExternalMessage;
	unsigned int selectedWeapon;
	std::atomic<float> health;	
	CameraLockedAt* camera;
	std::atomic<bool> isAnticloakEnabled, canUseAnticloak;
	bool isUpdateComplete;
	double lastMouseXPosition, lastMouseYPosition;
	Crosshair_s crosshair;
	struct Weapon_s;
	std::vector<Weapon_s*> weapons;

	/*Finds the closes enemy near the player.*/
	IEnemy* FindClosestEnemyInRange(const float& radius, const std::vector<IEnemy*>& enemies, Vector3D& enemyPosition);
	/*Fires the laser blaster.*/
	const bool FireLaserBlaster(const std::vector<IEnemy*>& enemies);
	/*Fires the WTW Cannon.*/
	const bool FireWTWCannon(const std::vector<IEnemy*>& enemies);

public:
	std::function<void(const Vector3D&, const Vector3D&, const Vector4D&, const float&)> addDebugLine;
	std::function<void()> drawDebugObjects;
	/*Defines a generic weapon.*/
	class Weapon_s
	{
	public:
		enum class Type_e
		{
			/*A physics-based laser blaster.*/
			Blaster,
			/*A physics-based heavy-energy blaster.*/
			WTWCannon
		};

		Type_e type;
		bool available, switchShootingSide;
		Entity* projectile;
		/*Contains copies of the projectile.*/
		CyclicContainer<Entity*> projectileBuffer;
		Vector3D startbarrelPosition;
		float damage;

		Weapon_s()
		{
			this->type = Type_e::Blaster;
			this->available = true;
			this->switchShootingSide = true;
			this->projectile = nullptr;
			this->startbarrelPosition = Vector3D();
			this->damage = 0.0f;
		}
		Weapon_s(const Type_e& type, const bool& available, Entity*& projectile, const Vector3D& startbarrelPosition, const float& damage, const unsigned int& projectileBufferSz = 10u)
		{
			this->switchShootingSide = true;
			this->type = type;
			this->available = available;
			/*Creates a buffer of projectiles*/
			this->projectile = projectile;
			if (this->projectile)
			{
				this->projectile->SetDisposing(true);
				this->projectileBuffer.Reserve(projectileBufferSz);

				auto projectile_scene = this->projectile->GetParent<Scene*>();
				if (projectile_scene)
				{
					for (unsigned int i = 0; i < projectileBufferSz; i++)
					{
						auto clone = (Entity*)this->projectile->Clone();
						if (clone)
						{
							clone->SetName("CLONED_" + clone->GetName() + "_FIREDBYPLAYER" /*+ std::to_string(i)*/);

							projectileBuffer.Add(clone);

							projectile_scene->Append(clone);
						}
					}
				}
			}

			this->startbarrelPosition = startbarrelPosition;
			this->damage = damage;
		}
		~Weapon_s()
		{

		}
		
	};
	enum class PlayerMovement_e
	{
		Halt,

		Forward,
		Backward,
		StrafeLeft,
		StrafeRight,
		HooverUp,
		HooverDown
	};
	
	/*Constructor*/
	Player();
	/*Destructor*/
	~Player();

	/*Position of the player in 3D space.*/
	const Vector3D GetPosition();
	/*Sets the state of the player's items.*/
	void SetItemState(const unsigned int& itemID, const bool& state);
	/*Returns the if an item is available or not.*/
	const bool GetItemState(const unsigned int& itemID);
	/*Sets the state of the anticloacking device.*/
	void SetAnticloakState(const bool& state);
	/*Gets the state of the anticloacking device.*/
	const bool GetAnticloakState();
	/*Turns on/off a sound effect.*/
	const bool ToggleSoundFX(const std::string& soundComponentName, const bool& play);
	/*Event handling for external game world messages.*/
	void BindOnExternalMessage(std::function<void(const unsigned int&, const std::string&, const double&, const unsigned int&)> fn);
	/*Returns if the last update cycle has been completed.*/
	const bool IsUpdateComplete();
	/*Returns the current health of the player.*/
	const float GetHealth();
	/*Sets the current health of the player.*/
	void SetHealth(const float& health);
	/*Causes damage to the player.*/
	void TakeDamage(const float& damage);
	/*Sets up a player weapon.*/
	void SetupWeapon(const Player::Weapon_s::Type_e& type, Entity*& projectile, const Vector3D& startbarrelPosition, const float& damage);
	/*Selects a game weapon.*/
	const bool SelectWeapon(const unsigned int& id);
	/*Fires the current selected weapon towards a group of enemies*/
	const bool FireWeapon(const std::vector<IEnemy*>& enemies);
	/*Returns the current weapon's damage.*/
	const bool GetCurrentWeaponDamage(float& damage);
	/*Gets the damage of any weapon.*/
	const bool GetWeaponDamage(const unsigned int& id, float& damage);
	/*Gets the damage of a weapon by using the projectile name.*/
	const bool GetWeaponDamageByProjectileName(const std::string& name, float& damage);
	/*Finds a weapon by the projectile name.*/
	Player::Weapon_s* FindWeaponByProjectileName(const std::string& name);
	/*Moves the player in 3D space.*/
	void UpdatePosition(const Player::PlayerMovement_e& action, const float& movementStep);
	/*Moves the camera eye in 2D space.*/
	void UpdateEye(const double& x, const double& y, const float& sensitivity);
	/*Sets up the camera used by the current player*/
	void SetCamera(ICamera* camera);
	/*Updates the player.*/
	void Update(const FrameTime& frameTime);
	/*Releases resources*/
	void Dispose();
};


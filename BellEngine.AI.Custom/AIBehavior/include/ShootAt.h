#pragma once
#include <IAIBehavior.h>
#include <Entity.h>
#include <Scene.h>
#include <MathLibrary.h>
#include <CyclicContainer.h>

class ShootAt : public IAIBehavior
{
private:
    bool isActive, isUpdateComplete;
    std::string name;
    float radius, intervalBetweenShots, timeAcc_intervalBetweenShots, projectileImpulse;
    Entity* target, * projectile;
    CyclicContainer<Entity*> projectileBuffer;
    Scene* scene;
    Vector3D pointAheadOfParent;
    std::vector<Vector3D> startPositions;
    std::function<void(const IAIBehavior::AIEvent_s&)> aiEvent;

    const bool IsThereAnObstacle(Entity*& parent);
    void Shoot(Entity*& parent);
  
public:
    ShootAt() :
        radius(0.0f),
        intervalBetweenShots(0.0f),
        timeAcc_intervalBetweenShots(0.0f),
        projectileImpulse(0.0f),
        pointAheadOfParent(Vector3D(0.0f, 0.0f, 1.0f)),
        isUpdateComplete(false),
        isActive(true),
        target(nullptr),
        projectile(nullptr),
        scene(nullptr)
    {};
    ~ShootAt();

    void SetProjectileImpulse(const float& value);
    const float GetProjectileImpulse();
    void BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn);
    /*Sets a point ahead of the parent. This is the 'target' for the caclulation of the bullet trajectory.*/
    void SetCrosshair(const Vector3D& value);
    const Vector3D GetCrosshair();
    const bool AddBarrelPosition(const Vector3D& value);
    const bool GetBarrelPositions(std::vector<Vector3D>& value);
    void SetRadius(const float& radius);
    const float GetRadius();
    void SetIntervalBetweenShots(const float& value);
    const float GetIntervalBetweenShots();
    void SetTarget(Entity*& target);
    Entity* GetTarget();
    void SetProjectile(Entity*& value);
    Entity* GetProjectile();
    void SetScene(Scene*& value);
    Scene* GetScene();
    const bool IsUpdateComplete();
    const std::string GetName();
    void SetName(const std::string& name);
    const unsigned long long GetTypeID();
    const bool RunCommand(ParameterContainer& input, ParameterContainer& output);
    void Activate(const bool& option = true);
    const bool IsActive();
    ICloneable* Clone();
    const std::string GetType();
    void Dispose();
    void Update(const FrameTime& frameTime);
};



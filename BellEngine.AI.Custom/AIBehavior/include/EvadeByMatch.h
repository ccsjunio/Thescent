#pragma once
#include <Entity.h>
#include <Scene.h>
#include <IAIBehavior.h>
#include <BellEngineID.h>
#include <IRigidbody.h>

class EvadeByMatch : public IAIBehavior
{
private:
    bool isActive, isUpdateComplete;
    std::string name;
    std::function<void(const IAIBehavior::AIEvent_s&)> aiEvent;
    Scene* scene;
    std::string targetNameMatch;
    float slowingDistance;
    float minDistance;
    float maxDistance;
    float topSpeed;

    void ExecuteEvasion(Entity*& parent);
    Entity* FindClosestEntityByNameMatch(IRigidbody*& parentRB);

public:
    EvadeByMatch() :
        isUpdateComplete(false),
        isActive(true),
        scene(nullptr),
        minDistance(0.0f),
        maxDistance(0.0f),
        topSpeed(0.0f),
        slowingDistance(0.0f) {};
    ~EvadeByMatch();

    void BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn);
    void SetNameMatch(const std::string& targetNameMatch);
    const std::string GetNameMatch();
    void SetMaxDistance(const float& value);
    const float GetMaxDistance();
    void SetMinDistance(const float& value);
    const float GetMinDistance();
    void SetSlowingDistance(const float& value);
    const float GetSlowingdistance();
    void SetTopSpeed(const float& value);
    const float GetTopSpeed();
    void SetScene(Scene* scene);
    Scene* GetScene();
    const bool IsUpdateComplete();
    const std::string GetName();
    void SetName(const std::string& name);
    const unsigned long long GetTypeID();
    const bool RunCommand(ParameterContainer& input, ParameterContainer& output);
    void Activate(const bool& option = true);
    const bool IsActive();
    const std::string GetType();
    ICloneable* Clone();
    void Dispose();
    void Update(const FrameTime& deltaTime);
};


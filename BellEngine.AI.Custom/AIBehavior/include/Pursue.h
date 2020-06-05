#pragma once
#include <IAIBehavior.h>
#include <Entity.h>

class Pursue : public IAIBehavior 
{
private:
    bool axisLock[3];
    bool isActive, isUpdateComplete;
    std::string name;
    std::function<void(const IAIBehavior::AIEvent_s&)> aiEvent;
    float slowingDistance;
    float minDistance;
    float maxDistance;
    float topSpeed;
    Entity* target;

    void ExecutePursue(Entity*& parent);

public:
    Pursue() :
        axisLock{false, false, false},
        isUpdateComplete(false),
        isActive(true),
        target(nullptr),
        minDistance(0.0f),
        maxDistance(0.0f),
        topSpeed(0.0f),
        slowingDistance(0.0f) {};
    ~Pursue();

    void SetAxisLock(const bool& x, const bool& y, const bool& z);
    const void GetAxisLock(bool& x, bool& y, bool& z);
    void SetMaxDistance(const float& value);
    const float GetMaxDistance();
    void SetMinDistance(const float& value);
    const float GetMinDistance();
    void SetSlowingDistance(const float& value);
    const float GetSlowingdistance();
    void SetTopSpeed(const float& value);
    const float GetTopSpeed();
    void BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn);
    void SetTarget(Entity* target);
    Entity* GetTarget();
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


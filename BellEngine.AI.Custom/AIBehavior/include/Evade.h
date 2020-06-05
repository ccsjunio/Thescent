#pragma once
#include <IAIBehavior.h>

class Evade : public IAIBehavior
{
private:
    bool isActive, isUpdateComplete;
    std::string name;
    std::function<void(const IAIBehavior::AIEvent_s&)> aiEvent;

public:
    Entity* target;
    float slowingDistance;
    float minDistance;
    float maxDistance;
    float topSpeed;

    Evade() :
        isUpdateComplete(false),
        isActive(true),
        target(nullptr),
        minDistance(0.0f),
        maxDistance(0.0f),
        topSpeed(0.0f),
        slowingDistance(0.0f) {};
    ~Evade();

    void BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn);
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


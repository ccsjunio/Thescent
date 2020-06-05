#pragma once
#include <IAIBehavior.h>
#include <Entity.h>
#include <GLMCommon.h>

class Wander : public IAIBehavior
{
private:
    bool isActive, isUpdateComplete;
    std::string name;
    std::function<void(const IAIBehavior::AIEvent_s&)> aiEvent;

public:
    Vector3D targetPosition;
    float distanceToCircle;
    float circleRadius;
    float topSpeed;

    Wander() :
        isUpdateComplete(false),
        isActive(true),
        distanceToCircle(0.0f),
        circleRadius(0.0f),
        topSpeed(0.0f),      
        targetPosition(Vector3D()) {};
    ~Wander();

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


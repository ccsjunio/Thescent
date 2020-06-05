#pragma once
#include <IAIBehavior.h>

class AxisRotation : public IAIBehavior
{
private:
    bool isActive, isUpdateComplete;
    std::string name;
    float velocityGain;
    std::function<void(const IAIBehavior::AIEvent_s&)> aiEvent;

public:
    AxisRotation() :
        isUpdateComplete(false),
        isActive(true),
        velocityGain(1.0f) {};
    ~AxisRotation();

    void BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn);
    void SetVelocityGain(const float& velocityGain);
    const bool IsUpdateComplete();
    const std::string GetName();
    void SetName(const std::string& name);
    const unsigned long long GetTypeID();
    void Activate(const bool& option = true);
    const bool IsActive();
    const std::string GetType();
    ICloneable* Clone();
    const bool RunCommand(ParameterContainer& input, ParameterContainer& output);
    void Dispose();
    void Update(const FrameTime& deltaTime);
};


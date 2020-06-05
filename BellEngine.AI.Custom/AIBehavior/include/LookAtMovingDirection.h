#pragma once
#include <IAIBehavior.h>
#include <Entity.h>
#include <MathLibrary.h>

class LookAtMovingDirection : public IAIBehavior
{
private:
    bool isActive, isUpdateComplete;
    std::string name;
    std::function<void(const IAIBehavior::AIEvent_s&)> aiEvent;

    const Quaternion SafeQuatLookAt(const Vector3D& lookFrom, const Vector3D& lookTo, const Vector3D& up, const Vector3D& alternativeUp);
    void LookAt(Entity*& parent);

public:
    LookAtMovingDirection() :
        isUpdateComplete(false),
        isActive(true) {};
    ~LookAtMovingDirection();

    void BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn);
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
    void Update(const FrameTime& deltaTime);
};


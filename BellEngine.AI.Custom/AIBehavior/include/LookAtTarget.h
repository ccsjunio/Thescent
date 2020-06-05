#pragma once
#include <IAIBehavior.h>
#include <Entity.h>
#include <MathLibrary.h>

class LookAtTarget : public IAIBehavior
{
private:
    bool axisLock[3];
    bool isActive, isUpdateComplete;
    std::string name;
    float radius, rotationalGain;
    Entity* target;
    Vector3D targetPosition, defaultTargetPosition;
    std::function<void(const IAIBehavior::AIEvent_s&)> aiEvent;

    void LookAt_Vector3D(Entity*& parent);
    void LookAt_Entity(Entity*& parent);
    void LookAt(Entity*& parent, const Vector3D& position);

public:
    LookAtTarget() :
        axisLock{ false, false, false },
        rotationalGain(1.0f),
        radius(0.0f),
        isUpdateComplete(false),
        isActive(true),
        target(nullptr),
        targetPosition(Vector3D()) {};
    ~LookAtTarget();

    void SetAxisLock(const bool& x, const bool& y, const bool& z);
    const void GetAxisLock(bool& x, bool& y, bool& z);
    void BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn);
    void SetRotationalGain(const float& value);
    const float GetRotationalGain();
    void SetRadius(const float& radius);
    const float GetRadius();
    void SetTarget(Entity* target);
    Entity* GetTarget();
    void SetTargetPosition(const Vector3D& target);
    const Vector3D GetTargetPosition();
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


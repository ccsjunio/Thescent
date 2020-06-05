#include "../include/LookAtTarget.h"
#include <IRigidbody.h>
#include <Easing.h>
#include <BellEngineID.h>

void LookAtTarget::LookAt_Vector3D(Entity*& parent)
{
    LookAt(parent, targetPosition);
}
void LookAtTarget::LookAt_Entity(Entity*& parent)
{
    LookAt(parent, target->GetComponent<IRigidbody*>()->GetPosition());
}
void LookAtTarget::LookAt(Entity*& parent, const Vector3D& position)
{
    Quaternion initialRotation = parent->GetComponent<IRigidbody*>()->GetRotation();

    Vector3D normalizedDistance = glm::normalize(position - parent->GetComponent<IRigidbody*>()->GetPosition());
    Quaternion rotationDirection = glm::quatLookAt(normalizedDistance, parent->GetComponent<IRigidbody*>()->GetUp());
    Quaternion slerp = glm::slerp(initialRotation, rotationDirection, Easing::GetEasingValue((float)this->frameTime.GetDeltaTime() * rotationalGain, (Easing::Easing_e::None)));

    //Rotate	
    if (this->axisLock[0] || this->axisLock[1] || this->axisLock[2])
    {
        Vector3D euler = MathHelper::QuaternionToEulerAngles(slerp);
        if (this->axisLock[0])euler.x = 0.0f;
        if (this->axisLock[1])euler.y = 0.0f;
        if (this->axisLock[2])euler.z = 0.0f;

        parent->GetComponent<IRigidbody*>()->SetRotation(euler);
    }
    else
    {
        parent->GetComponent<IRigidbody*>()->SetRotation(slerp);
    }
}

LookAtTarget::~LookAtTarget()
{
    Dispose();
}

/*Locks the movement vector. Eg.: 0 0 0 means no lock. 1 0 0 means that x will be ignored.*/
void LookAtTarget::SetAxisLock(const bool& x, const bool& y, const bool& z)
{
    this->axisLock[0] = x;
    this->axisLock[1] = y;
    this->axisLock[2] = z;
}

const void LookAtTarget::GetAxisLock(bool& x, bool& y, bool& z)
{
    x = axisLock[0];
    y = axisLock[1];
    z = axisLock[2];
}
void LookAtTarget::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
void LookAtTarget::SetRotationalGain(const float& value)
{
    this->rotationalGain = value;
}
const float LookAtTarget::GetRotationalGain()
{
    return this->rotationalGain;
}
void LookAtTarget::SetRadius(const float& radius)
{
    this->radius = radius;
}
const float LookAtTarget::GetRadius()
{
    return this->radius;
}
void LookAtTarget::SetTarget(Entity* target)
{
    this->target = target;
}
Entity* LookAtTarget::GetTarget()
{
    return target;
}
void LookAtTarget::SetTargetPosition(const Vector3D& target)
{
    this->targetPosition = target;
}
const Vector3D LookAtTarget::GetTargetPosition()
{
    return targetPosition;
}
const bool LookAtTarget::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string LookAtTarget::GetName()
{
    return this->name;
}
void LookAtTarget::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long LookAtTarget::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORLOOKATTARGET;
}
const bool LookAtTarget::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
void LookAtTarget::Activate(const bool& option)
{
    this->isActive = option;
}
const bool LookAtTarget::IsActive()
{
    return isActive;
}
ICloneable* LookAtTarget::Clone()
{
    LookAtTarget* clone = new LookAtTarget();
    clone->target = target;
    clone->name = name;
    clone->targetPosition = targetPosition;

    return clone;
}
const std::string LookAtTarget::GetType()
{
    return "lookattarget";
}
void LookAtTarget::Dispose()
{

}
void LookAtTarget::Update(const FrameTime& deltaTime)
{
    if (!this->isActive) return;
    this->isUpdateComplete = false;

    SetFrameTime(deltaTime);

    Entity* parent = GetParent<Entity*>();
    if (parent && !parent->IsDisposing())
    {
        /*This AI behavior can be used with a point in 3D space or an Entity.*/
        if (target)
        {
            if (!target->IsDisposing())
            {
                /*If the radius is 0.0f, then always look at the target, else, only look if the target is within range.*/
                if (radius != 0.0f)
                {
                    if (glm::distance(parent->GetComponent<IRigidbody*>()->GetPosition(), target->GetComponent<IRigidbody*>()->GetPosition()) < radius)
                    {
                        LookAt_Entity(parent);
                    }
                }
                else
                {
                    LookAt_Entity(parent);
                }
            }
            else
            {
                this->isActive = false;
            }          
        }
        else
        {
            /*If the radius is 0.0f, then always look at the target, else, only look if the target is within range.*/
            if (radius != 0.0f)
            {
                if (glm::distance(parent->GetComponent<IRigidbody*>()->GetPosition(), target->GetComponent<IRigidbody*>()->GetPosition()) < radius)
                {
                    LookAt_Vector3D(parent);
                }
            }
            else
            {
                LookAt_Vector3D(parent);
            }
        }
    }
    else
    {
        this->isActive = false;
    }      

    this->isUpdateComplete = true;
}
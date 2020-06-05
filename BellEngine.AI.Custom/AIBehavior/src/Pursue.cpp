#include "..\include\Pursue.h"
#include <IRigidbody.h>
#include <BellEngineID.h>

void Pursue::ExecutePursue(Entity*& parent)
{
    IRigidbody* targetRB = target->GetComponent<IRigidbody*>();
    IRigidbody* parentRB = parent->GetComponent<IRigidbody*>();
    Vector3D targetPosition = targetRB->GetPosition();
    Vector3D parentPosition = parentRB->GetPosition();

    float dist = glm::distance(targetRB->GetPosition(), parentRB->GetPosition());
    //If parent is already close to the target, or parent is too far, then halt
    if (dist <= minDistance || dist >= maxDistance)
    {
        //Clamp movement
        parentRB->ClampVelocity();
        return;
    }


    Vector3D distanceVector = targetPosition - parentPosition;
    float T = glm::length(distanceVector) / topSpeed;

    //Uses "prediction" to find out the target point the parent will pursue towards
    Vector3D targetavgVel = targetRB->GetVelocity();
    Vector3D futurePosition = targetPosition + targetavgVel * T;

    Vector3D desiredVelocity = futurePosition - parentPosition;
    Vector3D velocityVector = glm::normalize(desiredVelocity);

    velocityVector *= topSpeed;

    /*calculate the steering force */
    Vector3D parentAverageVelocity = parentRB->GetVelocity();
    Vector3D steer = velocityVector - parentAverageVelocity;

    /* add steering force to current velocity*/
    parentAverageVelocity += steer * (float)this->frameTime.GetDeltaTime();

    if (glm::length(parentAverageVelocity) > topSpeed)
    {
        parentAverageVelocity = glm::normalize(parentAverageVelocity) * topSpeed;
    }

    if (axisLock[0]) parentAverageVelocity.x = 0.0f;
    if (axisLock[1]) parentAverageVelocity.y = 0.0f;
    if (axisLock[2]) parentAverageVelocity.z = 0.0f;

    parentRB->SetVelocity(parentAverageVelocity);
}

Pursue::~Pursue()
{

}

/*Locks the movement vector. Eg.: 0 0 0 means no lock. 1 0 0 means that x will be ignored.*/
void Pursue::SetAxisLock(const bool& x, const bool& y, const bool& z)
{
    this->axisLock[0] = x;
    this->axisLock[1] = y;
    this->axisLock[2] = z;
}

const void Pursue::GetAxisLock(bool& x, bool& y, bool& z)
{
    x = axisLock[0];
    y = axisLock[1];
    z = axisLock[2];
}

void Pursue::SetMaxDistance(const float& value)
{
    this->maxDistance = value;
}
const float Pursue::GetMaxDistance()
{
    return this->maxDistance;
}
void Pursue::SetMinDistance(const float& value)
{
    this->minDistance = value;
}
const float Pursue::GetMinDistance()
{
    return this->minDistance;
}
void Pursue::SetSlowingDistance(const float& value)
{
    this->slowingDistance = value;
}
const float Pursue::GetSlowingdistance()
{
    return this->slowingDistance;
}
void Pursue::SetTopSpeed(const float& value)
{
    this->topSpeed = value;
}
const float Pursue::GetTopSpeed()
{
    return topSpeed;
}
void Pursue::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
void Pursue::SetTarget(Entity* target)
{
    this->target = target;
}
Entity* Pursue::GetTarget()
{
    return this->target;
}
const bool Pursue::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string Pursue::GetName()
{
    return this->name;
}
void Pursue::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long Pursue::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORPURSUE;
}
const bool Pursue::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
void Pursue::Activate(const bool& option)
{
    this->isActive = option;
}
const bool Pursue::IsActive()
{
    return isActive;
}
const std::string Pursue::GetType()
{
    return "pursue";
}
ICloneable* Pursue::Clone()
{
    Pursue* clone = new Pursue();
    clone->maxDistance = maxDistance;
    clone->target = target;
    clone->minDistance = minDistance;
    clone->name = name;
    clone->slowingDistance = slowingDistance;
    clone->topSpeed = topSpeed;

    return clone;
}
void Pursue::Dispose()
{

}
void Pursue::Update(const FrameTime& deltaTime)
{
    if (!this->isActive) return;
    this->isUpdateComplete = false;

    SetFrameTime(deltaTime);

    Entity* parent = GetParent<Entity*>();
    if (parent && !parent->IsDisposing())
    {
        ExecutePursue(parent);
    }
    else
    {
        this->isActive = false;
    }     

    this->isUpdateComplete = true;
}
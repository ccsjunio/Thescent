#include "..\include\Seek.h"
#include <BellEngineID.h>
#include <IRigidbody.h>

void Seek::ExecuteSeek(Entity*& parent)
{
    //Finds the desired seeking velocity
    Vector3D tpos = target->GetComponent<IRigidbody*>()->GetPosition();

    Vector3D ipos = parent->GetComponent<IRigidbody*>()->GetPosition();

    Vector3D desiredVelocity = tpos - ipos;
    Vector3D normalizedVelocity = glm::normalize(desiredVelocity);

    //Finds the distance to the target
    float dist = glm::distance(tpos, ipos);

    //If seeker is already close to the target, then halt
    if (dist <= minDistance || dist >= maxDistance)
    {
        //Clamp movement
        parent->GetComponent<IRigidbody*>()->ClampVelocity();
        return;
    }

    //Checks if the parent is close enought to the target
    if (dist < slowingDistance)
    {
        //Slows down movement according to distance
        desiredVelocity = desiredVelocity * topSpeed * (dist / slowingDistance);
    }
    else
    {
        //Velocity becomes the maximum allowed
        desiredVelocity *= topSpeed;
    }

    //Calculate the steering force
    Vector3D iavgVel = parent->GetComponent<IRigidbody*>()->GetVelocity();
    Vector3D steer = desiredVelocity - iavgVel;

    //Add steering force to current velocity
    iavgVel += steer * (float)this->frameTime.GetDeltaTime();

    if (glm::length(iavgVel) > topSpeed)
    {
        iavgVel = glm::normalize(iavgVel) * topSpeed;
    }

    parent->GetComponent<IRigidbody*>()->SetVelocity(iavgVel);
}

Seek::~Seek()
{
    Dispose();
}

void Seek::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
const bool Seek::IsUpdateComplete()
{
    return isUpdateComplete;
}
const std::string Seek::GetName()
{
    return this->name;
}
void Seek::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long Seek::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORFLEE;
}
const bool Seek::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
void Seek::Activate(const bool& option)
{
    this->isActive = option;
}
const bool Seek::IsActive()
{
    return isActive;
}
const std::string Seek::GetType()
{
    return "flee";
}
ICloneable* Seek::Clone()
{
    Seek* clone = new Seek();
    clone->maxDistance = maxDistance;
    clone->target = target;
    clone->minDistance = minDistance;
    clone->name = name;
    clone->slowingDistance = slowingDistance;
    clone->topSpeed = topSpeed;

    return clone;
}
void Seek::Dispose()
{

}
void Seek::Update(const FrameTime& deltaTime)
{
    this->isUpdateComplete = false;

    SetFrameTime(deltaTime);

    Entity* parent = GetParent<Entity*>();
    if (parent)
    {
        ExecuteSeek(parent);
    } 

    this->isUpdateComplete = true;
}

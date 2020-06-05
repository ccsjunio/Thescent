#include "..\include\Flee.h"
#include <Entity.h>
#include <BellEngineID.h>
#include <IRigidbody.h>

Flee::~Flee()
{
    Dispose();
}

void Flee::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
const bool Flee::IsUpdateComplete()
{
    return isUpdateComplete;
}
const std::string Flee::GetName()
{
    return this->name;
}
void Flee::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long Flee::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORFLEE;
}
const bool Flee::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
void Flee::Activate(const bool& option)
{
    this->isActive = option;
}
const bool Flee::IsActive()
{
    return isActive;
}
const std::string Flee::GetType()
{
    return "flee";
}
ICloneable* Flee::Clone()
{
    Flee* clone = new Flee();
    clone->maxDistance = maxDistance;
    clone->target = target;
    clone->minDistance = minDistance;
    clone->name = name;
    clone->slowingDistance = slowingDistance;
    clone->topSpeed = topSpeed;

    return clone;
}
void Flee::Dispose()
{

}
void Flee::Update(const FrameTime& deltaTime)
{
    this->isUpdateComplete = false;

    SetFrameTime(deltaTime);

    Entity* parent = GetParent<Entity*>();
    if (parent)
    {
       //Finds the desired fleeing velocity
        Vector3D tpos = target->GetComponent<IRigidbody*>()->GetPosition();

        Vector3D ipos = parent->GetComponent<IRigidbody*>()->GetPosition();

        Vector3D desiredVelocity = ipos - tpos;
        Vector3D normalizedVelocity = glm::normalize(desiredVelocity);

        //Finds the distance to the target
        float distance = glm::distance(tpos, ipos);

        //If seeker is already close to the target, then halt
        if (distance >= maxDistance || distance <= minDistance)
        {
            //Clamp movement
            parent->GetComponent<IRigidbody*>()->ClampVelocity();
            return;
        }

        //Checks if the parent is close enought to the target
        if (distance < slowingDistance)
        {
            //Slows down movement according to distance
            desiredVelocity = desiredVelocity * topSpeed * (distance / slowingDistance);
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

    this->isUpdateComplete = true;
}
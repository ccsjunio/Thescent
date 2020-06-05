#include "..\include\Evade.h"
#include <BellEngineID.h>
#include <IRigidbody.h>

Evade::~Evade()
{
    Dispose();
}

void Evade::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
const bool Evade::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string Evade::GetName()
{
    return this->name;
}
void Evade::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long Evade::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIOREVADE;
}
const bool Evade::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
void Evade::Activate(const bool& option)
{
    this->isActive = option;
}
const bool Evade::IsActive()
{
    return isActive;
}
const std::string Evade::GetType()
{
    return "evade";
}
ICloneable* Evade::Clone()
{
    Evade* clone = new Evade();
    clone->maxDistance = maxDistance;
    clone->target = target;
    clone->minDistance = minDistance;
    clone->name = name;
    clone->slowingDistance = slowingDistance;
    clone->topSpeed = topSpeed;

    return clone;
}
void Evade::Dispose()
{

}
void Evade::Update(const FrameTime& deltaTime)
{
    this->isUpdateComplete = false;

    SetFrameTime(deltaTime);

    Entity* parent = GetParent<Entity*>();
    if (parent)
    {
        Vector3D tpos = target->GetComponent<IRigidbody*>()->GetPosition();

        Vector3D ipos = parent->GetComponent<IRigidbody*>()->GetPosition();

        Vector3D distance = tpos - ipos;
        float T = glm::length(distance) / topSpeed;

        //Uses "prediction" to find out the target point the parent will evade
        Vector3D targetavgVel = target->GetComponent<IRigidbody*>()->GetVelocity();
        Vector3D futurePosition = tpos + targetavgVel * T;

        Vector3D desiredVelocity = ipos - futurePosition;
        Vector3D velocityVector = glm::normalize(desiredVelocity);

        velocityVector *= topSpeed;

        //Calculate the steering force
        Vector3D instavgVel = parent->GetComponent<IRigidbody*>()->GetVelocity();
        Vector3D steer = velocityVector - instavgVel;

        //Steering force to current velocity
        instavgVel += steer * (float)this->frameTime.GetDeltaTime();

        if (glm::length(instavgVel) > topSpeed)
        {
            instavgVel = glm::normalize(instavgVel) * topSpeed;
        }

        parent->GetComponent<IRigidbody*>()->SetVelocity(instavgVel);
    }

    this->isUpdateComplete = true;
}

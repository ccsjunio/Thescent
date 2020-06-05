#include "..\include\EvadeByMatch.h"

void EvadeByMatch::ExecuteEvasion(Entity*& parent)
{
    IRigidbody* parentRB = parent->GetComponent<IRigidbody*>();

    Entity* target = FindClosestEntityByNameMatch(parentRB);
    if (target)
    {
        IRigidbody* targetRB = target->GetComponent<IRigidbody*>();


        Vector3D targetPosition = targetRB->GetPosition();
        Vector3D parentPosition = parentRB->GetPosition();

        Vector3D distance = targetPosition - parentPosition;
        float T = glm::length(distance) / topSpeed;

        //Uses "prediction" to find out the target point the parent will evade
        Vector3D targetavgVel = targetRB->GetVelocity();
        Vector3D futurePosition = targetPosition + targetavgVel * T;

        Vector3D desiredVelocity = parentPosition - futurePosition;
        Vector3D velocityVector = glm::normalize(desiredVelocity);

        velocityVector *= topSpeed;

        //Calculate the steering force
        Vector3D parentAvgVelocity = parentRB->GetVelocity();
        Vector3D steer = velocityVector - parentAvgVelocity;

        //Steering force to current velocity
        parentAvgVelocity += steer * (float)this->frameTime.GetDeltaTime();

        if (glm::length(parentAvgVelocity) > topSpeed)
        {
            parentAvgVelocity = glm::normalize(parentAvgVelocity) * topSpeed;
        }

        parentRB->SetVelocity(parentAvgVelocity);
    }
}

Entity* EvadeByMatch::FindClosestEntityByNameMatch(IRigidbody*& parentRB)
{
    if (scene)
    {
        std::vector<Entity*> entities;
        if (scene->GetEntities(entities))
        {
            Entity* closestEntity = nullptr;
            float currentDistance(0.0f), closestDistance(FLT_MAX);

            for (Entity*& entity : entities)
            {
                /*Ignore disposing entities*/
                if (entity->IsDisposing()) continue;

                /*Find match*/
                if (entity->GetName().find(targetNameMatch) != std::string::npos)
                {
                    IRigidbody* targetRB = entity->GetComponent<IRigidbody*>();
                    if (targetRB)
                    {
                        if (!parentRB || !targetRB) return nullptr;

                        currentDistance = glm::distance(parentRB->GetPosition(), targetRB->GetPosition());
                        /*Do not care for entities outside the range*/
                        if (currentDistance <= minDistance /*|| currentDistance >= maxDistance*/)
                        {
                            if (currentDistance < closestDistance)
                            {
                                closestDistance = currentDistance;
                                closestEntity = entity;
                            }
                        }
                    }
                }
            }

            return closestEntity;
        }
    }

    return nullptr;
}

EvadeByMatch::~EvadeByMatch()
{
}

void EvadeByMatch::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
void EvadeByMatch::SetNameMatch(const std::string& targetNameMatch)
{
    this->targetNameMatch = targetNameMatch;
}
const std::string EvadeByMatch::GetNameMatch()
{
    return this->targetNameMatch;
}
void EvadeByMatch::SetMaxDistance(const float& value)
{
    this->maxDistance = value;
}
const float EvadeByMatch::GetMaxDistance()
{
    return this->maxDistance;
}
void EvadeByMatch::SetMinDistance(const float& value)
{
    this->minDistance = value;
}
const float EvadeByMatch::GetMinDistance()
{
    return this->minDistance;
}
void EvadeByMatch::SetSlowingDistance(const float& value)
{
    this->slowingDistance = value;
}
const float EvadeByMatch::GetSlowingdistance()
{
    return this->slowingDistance;
}
void EvadeByMatch::SetTopSpeed(const float& value)
{
    this->topSpeed = value;
}
const float EvadeByMatch::GetTopSpeed()
{
    return topSpeed;
}
void EvadeByMatch::SetScene(Scene* scene)
{
    this->scene = scene;
}
Scene* EvadeByMatch::GetScene()
{
    return this->scene;
}
const bool EvadeByMatch::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string EvadeByMatch::GetName()
{
    return this->name;
}
void EvadeByMatch::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long EvadeByMatch::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIOREVADEBYMATCH;
}
const bool EvadeByMatch::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
void EvadeByMatch::Activate(const bool& option)
{
    this->isActive = option;
}
const bool EvadeByMatch::IsActive()
{
    return isActive;
}
const std::string EvadeByMatch::GetType()
{
    return "evadebymatch";
}
ICloneable* EvadeByMatch::Clone()
{
    EvadeByMatch* clone = new EvadeByMatch();
    clone->maxDistance = maxDistance;
    clone->targetNameMatch = targetNameMatch;
    clone->minDistance = minDistance;
    clone->name = name;
    clone->scene = scene;
    clone->slowingDistance = slowingDistance;
    clone->topSpeed = topSpeed;

    return clone;
}
void EvadeByMatch::Dispose()
{

}
void EvadeByMatch::Update(const FrameTime& deltaTime)
{
    if (!this->isActive) return;
    this->isUpdateComplete = false;

    SetFrameTime(deltaTime);

    Entity* parent = GetParent<Entity*>();
    if (parent && !parent->IsDisposing())
    {
        ExecuteEvasion(parent);
    }
    else
    {
        this->isActive = false;
    }
    

    this->isUpdateComplete = true;
}
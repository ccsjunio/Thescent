#include "../include/LookAtMovingDirection.h"
#include <IRigidbody.h>
#include <MathLibrary.h>
#include <Easing.h>
#include <BellEngineID.h>

const Quaternion LookAtMovingDirection::SafeQuatLookAt(const Vector3D& lookFrom, const Vector3D& lookTo, const Vector3D& up, const Vector3D& alternativeUp)
{
    //https://stackoverflow.com/questions/18172388/glm-quaternion-lookat-function

    Vector3D direction = lookTo - lookFrom;
    float directionLength = glm::length(direction);

    // Check if the direction is valid; Also deals with NaN
    if (!(directionLength > 0.0001))
    {
        return Quaternion(1, 0, 0, 0); // Just return identity
    }

    // Normalize direction
    direction /= directionLength;

    // Is the normal up (nearly) parallel to direction?
    if (glm::abs(glm::dot(direction, up)) > .9999f)
    {
        // Use alternative up
        return glm::quatLookAt(direction, alternativeUp);
    }
    else
    {
        return glm::quatLookAt(direction, up);
    }
}
void LookAtMovingDirection::LookAt(Entity*& parent)
{
    Quaternion initialRotation = parent->GetComponent<IRigidbody*>()->GetRotation();

    Vector3D currVelocity = parent->GetComponent<IRigidbody*>()->GetVelocity();
    if (currVelocity != Vector3D())
    {
        Quaternion rotationDirection = SafeQuatLookAt(parent->GetComponent<IRigidbody*>()->GetForwardInWorld(),
            glm::normalize(currVelocity),
            parent->GetComponent<IRigidbody*>()->GetUp(),
            Vector3D(0.0f, 0.9f, 0.0f));

        /*  Vector3D normalizedDistance = glm::normalize(currVelocity);
          Quaternion rotationDirection = glm::quatLookAt(normalizedDistance, parent->GetComponent<IRigidbody*>()->GetUp());*/
        Quaternion slerp = glm::slerp(initialRotation, rotationDirection, Easing::GetEasingValue((float)this->frameTime.GetDeltaTime(), (Easing::Easing_e::None)));

        //Rotate	
        parent->GetComponent<IRigidbody*>()->SetRotation(slerp);
    }
}

LookAtMovingDirection::~LookAtMovingDirection()
{
    Dispose();
}

void LookAtMovingDirection::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
const bool LookAtMovingDirection::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string LookAtMovingDirection::GetName()
{
    return this->name;
}
void LookAtMovingDirection::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long LookAtMovingDirection::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORLOOKATMOVINGDIR;
}
const bool LookAtMovingDirection::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
void LookAtMovingDirection::Activate(const bool& option)
{
    this->isActive = option;
}
const bool LookAtMovingDirection::IsActive()
{
    return isActive;
}
ICloneable* LookAtMovingDirection::Clone()
{
    LookAtMovingDirection* clone = new LookAtMovingDirection();
    clone->SetName(GetName());

    return clone;
}
const std::string LookAtMovingDirection::GetType()
{
    return "lookatmovingdirection";
}
void LookAtMovingDirection::Dispose()
{

}
void LookAtMovingDirection::Update(const FrameTime& deltaTime)
{
    this->isUpdateComplete = false;

    SetFrameTime(deltaTime);

    Entity* parent = GetParent<Entity*>();
    if (parent)
    {
        LookAt(parent);
    }
    else
    {
        Activate(false);
    }

    this->isUpdateComplete = true;
}
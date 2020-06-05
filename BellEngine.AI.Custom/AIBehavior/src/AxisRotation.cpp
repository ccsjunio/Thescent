#include "../include/AxisRotation.h"
#include <IRigidbody.h>
#include <Easing.h>
#include <StringHelpers.h>
#include <BellEngineID.h>

AxisRotation::~AxisRotation()
{
    Dispose();
}

void AxisRotation::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
void AxisRotation::SetVelocityGain(const float& velocityGain)
{
    this->velocityGain = velocityGain;
}
const bool AxisRotation::IsUpdateComplete()
{
    return isUpdateComplete;
}
const std::string AxisRotation::GetName()
{
    return this->name;
}
void AxisRotation::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long AxisRotation::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORSEEK;
}
void AxisRotation::Activate(const bool& option)
{
    this->isActive = option;
}
const bool AxisRotation::IsActive()
{
    return isActive;
}
const std::string AxisRotation::GetType()
{
    return "axisrotation";
}
ICloneable* AxisRotation::Clone()
{
    AxisRotation* clone = new AxisRotation();
    clone->name = name;
    clone->velocityGain = velocityGain;

    return clone;
}
const bool AxisRotation::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
void AxisRotation::Dispose()
{

}
void AxisRotation::Update(const FrameTime& deltaTime)
{
    this->isUpdateComplete = false;

    SetFrameTime(deltaTime);

    Entity* parent = GetParent<Entity*>();
    if (parent)
    {
        IRigidbody* rb = parent->GetComponent<IRigidbody*>();
        if (rb)
        {
            Vector3D rot = rb->GetRotation_Euler();
            Vector3D newRot = Vector3D(0.0f, rot.y, 0.0f);
            if (newRot.y < 90.0)
            {
                newRot.y += velocityGain * Easing::GetEasingValue((float)this->frameTime.GetDeltaTime(), (Easing::Easing_e::None));
            }
            else
            {
                newRot.y += velocityGain * Easing::GetEasingValue((float)this->frameTime.GetDeltaTime(), (Easing::Easing_e::None));
            }

            rb->SetRotation(newRot);

            printf("%s\n", StringHelpers::Vec3ToStr(rb->GetRotation_Euler()).c_str());
        }
    }

    this->isUpdateComplete = true;
}
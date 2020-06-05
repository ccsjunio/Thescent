#include "..\include\Wander.h"
#include <IRigidbody.h>
#include <BellEngineID.h>

Wander::~Wander()
{
	Dispose();
}

void Wander::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
const bool Wander::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string Wander::GetName()
{
    return this->name;
}
void Wander::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long Wander::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORWANDER;
}
const bool Wander::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
void Wander::Activate(const bool& option)
{
    this->isActive = option;
}
const bool Wander::IsActive()
{
    return isActive;
}
const std::string Wander::GetType()
{
    return "wander";
}
ICloneable* Wander::Clone()
{
    Wander* clone = new Wander();
    clone->targetPosition = targetPosition;
    clone->name = name;
    clone->topSpeed = topSpeed;

    return clone;
}
void Wander::Dispose()
{

}
void Wander::Update(const FrameTime& deltaTime)
{
    this->isUpdateComplete = false;

    SetFrameTime(deltaTime);

    Entity* parent = GetParent<Entity*>();
    if (parent)
    {
        Vector3D ipos = parent->GetComponent<IRigidbody*>()->GetPosition();

        Vector3D eulerAngles = parent->GetComponent<IRigidbody*>()->GetRotation_Euler();

        float parentOrientation = glm::radians(eulerAngles.y);

        //Resolves the center of the circle based on the orientation of the parent
        Vector3D circlePoint = ipos + Vector3D(cos(-parentOrientation), 0, sin(-parentOrientation)) * distanceToCircle;
        std::vector<Vector3D> pointsInCircle;
        //Gets a set of points in a circle
        bool rc = MathHelper::GetPointsInFlatCircle(circleRadius, circlePoint, 0.5f, 100, pointsInCircle);
        //Picks a random point among the 100
        size_t pointIndex = rand() % pointsInCircle.size();

        //the target point the wandering vehicle will seek towards
        targetPosition = pointsInCircle[pointIndex];

        //Finds velocity needed
        Vector3D desiredVelocity = targetPosition - ipos;

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
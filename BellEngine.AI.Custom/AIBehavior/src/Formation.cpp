#include "../include/Formation.h"
#include <BellEngineID.h>
#include <IRigidbody.h>
#include <MathLibrary.h>
#include <Easing.h>

const Quaternion Formation::SafeQuatLookAt(const Vector3D& lookFrom, const Vector3D& lookTo, const Vector3D& up, const Vector3D& alternativeUp)
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
void Formation::LookAt(Entity* boid, const Vector3D& target, const double& deltaTime)
{
    Quaternion initialRotation = boid->GetComponent<IRigidbody*>()->GetRotation();

    if (target != Vector3D())
    {
        Quaternion rotationDirection = SafeQuatLookAt(boid->GetComponent<IRigidbody*>()->GetForwardInWorld(),
            glm::normalize(target),
            boid->GetComponent<IRigidbody*>()->GetUp(),
            Vector3D(0.0f, 0.9f, 0.0f));

        /*  Vector3D normalizedDistance = glm::normalize(currVelocity);
          Quaternion rotationDirection = glm::quatLookAt(normalizedDistance, parent->GetComponent<IRigidbody*>()->GetUp());*/
        Quaternion slerp = glm::slerp(initialRotation, rotationDirection, Easing::GetEasingValue((float)deltaTime, (Easing::Easing_e::None)));

        //Rotate	
        boid->GetComponent<IRigidbody*>()->SetRotation(slerp);
    }
}
void Formation::Seek(Entity* boid, const Vector3D& target, const float& slowingDistance, const float& topSpeed, const double& deltaTime)
{
    //Finds the desired seeking velocity
    Vector3D tpos = target;
    Vector3D ipos = boid->GetComponent<IRigidbody*>()->GetPosition();

    Vector3D desiredVelocity = tpos - ipos;
    Vector3D normalizedVelocity = glm::normalize(desiredVelocity);

    //Finds the distance to the target
    float dist = glm::distance(tpos, ipos);

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
    Vector3D iavgVel = boid->GetComponent<IRigidbody*>()->GetVelocity();
    Vector3D steer = desiredVelocity - iavgVel;

    //Add steering force to current velocity
    iavgVel += steer * (float)deltaTime;

    if (glm::length(iavgVel) > topSpeed)
    {
        iavgVel = glm::normalize(iavgVel) * topSpeed;
    }

    boid->GetComponent<IRigidbody*>()->SetVelocity(iavgVel);
}

Formation::~Formation()
{
    Dispose();
}

void Formation::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
const bool Formation::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string Formation::GetName()
{
    return this->name;
}
void Formation::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long Formation::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORFORMATION;
}
const bool Formation::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
void Formation::SetCoordinatorPosition(const Vector3D& position)
{
    coordinator.position = position;
}
const Vector3D Formation::GetCoordinatorPosition()
{
    return coordinator.position;
}
void Formation::SetCoordinatorTarget(const Vector3D& target)
{
    coordinator.target = target;
}
const Vector3D Formation::GetCoordinatorTarget()
{
    return coordinator.target;
}
void Formation::Activate(const bool& option)
{
    this->isActive = option;
}
const bool Formation::IsActive()
{
    return isActive;
}
bool Formation::AddBoid(Entity* boid)
{
    auto it = std::find(boids.begin(), boids.end(), boid);
    //auto it = std::find_if(boids.begin(), boids.end(), [&boid](Entity* go) { return go->GetName() == boid->GetName(); });

    if (it == boids.end())
    {
        boids.push_back(boid);
        return true;
    }

    return false;
}
bool Formation::RemoveBoid(Entity* boid)
{
    auto it = std::find(boids.begin(), boids.end(), boid);
    if (it != boids.end())
    {
        boids.erase(it);
        return true;
    }

    return false;
}
bool Formation::AddFormationNode(const Vector3D& node)
{
    coordinator.formationShape.push_back(node);
    return true;
}
bool Formation::RemoveFormationNode(const Vector3D& node)
{
    //auto it = std::find(path.begin(), path.end(), node);
    auto it = std::find_if(coordinator.formationShape.begin(), coordinator.formationShape.end(),
        [node](Vector3D _node)
        {
            /*Approximation compare for floating point errors*/
            return
                (node.x < _node.x + 0.001f && node.x > _node.x - 0.001f) &&
                (node.y < _node.y + 0.001f && node.y > _node.y - 0.001f) &&
                (node.z < _node.z + 0.001f && node.z > _node.z - 0.001f);
        });

    if (it != coordinator.formationShape.end())
    {
        coordinator.formationShape.erase(it);
        return true;
    }

    return false;
}
void Formation::SetTopSpeed(const float& value)
{
    this->topSpeed = value;
}
const float Formation::GetTopSpeed()
{
    return topSpeed;
}
ICloneable* Formation::Clone()
{
    Formation* clone = new Formation();
    /* clone->Activate(IsActive());
     clone->SetCurrentNode(GetCurrentNode());
     clone->path = path;*/

    return clone;
}
const std::string Formation::GetType()
{
    return "formation";
}
void Formation::Dispose()
{
    boids.clear();
    boids.shrink_to_fit();
}
void Formation::Update(const FrameTime& deltaTime)
{
    this->isUpdateComplete = false;

    SetFrameTime(deltaTime);

    coordinator.Update(this->frameTime.GetDeltaTime());
    for (size_t i = 0; i < boids.size(); i++)
    {
        Seek(boids[i], coordinator.GetOffset(i), 1.0f, 1.0f, this->frameTime.GetDeltaTime());
    }

    this->isUpdateComplete = true;
}
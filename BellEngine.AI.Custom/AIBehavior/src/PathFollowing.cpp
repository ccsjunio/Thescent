#include "../include/PathFollowing.h"
#include <MathLibrary.h>
#include <IRigidbody.h>
#include <Easing.h>
#include <BellEngineID.h>

void PathFollowing::LookAt(Entity*& parent, const Vector3D& position, const double& deltaTime)
{
    Quaternion initialRotation = parent->GetComponent<IRigidbody*>()->GetRotation();

    Vector3D normalizedDistance = glm::normalize(position - parent->GetComponent<IRigidbody*>()->GetPosition());
    Quaternion rotationDirection = glm::quatLookAt(normalizedDistance, parent->GetComponent<IRigidbody*>()->GetUp());
    Quaternion slerp = glm::slerp(initialRotation, rotationDirection, Easing::GetEasingValue((float)deltaTime, (Easing::Easing_e::None)));

    //Rotate	
    parent->GetComponent<IRigidbody*>()->SetRotation(slerp);
}

PathFollowing::~PathFollowing()
{
    Activate(false);
    SetCurrentNode(0);;
    path.clear();
    path.shrink_to_fit();
}

void PathFollowing::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
void PathFollowing::Loop(const bool& value)
{
    this->isLooping = value;
}
const bool PathFollowing::IsLooping()
{
    return this->isLooping;
}
const bool PathFollowing::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string PathFollowing::GetName()
{
    return this->name;
}
void PathFollowing::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long PathFollowing::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORPATHFOLLOWING;
}
const bool PathFollowing::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
void PathFollowing::SetNodeRadius(const float& value)
{
    nodeRadius = value;
}
const float PathFollowing::GetNodeRadius()
{
    return nodeRadius;
}
void PathFollowing::SetCurrentNode(unsigned int nodeID)
{
    currentNode = nodeID;
}
const unsigned int PathFollowing::GetCurrentNode()
{
    return currentNode;
}
void PathFollowing::Activate(const bool& option)
{
    this->isActive = option;
}
const bool PathFollowing::IsActive()
{
    return isActive;
}
bool PathFollowing::AddPathNode(const Vector3D& node)
{
    path.push_back(node);
    return true;
}
bool PathFollowing::RemovePathNode(const Vector3D& node)
{
    //auto it = std::find(path.begin(), path.end(), node);
    auto it = std::find_if(path.begin(), path.end(),
        [node](Vector3D _node)
        {
            /*Approximation compare for floating point errors*/
            return
                (node.x < _node.x + 0.001f && node.x > _node.x - 0.001f) &&
                (node.y < _node.y + 0.001f && node.y > _node.y - 0.001f) &&
                (node.z < _node.z + 0.001f && node.z > _node.z - 0.001f);
        });

    if (it != path.end())
    {
        path.erase(it);
        return true;
    }

    return false;
}
void PathFollowing::SetTopSpeed(const float& value)
{
    this->topSpeed = value;
}
const float PathFollowing::GetTopSpeed()
{
    return topSpeed;
}
void PathFollowing::Dispose()
{

}
ICloneable* PathFollowing::Clone()
{
    PathFollowing* clone = new PathFollowing();
    clone->Activate(IsActive());
    clone->SetCurrentNode(GetCurrentNode());
    clone->path = path;

    return clone;
}
const std::string PathFollowing::GetType()
{
    return "pathfollowing";
}
void PathFollowing::Update(const FrameTime& deltaTime)
{
    this->isUpdateComplete = false;

    SetFrameTime(deltaTime);

    Entity* parent = GetParent<Entity*>();
    if (parent)
    {
        float distanceToNode = glm::distance(parent->GetComponent<IRigidbody*>()->GetPosition(), path[currentNode]);

        if (distanceToNode < GetNodeRadius())
        {
            //If the parent has reached a node that is no the last, move to the next node  
            if (currentNode != path.size() - 1)
            {
                currentNode++;
            }
            //If the parent has reached the last node, then halt
            else
            {
                /*If is looping, restart the path from the start*/
                if (IsLooping())
                {
                    currentNode = 0;
                }
                /*Stop moving*/
                else
                {
                    parent->GetComponent<IRigidbody*>()->ClampVelocity();
                    isUpdateComplete = true;
                    return;
                }
            }
        }

        //Calculate stteerinf direction
       /* Vector3D steer = path[currentNode] - GetParent()->GetComponent<IRigidbody*>()->GetPosition();
        steer = glm::normalize(steer);*/

        //Updates the velocity of the parent
        Vector3D currentVelocity = parent->GetComponent<IRigidbody*>()->GetVelocity();
        Vector3D desiredVelocity = path[currentNode] - parent->GetComponent<IRigidbody*>()->GetPosition();
        Vector3D steer = desiredVelocity - currentVelocity;

        currentVelocity += steer * (float)this->frameTime.GetDeltaTime();

        if (glm::length(currentVelocity) > GetTopSpeed())
        {
            currentVelocity = glm::normalize(currentVelocity) * GetTopSpeed();
        }

        //parent->GetComponent<IRigidbody*>()->SetVelocity(iavgVel);

        parent->GetComponent<IRigidbody*>()->SetVelocity(currentVelocity);
    }

    this->isUpdateComplete = true;
}
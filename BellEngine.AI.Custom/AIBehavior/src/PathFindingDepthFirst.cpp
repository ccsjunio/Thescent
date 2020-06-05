#include "../include/PathFindingDepthFirst.h"
#include <IRigidbody.h>
#include <BellEngineID.h>

void PathFindingDepthFirst::Seek(Entity* boid, const Vector3D& target, const float& slowingDistance, const float& topSpeed, const double& deltaTime)
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

const bool PathFindingDepthFirst::FindPath(std::vector<Graph::Node*>& path)
{
    if (!rootNode->WasVisited())
    {
        rootNode->SetVisited(true);
        openStack.push(rootNode);
    }

    while (!openStack.empty())
    {
        Graph::Node* currNode = openStack.top();
        openStack.pop();

        if (currNode->HasGoal())
        {
            /*Stops search.*/

            while (currNode->GetParent())
            {
                path.push_back(currNode);
                currNode = currNode->GetParent();
            }
            std::reverse(path.begin(), path.end());

#ifdef DEBUG_PATH_FINDING
            //For debug
            for (auto n : path)
            {
                printf("DFS: XYZ=%s\n", StringHelpers::Vec3ToStr(n->GetPosition()).c_str());
            }
#endif

            return true;
        }

        std::vector<std::pair<Graph::Node*, float>> children;
        if (currNode->GetChildren(children))
        {
            for (std::pair<Graph::Node*, float> child : children)
            {
                if (!child.first->WasVisited())
                {
                    child.first->SetVisited(true);
                    child.first->SetParent(currNode);
                    openStack.push(child.first);
                }
            }
        }
    }

    return false;
}

PathFindingDepthFirst::~PathFindingDepthFirst()
{
	Dispose();
}

void PathFindingDepthFirst::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
void PathFindingDepthFirst::SetNodeRadius(const float& value)
{
    nodeRadius = value;
}
const float PathFindingDepthFirst::GetNodeRadius()
{
    return nodeRadius;
}
void PathFindingDepthFirst::SetTopSpeed(const float& value)
{
    this->topSpeed = value;
}
const float PathFindingDepthFirst::GetTopSpeed()
{
    return topSpeed;
}
const bool PathFindingDepthFirst::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string PathFindingDepthFirst::GetName()
{
    return this->name;
}
void PathFindingDepthFirst::SetName(const std::string& name)
{
    this->name = name;
}
void PathFindingDepthFirst::Activate(const bool& option)
{
    this->isActive = option;
}
const bool PathFindingDepthFirst::IsActive()
{
    return isActive;
}
const std::string PathFindingDepthFirst::GetType()
{
    return "depthfirstsearch";
}
const unsigned long long PathFindingDepthFirst::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORDFS;
}
const bool PathFindingDepthFirst::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
ICloneable* PathFindingDepthFirst::Clone()
{
    PathFindingDepthFirst* clone = new PathFindingDepthFirst();
    clone->SetName(GetName());
    clone->SetGraph(GetGraph());
    clone->SetRootNode(GetRootNode());
    clone->Activate(IsActive());

    return clone;
}
void PathFindingDepthFirst::SetGraph(Graph* graph)
{
    this->graph = graph;
}
void PathFindingDepthFirst::SetRootNode(Graph::Node* rootNode)
{
    this->rootNode = rootNode;
}
Graph* PathFindingDepthFirst::GetGraph()
{
    return graph;
}
Graph::Node* PathFindingDepthFirst::GetRootNode()
{
    return rootNode;
}
void PathFindingDepthFirst::Dispose()
{
    DisposePointer(graph);
}
void PathFindingDepthFirst::Update(const FrameTime& deltaTime)
{
    this->isUpdateComplete = false;
    if (isActive)
    {
        SetFrameTime(deltaTime);

        Entity* parent = GetParent<Entity*>();
        if (parent)
        {
            if (pathToFollow.empty())
            {
                FindPath(pathToFollow);
                this->isUpdateComplete = true;
                return;
            }

    #ifdef DEBUG_PATH_FINDING
            //For debug, jumps around instead of using seek
            if (timeacc < 1.0)
            {
                timeacc += frameTime.GetDeltaTime();
            }
            else
            {
                if (currentNode != pathToFollow.size() - 1)
                {
                    parent->GetComponent<IRigidbody*>()->SetPosition(pathToFollow[currentNode]->GetPosition());
                    currentNode++;
                }
                else
                {
                    Activate(false);
                }
                timeacc = 0.0;
            }
    #else
            float distanceToNode = glm::distance(parent->GetComponent<IRigidbody*>()->GetPosition(), pathToFollow[currentNode]->GetPosition());
            if (distanceToNode < GetNodeRadius())
            {
                //If the parent has reached a node that is no the last, move to the next node  
                if (currentNode != pathToFollow.size() - 1)
                {
                    currentNode++;
                    Seek(parent, pathToFollow[currentNode]->GetPosition(), 0.0f, GetTopSpeed(), this->frameTime.GetDeltaTime());
                }
                //If the parent has reached the last node, then halt
                else
                {
                    /*Halt parent*/
                    parent->GetComponent<IRigidbody*>()->ClampVelocity();
                    /*Turn off this AI*/
                    Activate(false);
                }
            }
            else
            {
                Seek(parent, pathToFollow[currentNode]->GetPosition(), 0.0f, GetTopSpeed(), this->frameTime.GetDeltaTime());
            }
    #endif
        }
    }
    this->isUpdateComplete = true;
}
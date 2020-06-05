#include "..//include/PathFindingDjikstra.h"
#include <BellEngineID.h>
#include <IRigidbody.h>

Graph::Node* PathFindingDjikstra::GetLowestCostChild(Graph::Node* parentNode)
{
    std::vector<std::pair<Graph::Node*, float>> children;
    if (parentNode->GetChildren(children))
    {
        Graph::Node* lowCostChild = nullptr;
        float lowestCost = FLT_MAX;
        for (auto child : children)
        {
            if (child.first->GetCost() < lowestCost)
            {
                lowCostChild = child.first;
                lowestCost = child.first->GetCost();
            }
        }

        return lowCostChild;
    }

    return nullptr;
}
Graph::Node* PathFindingDjikstra::GetLargestCostChild(Graph::Node* parentNode)
{
    std::vector<std::pair<Graph::Node*, float>> children;
    if (parentNode->GetChildren(children))
    {
        Graph::Node* currentChild = nullptr;
        float cost = 0.0f;
        for (auto child : children)
        {
            if (child.first->GetCost() > cost)
            {
                currentChild = child.first;
                cost = child.first->GetCost();
            }
        }

        return currentChild;
    }

    return nullptr;
}

void PathFindingDjikstra::Seek(Entity* boid, const Vector3D& target, const float& slowingDistance, const float& topSpeed, const double& deltaTime)
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
const bool PathFindingDjikstra::FindPath(std::vector<Graph::Node*>& path)
{
    openList.clear();
    closedList.clear();

    if (!rootNode->WasVisited())
    {
        rootNode->SetVisited(true);
        rootNode->SetCost(0.0f);
        openList.push_back(rootNode);
    }

    while (!openList.empty())
    {
        float minCost = FLT_MAX;
        size_t minIndex = 0;
        Graph::Node* currNode;
        //find node with the lowest cost from root node
        for (size_t i = 0; i < openList.size(); i++)
        {
            if (openList[i]->GetCost() < minCost)
            {
                minCost = openList[i]->GetCost();
                minIndex = i;
            }
        }

        /*Removes current node from the open list and add to the closed list*/
        currNode = openList[minIndex];
        auto olIT = std::find(openList.begin(), openList.end(), currNode);
        if (olIT != openList.end())
        {
            openList.erase(olIT);
        }
        closedList.push_back(currNode);

        currNode->SetVisited(true);
        if (currNode->HasGoal())
        {
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
                printf("DJIKSTRA: XYZ=%s C=%.2f\n",
                    StringHelpers::Vec3ToStr(n->GetPosition()).c_str(),
                    n->GetCost());
            }
#endif

            return true;
        }

        //Go through every child node node 
        std::vector<std::pair<Graph::Node*, float>> children;
        if (currNode->GetChildren(children))
        {
            float weightSoFar = 0.0f;
            for (auto child : children)
            {
                if (child.first->WasVisited() == false)
                {
                    weightSoFar = currNode->GetCost() + child.second;
                    if (weightSoFar < child.first->GetCost())
                    {
                        //update node when new better path is found
                        child.first->SetCost(weightSoFar);
                        child.first->SetParent(currNode);
                        if (std::find(openList.begin(), openList.end(), child.first) == openList.end())
                        {
                            openList.push_back(child.first); //add newly discovered node to open list
                        }
                    }
                }
            }
        }
    }

    return false;
}

PathFindingDjikstra::~PathFindingDjikstra()
{
    Dispose();
}

void PathFindingDjikstra::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
void PathFindingDjikstra::SetNodeRadius(const float& value)
{
    nodeRadius = value;
}
const float PathFindingDjikstra::GetNodeRadius()
{
    return nodeRadius;
}
void PathFindingDjikstra::SetTopSpeed(const float& value)
{
    this->topSpeed = value;
}
const float PathFindingDjikstra::GetTopSpeed()
{
    return topSpeed;
}
const bool PathFindingDjikstra::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string PathFindingDjikstra::GetName()
{
    return this->name;
}
void PathFindingDjikstra::SetName(const std::string& name)
{
    this->name = name;
}
void PathFindingDjikstra::Activate(const bool& option)
{
    this->isActive = option;
}
const bool PathFindingDjikstra::IsActive()
{
    return isActive;
}
const std::string PathFindingDjikstra::GetType()
{
    return "djikstrasearch";
}
const unsigned long long PathFindingDjikstra::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORDJIKSTRA;
}
const bool PathFindingDjikstra::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
ICloneable* PathFindingDjikstra::Clone()
{
    PathFindingDjikstra* clone = new PathFindingDjikstra();
    clone->SetName(GetName());
    clone->SetGraph(GetGraph());
    clone->SetRootNode(GetRootNode());
    clone->Activate(IsActive());

    return clone;
}
void PathFindingDjikstra::SetGraph(Graph* graph)
{
    this->graph = graph;
}
void PathFindingDjikstra::SetRootNode(Graph::Node* rootNode)
{
    this->rootNode = rootNode;
}
Graph* PathFindingDjikstra::GetGraph()
{
    return graph;
}
Graph::Node* PathFindingDjikstra::GetRootNode()
{
    return rootNode;
}
void PathFindingDjikstra::Dispose()
{
    DisposePointer(graph);
}
void PathFindingDjikstra::Update(const FrameTime& deltaTime)
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
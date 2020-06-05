#include "../include/PathFindingAStar.h"
#include <BellEngineID.h>
#include <IRigidbody.h>

const float PathFindingAStar::CalculateHeuristics(Graph::Node* node, Graph::Node* goal)
{
    float D = 1.0f;
    float dx = abs(goal->GetPosition().x - node->GetPosition().x);
    float dy = abs(goal->GetPosition().y - node->GetPosition().y);
    float dz = abs(goal->GetPosition().z - node->GetPosition().z);
    return D * (dx + dy + dz);
}
Graph::Node* PathFindingAStar::FindNextGoalNode()
{
    Graph::Node* ptr = nullptr;

    std::vector<Graph::Node*> nodes;
    if (graph->GetNodes(nodes))
    {
        for (Graph::Node* node : nodes)
        {
            if (node->HasGoal())
            {
                ptr = node;
                break;
            }
        }
    }

    return ptr;
}
Graph::Node* PathFindingAStar::GetClosestChild(Graph::Node* parentNode)
{
    std::vector<std::pair<Graph::Node*, float>> children;
    if (parentNode->GetChildren(children))
    {
        Graph::Node* _child = nullptr;
        float _closestDistance = FLT_MAX;
        for (auto child : children)
        {
            if (child.first->GetCost() + child.first->GetHeuristicsDistance() < _closestDistance)
            {
                _child = child.first;
                _closestDistance = child.first->GetCost() + child.first->GetHeuristicsDistance();
            }
        }

        return _child;
    }

    return nullptr;
}
Graph::Node* PathFindingAStar::GetLowestCostChild(Graph::Node* parentNode)
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
Graph::Node* PathFindingAStar::GetLargestCostChild(Graph::Node* parentNode)
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

void PathFindingAStar::Seek(Entity* boid, const Vector3D& target, const float& slowingDistance, const float& topSpeed, const double& deltaTime)
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
const bool PathFindingAStar::FindPath(std::vector<Graph::Node*>& path)
{
    openList.clear();
    closedList.clear();

    if (!rootNode->WasVisited())
    {
        //rootNode->SetVisited(true);
        rootNode->SetCost(0.0f);
        rootNode->SetHeuristicsDistance(CalculateHeuristics(rootNode, targetNode));
        openList.push_back(rootNode);
    }

    while (!openList.empty())
    {
        float minCost = FLT_MAX;
        size_t minIndex = 0;
        Graph::Node* currNode = nullptr;

        /*Find node with the lowest cost and distance from targetnode*/
        for (size_t i = 0; i < openList.size(); i++)
        {
            Graph::Node* node = openList[i];
            if (node->GetCost() + node->GetHeuristicsDistance() < minCost)
            {
                minCost = node->GetCost() + node->GetHeuristicsDistance();
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
                printf("ASTAR: XYZ=%s H=%.2f C=%.2f\n",
                    StringHelpers::Vec3ToStr(n->GetPosition()).c_str(),
                    n->GetHeuristicsDistance(),
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
            for (std::pair<Graph::Node*, float> child : children)
            {
                if (!child.first->WasVisited())
                {
                    weightSoFar = currNode->GetCost() + child.second;
                    if (weightSoFar < child.first->GetCost())
                    {
                        child.first->SetCost(weightSoFar);
                        child.first->SetParent(currNode);
                        if (std::find(openList.begin(), openList.end(), child.first) == openList.end())
                        {
                            child.first->SetHeuristicsDistance(CalculateHeuristics(child.first, targetNode));
                            openList.push_back(child.first);
                        }
                    }
                }
            }
        }
    }

    return false;
}

PathFindingAStar::~PathFindingAStar()
{
    Dispose();
}

void PathFindingAStar::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
void PathFindingAStar::SetNodeRadius(const float& value)
{
    nodeRadius = value;
}
const float PathFindingAStar::GetNodeRadius()
{
    return nodeRadius;
}
void PathFindingAStar::SetTopSpeed(const float& value)
{
    this->topSpeed = value;
}
const float PathFindingAStar::GetTopSpeed()
{
    return topSpeed;
}
const bool PathFindingAStar::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string PathFindingAStar::GetName()
{
    return this->name;
}
void PathFindingAStar::SetName(const std::string& name)
{
    this->name = name;
}
void PathFindingAStar::Activate(const bool& option)
{
    this->isActive = option;
}
const bool PathFindingAStar::IsActive()
{
    return isActive;
}
const std::string PathFindingAStar::GetType()
{
    return "astarsearch";
}
const unsigned long long PathFindingAStar::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORASTAR;
}
const bool PathFindingAStar::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
ICloneable* PathFindingAStar::Clone()
{
    PathFindingAStar* clone = new PathFindingAStar();
    clone->SetName(GetName());
    clone->SetGraph(GetGraph());
    clone->SetRootNode(GetRootNode());
    clone->Activate(IsActive());

    return clone;
}
void PathFindingAStar::SetGraph(Graph* graph)
{
    this->graph = graph;
}
void PathFindingAStar::SetRootNode(Graph::Node* rootNode)
{
    this->rootNode = rootNode;
}
Graph* PathFindingAStar::GetGraph()
{
    return graph;
}
Graph::Node* PathFindingAStar::GetRootNode()
{
    return rootNode;
}
void PathFindingAStar::SetTargetNode(Graph::Node* node)
{
    this->targetNode = node;
}
Graph::Node* PathFindingAStar::GetTargetNode()
{
    return targetNode;
}
void PathFindingAStar::Dispose()
{
    DisposePointer(graph);
}

void PathFindingAStar::Update(const FrameTime& deltaTime)
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
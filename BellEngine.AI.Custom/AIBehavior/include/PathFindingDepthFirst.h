#pragma once
#include "../include//IPathFinding.h"
#include <stack>

class PathFindingDepthFirst : public IPathFinding
{
private:
    Graph* graph;
    Graph::Node* rootNode;
    bool isActive, isUpdateComplete, goalFound;
    std::stack<Graph::Node*> openStack;
    std::vector<Graph::Node*> pathToFollow;
    unsigned int currentNode;
    std::string name;
    float topSpeed, nodeRadius;
    std::function<void(const IAIBehavior::AIEvent_s&)> aiEvent;
#ifdef DEBUG_PATH_FINDING
    double timeacc = 0.0;
#endif

    void Seek(Entity* boid, const Vector3D& target, const float& slowingDistance, const float& topSpeed, const double& deltaTime);
    const bool FindPath(std::vector<Graph::Node*>& path);

public:
    PathFindingDepthFirst()
    {
        isUpdateComplete = false;
        goalFound = false;
        currentNode = 0u;
        isActive = true;
        graph = nullptr;
        rootNode = nullptr;
        topSpeed = 20.0f;
        nodeRadius = 2.0f / 3.0f;
    }
    ~PathFindingDepthFirst();

    void BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn);
    void SetNodeRadius(const float& value);
    const float GetNodeRadius();
    void SetTopSpeed(const float& value);
    const float GetTopSpeed();
    const bool IsUpdateComplete();
    const std::string GetName();
    void SetName(const std::string& name);
    void Activate(const bool& option = true);
    const bool IsActive();
    const std::string GetType();
    const unsigned long long GetTypeID();
    const bool RunCommand(ParameterContainer& input, ParameterContainer& output);
    ICloneable* Clone();
    void SetGraph(Graph* graph);
    void SetRootNode(Graph::Node* rootNode);
    Graph* GetGraph();
    Graph::Node* GetRootNode();
    void Dispose();
    void Update(const FrameTime& deltaTime);
};


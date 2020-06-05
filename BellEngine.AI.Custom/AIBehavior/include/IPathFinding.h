#pragma once
#include <Entity.h>
#include <IAIBehavior.h>
#include "../include/Graph.h"

//#define DEBUG_PATH_FINDING

class IPathFinding : public IAIBehavior
{
protected:
    IPathFinding() {};
    virtual const bool FindPath(std::vector<Graph::Node*>& path) = 0;

public:
    virtual ~IPathFinding() {};

    virtual void SetNodeRadius(const float& value) = 0;
    virtual const float GetNodeRadius() = 0;
    virtual void SetTopSpeed(const float& value) = 0;
    virtual const float GetTopSpeed() = 0;
    virtual void SetGraph(Graph* graph) = 0;
    virtual void SetRootNode(Graph::Node* rootNode) = 0;
    virtual Graph* GetGraph() = 0;
    virtual Graph::Node* GetRootNode() = 0;
};

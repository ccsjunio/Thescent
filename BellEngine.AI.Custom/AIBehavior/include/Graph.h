#pragma once
#include <MathLibrary.h>
#include <vector>
#include <IDisposable.h>

class Graph : public IDisposable
{
public:
	class Node : public IDisposable
	{
    private:
        unsigned long id;
        bool visited, hasGoal;
        float currentCost, heuristicsDistance;
        Vector3D position;
        Graph::Node* parent; //parent Node that we can follow back to the root node.
        std::vector<std::pair<Graph::Node*, float>> children; //Edges pointing to neighbouring graph <childNode, edgeWeight>

    public:
        Node()
        {
            id = 0ul;
            visited = hasGoal = false;
            currentCost = heuristicsDistance = FLT_MAX;
            position = Vector3D();
            parent = nullptr;
        }

        void SetHeuristicsDistance(const float& distance)
        {
            this->heuristicsDistance = distance;
        }
        const float GetHeuristicsDistance()
        {
            return this->heuristicsDistance;
        }
        void SetCost(const float& cost)
        {
            this->currentCost = cost;
        }
        const float GetCost()
        {
            return currentCost;
        }
        void SetID(const unsigned long& id)
        {
            this->id = id;
        }
        const unsigned long GetID()
        {
            return id;
        }
        void SetVisited(const bool& value)
        {
            this->visited = value;
        }
        const bool WasVisited()
        {
            return this->visited;
        }
        void SetIfHasGoal(const bool& value)
        {
            this->hasGoal = value;
        }
        const bool HasGoal()
        {
            return this->hasGoal;
        }
        void SetParent(Graph::Node* parent)
        {
            this->parent = parent;
        }
        Graph::Node* GetParent()
        {
            return this->parent;
        }
        void SetPosition(const Vector3D& value)
        {
            this->position = value;
        }
        const Vector3D GetPosition()
        {
            return this->position;
        }
        const bool AddChild(Graph::Node* child, const float& weight)
        {
            auto it = std::find_if(children.begin(), children.end(), [child, weight](std::pair<Node*, float> pair) { return pair.first == child && pair.second == weight; });
            if (it == children.end())
            {
                //Take a load off Fanny, take a load for free, take a load off Fanny, and (and) put the load right on me...
                children.push_back(std::pair<Node*, float>(child, /*The Weight*/ weight));
                return true;
            }

            return false;
        }
        const bool RemoveChild(Graph::Node* child)
        {
            auto it = std::find_if(children.begin(), children.end(), [child](std::pair<Node*, float> pair) { return pair.first == child; });
            if (it != children.end())
            {
                children.erase(it);
                return true;
            }

            return false;
        }
        const bool GetChildren(std::vector<std::pair<Graph::Node*, float>>& children)
        {
            children = this->children;
            return !children.empty();
        }
        void Dispose()
        {
            DisposeVector(children);
        }
	};

    Graph();
    ~Graph();

    Graph::Node* FindNodeByID(const unsigned int& id);
    const bool AddNode(Graph::Node* node);
    const bool RemoveChild(Graph::Node* node);
    const bool GetNodes(std::vector<Node*> nodes)
    {
        nodes = this->nodes;
        return !nodes.empty();
    }
    void AddEdge(Graph::Node* parent, Graph::Node* child, const float& weight);
    void ResetGraph();
    /*void PrintGraph();
    void PrintParents(bool includeCost = false);*/
    void Dispose();

private:
    std::vector<Node*> nodes;
};


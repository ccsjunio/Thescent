#include "../include/Graph.h"

Graph::Graph()
{
}

Graph::~Graph()
{
	Dispose();
}

Graph::Node* Graph::FindNodeByID(const unsigned int& id)
{
	auto it = std::find_if(nodes.begin(), nodes.end(), [id](Graph::Node* node) { return node->GetID() == id; });
	if (it != nodes.end())
	{
		return *it;
	}

	return nullptr;
}

const bool Graph::AddNode(Graph::Node* node)
{
	auto it = std::find(nodes.begin(), nodes.end(), node);
	if (it == nodes.end())
	{
		//Take a load off Fanny, take a load for free, take a load off Fanny, and (and) put the load right on me...
		nodes.push_back(node);
		return true;
	}

	return false;
}

const bool Graph::RemoveChild(Graph::Node* node)
{
	auto it = std::find(nodes.begin(), nodes.end(), node);
	if (it != nodes.end())
	{
		nodes.erase(it);
		DisposePointer(node);
		return true;
	}

	return false;
}

void Graph::AddEdge(Graph::Node* parent, Graph::Node* child, const float& weight)
{
	parent->AddChild(child, weight);
}

void Graph::ResetGraph()
{
	for (auto node : this->nodes)
	{
		node->SetVisited(false);
		node->SetParent(nullptr);
		node->SetCost(FLT_MAX);
		node->SetHeuristicsDistance(0.0f);
	}
}

void Graph::Dispose()
{
	DisposeVector(nodes);
}

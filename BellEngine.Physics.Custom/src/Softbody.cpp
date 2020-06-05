#include "Softbody.h"
#include <BellEngineID.h>

/*Constructor.*/
Softbody::Softbody()
{
	allowCollisionDetection = true;
	parent = nullptr;
	isActive = true;
	mass = 0.0f;
	transform = new Transform();
}
/*Destructor.*/
Softbody::~Softbody()
{	
	Dispose();
}
const bool Softbody::IsUpdateComplete()
{
	return this->isUpdateComplete;
}
const std::string Softbody::GetName()
{
	return this->name;
}
void Softbody::SetName(const std::string& name)
{
	this->name = name;
}
/*Controls wheter this object should be tested for collisions or not.*/
void Softbody::ToggleCollisionDetection(const bool& option)
{
	this->allowCollisionDetection = option;
}
/*Returns wheter this object should be tested for collisions or not.*/
const bool Softbody::AllowCollisionDetection()
{
	return this->allowCollisionDetection;
}
void Softbody::SetParent(void* parent)
{
	this->parent = parent;
}
/*Similar to 'GetUserPointer', returns the parent of the softbody.*/
void* Softbody::GetParent()
{
	return this->parent;
}
/*Searches for a node with a given ID.*/
Softbody::Node* Softbody::FindNodeByID(const unsigned int& id)
{
	Softbody::Node* ptr = nullptr;

	auto it = std::find_if(nodes.begin(), nodes.end(), [id](Softbody::Node* n) 
		{
			return n->GetID() == id;
		});

	if (it != nodes.end())
	{
		ptr = (*it);
	}

	return ptr;
}
/*Adds a new connection between 2 notes.*/
const bool Softbody::AddConnection(const unsigned int& c0, const unsigned int& c1)
{
	auto it = std::find_if(connections.begin(), connections.end(), [c0, c1](std::pair<unsigned int, unsigned int> p) { return p.first == c0 && p.second == c1; });

	if (it == connections.end())
	{
		connections.push_back(std::make_pair(c0, c1));
		return true;
	}

	return false;
}
/*Removes a new connection between 2 notes.*/
const bool Softbody::RemoveConnection(const unsigned int& c0, const unsigned int& c1)
{
	auto it = std::find_if(connections.begin(), connections.end(), [c0, c1](std::pair<unsigned int, unsigned int> p) { return p.first == c0 && p.second == c1; });

	if (it != connections.end())
	{
		connections.erase(it);
		return true;
	}

	return false;
}
/*Activates/Deactivates the softbody.*/
void Softbody::Activate(const bool& value)
{
	isActive = value;
}
/*Checks if the softbody is active or awake.*/
const bool Softbody::IsActive()
{
	return isActive;
}
/*Adds a new node to the collection of nodes.*/
const bool Softbody::AddNode(Softbody::Node* node)
{
	auto it = std::find(nodes.begin(), nodes.end(), node);

	if (it == nodes.end())
	{
		nodes.push_back(node);
		return true;
	}

	return false;
}
/*Removes a node from the collection of nodes.*/
const bool Softbody::RemoveNode(Softbody::Node* node)
{
	auto it = std::find(nodes.begin(), nodes.end(), node);

	if (it != nodes.end())
	{
		nodes.erase(it);
		return true;
	}

	return false;
}
/*Adds a new spring to the colledtion of springs.*/
const bool Softbody::AddSpring(Softbody::Spring* spring)
{
	auto it = std::find(springs.begin(), springs.end(), spring);
	
	if (it == springs.end())
	{
		springs.push_back(spring);
		return true;
	}
	
	return false;
}
/*Removes a new spring to the colledtion of springs.*/
const bool Softbody::RemoveSpring(Softbody::Spring* spring)
{
	auto it = std::find(springs.begin(), springs.end(), spring);

	if (it != springs.end())
	{
		springs.erase(it);
		return true;
	}

	return false;
}
/*Returns the current transform of the softbody.*/
ITransform* Softbody::GetTransform()
{
	return transform;
}
/*Returns the quantity of nodes in the softbody.*/
const size_t Softbody::GetNodeCount()
{
	return nodes.size();
}
const size_t Softbody::GetRowCount()
{
	return this->rowCount;
}
const size_t Softbody::GetColumnCount()
{
	return this->columnCount;
}
void Softbody::SetRowCount(size_t& count)
{
	this->rowCount = count;
}
void Softbody::SetColumnCount(size_t& count)
{
	this->columnCount = count;
}
Softbody::Node* Softbody::GetNodeByID(const unsigned int& id)
{
	for (Softbody::Node* node : nodes)
	{
		if (node->GetID() == id)
		{
			return node;
		}
	}

	return nullptr;
}
IRigidbody* Softbody::GetNodeRigidbodyByID(const unsigned int& id)
{
	for (Softbody::Node* node : nodes)
	{
		if (node->GetID() == id)
		{
			return node->GetComponent<IRigidbody*>();
		}
	}

	return nullptr;
}
/*Returns the radius of a node.*/
const bool Softbody::GetNodeRadius(const unsigned int& nodeIndex, float& radius)
{
	if (nodeIndex < GetNodeCount())
	{
		Node* node = nodes[nodeIndex];

		if (node)
		{
			radius = node->GetRadius();
			return true;
		}
	}

	return false;
}
const bool Softbody::GetNodePositions(std::vector<Vector3D>& positions)
{
	positions.clear();

	for (Softbody::Node* node : nodes)
	{
		IRigidbody* rb = node->GetComponent<IRigidbody*>();
		if (rb)
		{
			positions.push_back(rb->GetPosition());
		}
	}

	return positions.size() != 0;
}
/*Returns the position of a node.*/
const bool Softbody::GetNodePosition(const unsigned int& nodeIndex, Vector3D& position)
{
	if (nodeIndex < GetNodeCount())
	{
		Node* node = nodes[nodeIndex];

		if (node)
		{
			IRigidbody* rb = node->GetComponent<IRigidbody*>();

			if (rb)
			{
				position = rb->GetPosition();
				return true;
			}
		}
	}

	return false;
}
/*Return the worldMatrix of a node.*/
const bool Softbody::GetNodeWorldMatrix(const size_t& nodeIndex, Matrix4D& transformMatrix)
{
	if (nodeIndex < GetNodeCount())
	{
		Node* node = nodes[nodeIndex];

		if (node)
		{
			IRigidbody* rb = node->GetComponent<IRigidbody*>();

			if (rb)
			{
				transformMatrix = rb->GetWorldMatrix();
				return true;
			}
		}
	}

	return false;
}
/*Returns all connections between nodes in this softbody.*/
const bool Softbody::GetConnections(std::vector<std::pair<unsigned int, unsigned int>>& connections)
{
	connections = this->connections;
	return connections.size() != 0;
}
/*Returns all nodes in the softbody.*/
const bool Softbody::GetNodes(std::vector<Softbody::Node*>& nodes)
{
	nodes = this->nodes;
	return nodes.size() != 0;
}
/*Returns all springs in the softbody.*/
const bool Softbody::GetSprings(std::vector<Softbody::Spring*>& springs)
{
	springs = this->springs;
	return springs.size() != 0;
}
/*Commands for systems that do not know anything besides the ISoftbody interface.*/
const bool Softbody::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
	bool rc = false;
	Parameter p;

	if (input.FindParameterByName("command", p))
	{
		if (p.value == "GetRigidbodies")
		{			
			for (unsigned int i = 0; i < nodes.size(); i++)
			{
				char buf[255];
				sprintf_s(buf, 255, "IRigidbody_%d", i);

				output.Add(Parameter("IRigidbody", buf, nodes[i]->GetComponent<IRigidbody*>(), false));
			}
			return true;
		}
		else if(p.value == "GetConnections")
		{
			output.Add(Parameter("std::vector<std::pair<unsigned int, unsigned int>>", "connections", &connections, false));
			return true;
		}
	}

	return false;
}
/*Clones the current softbody.*/
ICloneable* Softbody::Clone()
{
	Softbody* clone = new Softbody();
	clone->SetName(GetName());
	clone->SetTransform((ITransform*)transform->Clone());
	clone->Activate(IsActive());
	
	for (auto connection : connections)
	{
		clone->AddConnection(connection.first, connection.second);
	}

	/*Review this later!*/
	for (auto node : nodes)
	{
		clone->AddNode((Node*)node->Clone());
	}

	for (auto spring : springs)
	{
		clone->AddSpring((Spring*)spring->Clone());
	}
	
	return clone;
}
/*Disposes the softbody, cleaning up all resources.*/
void Softbody::Dispose()
{
	DisposePointer(transform);
	DisposeVector(nodes);
	DisposeVector(springs);
}
/*Returns the type of the softbody as string.*/
const std::string Softbody::GetType()
{
	return "Softbody";
}
const unsigned long long Softbody::GetTypeID()
{
	return ENGINE_OBJ_ID_PHY_SOFTBODY;
}
/*Updates the softbody.*/
void Softbody::Update(const FrameTime& frameTime)
{
	this->isUpdateComplete = false;

	SetFrameTime(frameTime);

	for (Spring* spring : springs)
	{
		spring->Update(frameTime);
	}

	this->isUpdateComplete = true;
}

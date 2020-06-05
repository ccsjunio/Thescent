#pragma once
#include <ISoftbody.h>
#include <vector>
#include "RigidBody.h"
#include <Transform.h>
#include <IEntity.h>
#include <IDisposable.h>
#include <BellEngineID.h>

class Softbody : public ISoftbody
{
public:
	class Spring;
	class Node;

private:
	std::string name;
	size_t columnCount, rowCount;
	std::vector<Softbody::Spring*> springs;
	std::vector<Softbody::Node*> nodes;
	/*Pair of: NodeA ID, NodeB ID*/
	std::vector<std::pair<unsigned int, unsigned int>> connections;
	float mass;
	bool isActive, isUpdateComplete, allowCollisionDetection;
	void* parent;

public:
	class Node : public IEntity
	{
	private:
		/*	float radius;*/
		float radiusFactor;
		unsigned int id;
		bool isActive;
		std::string name;
	public:
		std::vector<Softbody::Spring*> springs;

		Node()
		{
			/*	radius = 1.0f;*/
			id = 0;
			radiusFactor = 0.5f;
			this->isActive = true;
		}
		~Node()
		{
			Dispose();
		}

		const std::string GetName()
		{
			return name;
		}
		void SetName(const std::string& name)
		{
			this->name = name;
		}
		void Activate(const bool& value)
		{
			this->isActive = value;
		}
		const bool IsActive()
		{
			return this->isActive;
		}
		const unsigned long long GetTypeID()
		{
			return ENGINE_OBJ_ID_PHY_SOFTBODYNODE;
		}
		const std::string GetType()
		{
			return "Softbody::Node";
		}
		ICloneable* Clone()
		{
			Node* clone = new Node();
			clone->SetID(GetID());
			clone->radiusFactor = radiusFactor;
			clone->springs = springs;

			std::vector<IComponent*> comps;
			if (GetComponents(comps))
			{
				for (auto c : comps)
				{
					auto _c = c->Clone();
					clone->AddComponent(_c);
				}
			}

			return clone;
		}
		void SetID(const int &id)
		{
			this->id = id;
		}
		const unsigned int GetID()
		{
			return id;
		}
		void Dispose()
		{
			//DisposeVector(components);
		}
		const bool IsConnectedTo(Softbody::Node* node)
		{
			for (Spring* spring : springs)
			{
				if (spring->GetOther(this) == node)
				{
					return true;
				}
			}

			return  false;
		}
		const void CalculateRadius()
		{
			/*float smallestDistance = FLT_MAX;
			size_t neighborCount = springs.size();
			float currentDistance = 0.0f;
			for (size_t i = 0; i < neighborCount; i++)
			{
				currentDistance = glm::distance(springs[i]->GetOther(this)->GetPosition(), GetPosition());
				if (currentDistance < smallestDistance)
				{
					smallestDistance = currentDistance;
				}
			}
			radius = smallestDistance * radiusFactor;*/
		}
		const float GetRadius()
		{
			ParameterContainer params;
			if (GetComponent<IRigidbody*>()->GetCollider()->GetContent("radius", params))
			{
				Parameter p;
				if (params.FindParameterByName("radius", p))
				{
					return *(float*)p.valuePtr;
				}
			}

			return 0.0f;
		}
		const void GetSprings(std::vector<Softbody::Spring*>& springs)
		{
			springs = this->springs;
		}
		const bool AddSpring(Softbody::Spring* spring)
		{
			auto it = std::find(springs.begin(), springs.end(), spring);
			if (it == springs.end())
			{
				springs.push_back(spring);
				return true;
			}

			return false;
		}
		const bool RemoveSpring(Softbody::Spring* spring)
		{
			auto it = std::find(springs.begin(), springs.end(), spring);
			if (it != springs.end())
			{
				springs.erase(it);
				return true;
			}

			return false;
		}
		const bool SetVelocity(const Vector3D& velocity)
		{
			IRigidbody* rb = GetComponent<IRigidbody*>();
			if (rb && !rb->IsFixed())
			{
				rb->SetVelocity(velocity);
				return true;
			}

			return false;
		}
		const bool GetVelocity(Vector3D& velocity)
		{
			IRigidbody* rb = GetComponent<IRigidbody*>();
			if (rb)
			{
				velocity = rb->GetVelocity();
				return true;
			}

			velocity = Vector3D(0.0f, 0.0f, 0.0f);
			return false;
		}
		const bool GetPosition(Vector3D& position)
		{
			IRigidbody* rb = GetComponent<IRigidbody*>();
			if (rb)
			{
				position = rb->GetPosition();
				return true;
			}

			position = Vector3D(0.0f, 0.0f, 0.0f);
			return false;
		}
		const bool SetAcceleration(const Vector3D& velocity)
		{
			IRigidbody* rb = GetComponent<IRigidbody*>();
			if (rb && !rb->IsFixed())
			{
				rb->SetAcceleration(velocity);
				return true;
			}

			return false;
		}
		const bool GetAcceleration(Vector3D& acceleration)
		{
			IRigidbody* rb = GetComponent<IRigidbody*>();
			if (rb)
			{
				acceleration = rb->GetAcceleration();
				return true;
			}

			acceleration = Vector3D(0.0f, 0.0f, 0.0f);
			return false;
		}
		inline const bool GetMass(float& mass)
		{
			IRigidbody* rb = GetComponent<IRigidbody*>();
			if (rb)
			{
				mass = rb->GetMass();
				return true;
			}

			mass = 0.0f;
			return false;
		}
		inline const bool SetMass(const float& mass)
		{
			IRigidbody* rb = GetComponent<IRigidbody*>();
			if (rb)
			{
				rb->SetMass(mass);
				return true;
			}

			return false;
		}
	};
	class Spring : public ICloneable
	{
	private:
		Node* a, * b;
		float springConstant, restingDistance;
		Vector3D springForce;
	public:
		Spring() : a(nullptr), b(nullptr), springConstant(0.0f), restingDistance(0.0f), springForce(Vector3D()) {};
		Spring(Node* a, Node* b)
		{
			this->a = a;
			this->b = b;
			CalculateRestingDistance();
		}
		~Spring() {}
			
		inline void SetSpringForce(const Vector3D& springForce)
		{
			this->springForce = springForce;
		}
		inline const Vector3D GetSpringForce()
		{
			return this->springForce;
		}
		inline const float GetSpringConstant()
		{
			return this->springConstant;
		}
		inline void SetSpringConstant(const float& springConstant)
		{
			this->springConstant = springConstant;
		}
		ICloneable* Clone()
		{
			Spring* clone = new Spring();

			clone->a = a;
			clone->b = b;
			clone->springConstant = springConstant;
			clone->restingDistance = restingDistance;
			clone->springForce = springForce;

			return clone;
		}
		inline Node* GetNodeA()
		{
			return a;
		}
		inline Node* GetNodeB()
		{
			return b;
		}
		inline Node* GetOther(Node* node)
		{
			return node == a ? b : b;
		}
		inline void CalculateRestingDistance()
		{
			restingDistance = glm::distance(GetNodeA()->GetComponent<IRigidbody*>()->GetPosition(), GetNodeB()->GetComponent<IRigidbody*>()->GetPosition());
		}
		inline const float GetRestingDistance()
		{
			return this->restingDistance;
		}
		inline void Update(const FrameTime& frameTime)
		{
			Vector3D sep = GetNodeB()->GetComponent<IRigidbody*>()->GetPosition() - GetNodeA()->GetComponent<IRigidbody*>()->GetPosition();
			float distance = glm::length(sep);
			float x = (distance - restingDistance);

			SetSpringForce(-glm::normalize(sep) * x * springConstant);

			//Apply springforce to nodes
			ApplyForceToNodes();
		}
		inline void ApplyForceToNodes()
		{
			/*Gets the rigidbodies inside the node and updates its acceleration using the springForce*/

			IRigidbody* rbB = GetNodeB()->GetComponent<IRigidbody*>();
			if (rbB && !rbB->IsFixed())
			{
				/*Vector3D acc = rbB->GetAcceleration() + springForce;
				if (glm::length(acc) != 0.0f)
				{
					rbB->SetAcceleration(acc / rbB->GetMass());
				}*/

				rbB->SetAcceleration(rbB->GetAcceleration() + springForce / rbB->GetMass());
			}

			IRigidbody* rbA = GetNodeA()->GetComponent<IRigidbody*>();
			if (rbA && !rbA->IsFixed())
			{
				rbA->SetAcceleration(rbA->GetAcceleration() - springForce / rbA->GetMass());
			}
		}
	};
	
	Softbody();
	~Softbody();

	const bool IsUpdateComplete();
	const std::string GetName();
	void SetName(const std::string& name);
	void ToggleCollisionDetection(const bool& option);
	const bool AllowCollisionDetection();
	void SetParent(void* parent);
	void* GetParent();
	Softbody::Node* FindNodeByID(const unsigned int &id);
	const bool AddConnection(const unsigned int &c0, const unsigned int &c1);
	const bool RemoveConnection(const unsigned int& c0, const unsigned int& c1);
	void Activate(const bool& value);
	const bool IsActive();
	const bool AddNode(Softbody::Node* spring);
	const bool RemoveNode(Softbody::Node* spring);
	const bool AddSpring(Softbody::Spring* spring);
	const bool RemoveSpring(Softbody::Spring* spring);
	ITransform* GetTransform();
	const size_t GetNodeCount();
	const size_t GetRowCount();
	const size_t GetColumnCount();
	void SetRowCount(size_t& count);
	void SetColumnCount(size_t& count);
	Softbody::Node* GetNodeByID(const unsigned int& id);
	IRigidbody* GetNodeRigidbodyByID(const unsigned int& id);
	const bool GetNodeRadius(const unsigned int& nodeIndex, float& radius);
	const bool GetNodePositions(std::vector<Vector3D>& positions);
	const bool GetNodePosition(const unsigned int& nodeIndex, Vector3D& position);
	const bool GetNodeWorldMatrix(const size_t& nodeIndex, Matrix4D& transformMatrix);
	const bool GetConnections(std::vector<std::pair<unsigned int, unsigned int>>& connections);
	const bool GetNodes(std::vector<Softbody::Node*>& nodes);
	const bool GetSprings(std::vector<Softbody::Spring*>& springs);
	const bool RunCommand(ParameterContainer& input, ParameterContainer& output);
	ICloneable* Clone();
	void Dispose();
	const std::string GetType();
	const unsigned long long GetTypeID();
	void Update(const FrameTime& frameTime);
};


#pragma once
#include <IAIBehavior.h>
#include <MathLibrary.h>
#include <Entity.h>

class PathFollowing : public IAIBehavior
{
private:
    std::vector<Vector3D> path;
    bool isActive, isLooping, isUpdateComplete;
    unsigned int currentNode;
    float nodeRadius;
    float topSpeed;
    std::string name;
    std::function<void(const IAIBehavior::AIEvent_s&)> aiEvent;

    void LookAt(Entity*& parent, const Vector3D& position, const double& deltaTime);

public:
    PathFollowing() :
        isUpdateComplete(false),
        isActive(true),
        isLooping(false),
        currentNode(0),
        nodeRadius(0.0f),
        topSpeed(0.0f)
    {
    }
    ~PathFollowing();

    void BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn);
    void Loop(const bool& value);
    const bool IsLooping();
    const bool IsUpdateComplete();
    const std::string GetName();
    void SetName(const std::string& name);
    const unsigned long long GetTypeID();
    const bool RunCommand(ParameterContainer& input, ParameterContainer& output);
    void SetNodeRadius(const float& value);
    const float GetNodeRadius();
    void SetCurrentNode(unsigned int nodeID);
    const unsigned int GetCurrentNode();
    void Activate(const bool& option = true);
    const bool IsActive();
    bool AddPathNode(const Vector3D& node);
    bool RemovePathNode(const Vector3D& node);
    void SetTopSpeed(const float& value);
    const float GetTopSpeed();
    void Dispose();
    ICloneable* Clone();
    const std::string GetType();
    void Update(const FrameTime& deltaTime);
};


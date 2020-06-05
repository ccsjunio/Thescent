#pragma once
#include <IAIBehavior.h>
#include <Entity.h>
#include <MathLibrary.h>

class Flocking : public IAIBehavior
{
private:
    bool isActive, isUpdateComplete;
    const float DEFAULT_WEIGHT = 1.0f / 3.0f;

    std::string name;
    float cohesionWeight, alignmentWeight, separationWeight, topSpeed;
    Vector3D cohesion, separation, alignment;
    std::vector<Entity*> boids;
    std::function<void(const IAIBehavior::AIEvent_s&)> aiEvent;

    void SetCohesion(const Vector3D& value);
    void SetSeparation(const Vector3D& value);
    void SetAlignment(const Vector3D& value);

    const Vector3D GetSteeringForce(Entity* boid);
    void CalculateVectors();
    void CalculateSteering(Entity*& parent);
    void LookAt(Entity* boid, const Vector3D& position, const double& deltaTime);

public:
    Flocking() :
        isUpdateComplete(false),
        isActive(true),
        cohesionWeight(DEFAULT_WEIGHT),
        alignmentWeight(DEFAULT_WEIGHT),
        separationWeight(DEFAULT_WEIGHT),
        topSpeed(5.0f),
        cohesion(Vector3D()),
        alignment(Vector3D()),
        separation(Vector3D()) {};
    Flocking(const float& cohesionWeight, const float& alignmentWeight, const float& separationWeight);
    ~Flocking();

    void BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn);
    const bool IsUpdateComplete();
    const std::string GetName();
    void SetName(const std::string& name);
    const unsigned long long GetTypeID();
    const bool RunCommand(ParameterContainer& input, ParameterContainer& output);
    void Activate(const bool& option = true);
    const bool IsActive();
    bool AddBoid(Entity* boid);
    bool RemoveBoid(Entity* boid);
    /*A value from 0 to 1*/
    void SetCohesionWeight(const float& value);
    const float GetCohesionWeight();
    /*A value from 0 to 1*/
    void SetAlignmentWeight(const float& value);
    const float GetAlignmentWeight();
    /*A value from 0 to 1*/
    void SetSeparationWeight(const float& value);
    const float GetSeparationWeight();
    void SetTopSpeed(const float& value);
    const float GetTopSpeed();
    void Dispose();
    ICloneable* Clone();
    const std::string GetType();
    void Update(const FrameTime& deltaTime);
};


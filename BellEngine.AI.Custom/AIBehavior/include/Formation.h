#pragma once
#include <IAIBehavior.h>
#include <MathLibrary.h>
#include <Entity.h>

class Formation : public IAIBehavior
{
private:
    struct Coordinator_s
    {
    public:
        Vector3D position, velocity, target;
        float topSpeed, slowingDistance;
        std::vector<Vector3D> formationShape;

        Coordinator_s() :
            position(Vector3D()),
            velocity(Vector3D()),
            target(Vector3D()),
            topSpeed(1.0f),
            slowingDistance(1.0f)
        {};

        const Vector3D GetOffset(size_t nodeID)
        {
            return formationShape[nodeID] + position;
        }
        void Seek(const double& deltaTime)
        {
            //Finds the desired seeking velocity
            Vector3D desiredVelocity = target - position;
            Vector3D normalizedVelocity = glm::normalize(desiredVelocity);

            //Finds the distance to the target
            float dist = glm::distance(target, position);

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
            Vector3D iavgVel = velocity;
            Vector3D steer = desiredVelocity - iavgVel;

            //Add steering force to current velocity
            iavgVel += steer * (float)deltaTime;

            if (glm::length(iavgVel) > topSpeed)
            {
                iavgVel = glm::normalize(iavgVel) * topSpeed;
            }

            velocity = (iavgVel);
            position += velocity;
        }
        void Update(const double& deltatime)
        {
            if (glm::distance(position, target) < 1.0f)
            {
                velocity = Vector3D();
            }
            else
            {
                Seek(deltatime);
            }
        }
    };

    Coordinator_s coordinator;
    float topSpeed;
    bool isActive, isUpdateComplete;
    std::vector<Entity*> boids;
    std::string name;
    std::function<void(const IAIBehavior::AIEvent_s&)> aiEvent;

    const Quaternion SafeQuatLookAt(const Vector3D& lookFrom, const Vector3D& lookTo, const Vector3D& up, const Vector3D& alternativeUp);
    void LookAt(Entity* boid, const Vector3D& target, const double& deltaTime);
    void Seek(Entity* boid, const Vector3D& target, const float& slowingDistance, const float& topSpeed, const double& deltaTime);

public:
    Formation() :
        isUpdateComplete(false),
        isActive(true),
        topSpeed(0.0f),
        coordinator(Coordinator_s())
    {

    }
    ~Formation();

    void BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn);
    const bool IsUpdateComplete();
    const std::string GetName();
    void SetName(const std::string& name);
    const unsigned long long GetTypeID();
    const bool RunCommand(ParameterContainer& input, ParameterContainer& output);
    void SetCoordinatorPosition(const Vector3D& position);
    const Vector3D GetCoordinatorPosition();
    void SetCoordinatorTarget(const Vector3D& target);
    const Vector3D GetCoordinatorTarget();
    void Activate(const bool& option = true);
    const bool IsActive();
    bool AddBoid(Entity* boid);
    bool RemoveBoid(Entity* boid);
    bool AddFormationNode(const Vector3D& node);
    bool RemoveFormationNode(const Vector3D& node);
    void SetTopSpeed(const float& value);
    const float GetTopSpeed();
    ICloneable* Clone();
    const std::string GetType();
    void Dispose();
    void Update(const FrameTime& deltaTime);
};


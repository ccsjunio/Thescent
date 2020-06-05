#include "../include/Flocking.h"
#include <IRigidbody.h>
#include <MathLibrary.h>
#include <Easing.h>
#include <BellEngineID.h>

void Flocking::SetCohesion(const Vector3D& value)
{
    this->cohesion = value;
}
void Flocking::SetSeparation(const Vector3D& value)
{
    this->separation = value;
}
void Flocking::SetAlignment(const Vector3D& value)
{
    this->alignment = value;
}

const Vector3D Flocking::GetSteeringForce(Entity* boid)
{
    Vector3D forward = Vector3D();
    Vector3D boidVelocity = boid->GetComponent<IRigidbody*>()->GetVelocity();
    if (boidVelocity == Vector3D())
    {
        forward = glm::normalize(boid->GetComponent<IRigidbody*>()->GetForwardInWorld());
    }
    else
    {
        forward = glm::normalize(boid->GetComponent<IRigidbody*>()->GetVelocity());
    }

    Vector3D steeringForce = Vector3D();
    float sizeMinusOne = (float)boids.size() - 1;

    if (boids.size() < 2)
    {
        steeringForce = forward;
    }

    // Calcuale cohesion vector
    Vector3D cohesionVec = glm::normalize((cohesion - boid->GetComponent<IRigidbody*>()->GetPosition()) / (sizeMinusOne - boid->GetComponent<IRigidbody*>()->GetPosition())) * cohesionWeight;

    // Remove the current position from the separation vector to exclude the 
    // current flock member from the sum of positions.
    Vector3D separationVec = (separation - boid->GetComponent<IRigidbody*>()->GetPosition());
    separationVec /= sizeMinusOne;
    separationVec = -glm::normalize(boid->GetComponent<IRigidbody*>()->GetPosition() - separationVec) * separationWeight;

    // Remove the current alignment from the separation vector to exclude the
    // current flock member from the sum of alignments;
    Vector3D alignmentVec = (alignment - forward);
    alignmentVec /= sizeMinusOne;
    alignmentVec = glm::normalize(alignmentVec) * alignmentWeight;

    // calculate the final steering direction
    // These three vectors should add up to a normalized vector, assuming
    // the weight values add up to 1.
    steeringForce = cohesionVec + separationVec + alignmentVec;

    if (glm::length(steeringForce) == 0)
    {
        steeringForce = forward;
    }

    return steeringForce;
}
void Flocking::CalculateVectors()
{
    // Calculate the cohesion for the flock. This is the average
    // position of all of the members of the flock.
    Vector3D forward = cohesion = separation = alignment = Vector3D();

    for (Entity* boid : boids)
    {
        auto boidVelocity = boid->GetComponent<IRigidbody*>()->GetVelocity();
        if (boidVelocity == Vector3D())
        {
            forward = glm::normalize(boid->GetComponent<IRigidbody*>()->GetForwardInWorld());
        }
        else
        {
            forward = glm::normalize(boid->GetComponent<IRigidbody*>()->GetVelocity());
        }

        SetCohesion(cohesion + boid->GetComponent<IRigidbody*>()->GetPosition());
        SetSeparation(separation + boid->GetComponent<IRigidbody*>()->GetPosition());
        SetAlignment(alignment + forward);
    }
}
void Flocking::CalculateSteering(Entity*& parent)
{
    CalculateVectors();

    Vector3D steeringForce = Vector3D();
    for (Entity* boid : boids)
    {
        steeringForce = GetSteeringForce(boid);

        boid->GetComponent<IRigidbody*>()->SetVelocity(boid->GetComponent<IRigidbody*>()->GetVelocity() + (steeringForce * (float)this->frameTime.GetDeltaTime()));
        //boid->GetComponent<IRigidbody*>()->SetAcceleration(steeringForce);
    }
}
void Flocking::LookAt(Entity* boid, const Vector3D& position, const double& deltaTime)
{
    Quaternion initialRotation = boid->GetComponent<IRigidbody*>()->GetRotation();

    Vector3D normalizedDistance = glm::normalize(position - boid->GetComponent<IRigidbody*>()->GetPosition());
    Quaternion rotationDirection = glm::quatLookAt(normalizedDistance, boid->GetComponent<IRigidbody*>()->GetUp());
    Quaternion slerp = glm::slerp(initialRotation, rotationDirection, Easing::GetEasingValue((float)deltaTime, (Easing::Easing_e::None)));

    //Rotate	
    boid->GetComponent<IRigidbody*>()->SetRotation(slerp);
}

Flocking::Flocking(const float& cohesionWeight, const float& alignmentWeight, const float& separationWeight)
{
    Activate();
    SetSeparation(Vector3D());
    SetAlignment(Vector3D());
    SetCohesion(Vector3D());
    SetTopSpeed(5.0f);
    SetCohesionWeight(cohesionWeight);
    SetAlignmentWeight(alignmentWeight);
    SetSeparationWeight(separationWeight);
};

Flocking::~Flocking()
{
    Dispose();
}

void Flocking::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
const bool Flocking::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string Flocking::GetName()
{
    return this->name;
}
void Flocking::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long Flocking::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORFLOCKING;
}
const bool Flocking::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    bool rc = false;

    Parameter p;
    rc = input.FindParameterByName("commandname", p);
    if (rc)
    {
        if (p.value == "SetCohesionWeight")
        {
            rc = input.FindParameterByName("weightvalue", p);
            if (rc)
            {
                float newValue = strtof(p.value.c_str(), NULL);
                if (newValue > 0.0f)
                {
                    //Adjusts other weights by half of the changed amount, example:
                    //if cohesionW = separationW = alignmentW = 0.33
                    //and a new cohesionW is going to be set to 0.4
                    //then calculate the changed amount doing newcohesionW - oldohesionW / 2
                    //(0.4 - 0.33) / 2 = 0.035 -> then equally add/subtract to the other two weights
                    if (newValue < 1.0f)
                    {
                        float threshold = newValue - GetCohesionWeight();
                        SetAlignmentWeight(GetAlignmentWeight() + (-1) * threshold / 2);
                        SetSeparationWeight(GetSeparationWeight() + (-1) * threshold / 2);
                        SetCohesionWeight(newValue);
                    }
                    else
                    {
                        SetCohesionWeight(1.0f);
                        SetAlignmentWeight(0.0f);
                        SetSeparationWeight(0.0f);
                    }
                }
                else
                {
                    float threshold = 1.0f - GetSeparationWeight() - GetAlignmentWeight();
                    SetAlignmentWeight(GetAlignmentWeight() + threshold / 2);
                    SetSeparationWeight(GetSeparationWeight() + threshold / 2);
                    SetCohesionWeight(0.0f);
                }

                //Return the new values
                output.Add(Parameter("float", "cohesionWeight", std::to_string(GetCohesionWeight()), false));
                output.Add(Parameter("float", "alignmentWeight", std::to_string(GetAlignmentWeight()), false));
                output.Add(Parameter("float", "separationWeight", std::to_string(GetSeparationWeight()), false));

                return true;
            }
        }
        else if (p.value == "SetAlignmentWeight")
        {
            rc = input.FindParameterByName("weightvalue", p);
            if (rc)
            {
                float newValue = strtof(p.value.c_str(), NULL);
                if (newValue > 0.0f)
                {
                    //Adjusts other weights by half of the changed amount, example:
                   //if cohesionW = separationW = alignmentW = 0.33
                   //and a new cohesionW is going to be set to 0.4
                   //then calculate the changed amount doing newcohesionW - oldohesionW / 2
                   //(0.4 - 0.33) / 2 = 0.035 -> then equally add/subtract to the other two weights
                    if (newValue < 1.0f)
                    {
                        float threshold = newValue - GetAlignmentWeight();
                        SetCohesionWeight(GetCohesionWeight() + (-1) * threshold / 2);
                        SetSeparationWeight(GetSeparationWeight() + (-1) * threshold / 2);
                        SetAlignmentWeight(newValue);
                    }
                    else
                    {
                        SetCohesionWeight(0.0f);
                        SetAlignmentWeight(1.0f);
                        SetSeparationWeight(0.0f);
                    }
                }
                else
                {
                    float threshold = 1.0f - GetCohesionWeight() - GetSeparationWeight();
                    SetCohesionWeight(GetCohesionWeight() + threshold / 2);
                    SetSeparationWeight(GetSeparationWeight() + threshold / 2);
                    SetAlignmentWeight(0.0f);
                }

                //Return the new values
                output.Add(Parameter("float", "cohesionWeight", std::to_string(GetCohesionWeight()), false));
                output.Add(Parameter("float", "alignmentWeight", std::to_string(GetAlignmentWeight()), false));
                output.Add(Parameter("float", "separationWeight", std::to_string(GetSeparationWeight()), false));

                return true;
            }
        }
        else if (p.value == "SetSeparationWeight")
        {
            rc = input.FindParameterByName("weightvalue", p);
            if (rc)
            {
                float newValue = strtof(p.value.c_str(), NULL);
                if (newValue > 0.0f)
                {
                    //Adjusts other weights by half of the changed amount, example:
                    //if cohesionW = separationW = alignmentW = 0.33
                    //and a new cohesionW is going to be set to 0.4
                    //then calculate the changed amount doing newcohesionW - oldohesionW / 2
                    //(0.4 - 0.33) / 2 = 0.035 -> then equally add/subtract to the other two weights
                    if (newValue < 1.0f)
                    {
                        float threshold = newValue - GetSeparationWeight();
                        SetCohesionWeight(GetCohesionWeight() + (-1) * threshold / 2);
                        SetAlignmentWeight(GetAlignmentWeight() + (-1) * threshold / 2);
                        SetSeparationWeight(newValue);
                    }
                    else
                    {
                        SetCohesionWeight(0.0f);
                        SetAlignmentWeight(0.0f);
                        SetSeparationWeight(1.0f);
                    }
                }
                else
                {
                    float threshold = 1.0f - GetCohesionWeight() - GetAlignmentWeight();
                    SetCohesionWeight(GetCohesionWeight() + threshold / 2);
                    SetAlignmentWeight(GetAlignmentWeight() + threshold / 2);
                    SetSeparationWeight(0.0f);
                }

                //Return the new values
                output.Add(Parameter("float", "cohesionWeight", std::to_string(GetCohesionWeight()), false));
                output.Add(Parameter("float", "alignmentWeight", std::to_string(GetAlignmentWeight()), false));
                output.Add(Parameter("float", "separationWeight", std::to_string(GetSeparationWeight()), false));

                return true;
            }
        }
        else if (p.value == "GetWeights")
        {
            output.Add(Parameter("float", "cohesionWeight", std::to_string(GetCohesionWeight()), false));
            output.Add(Parameter("float", "alignmentWeight", std::to_string(GetAlignmentWeight()), false));
            output.Add(Parameter("float", "separationWeight", std::to_string(GetSeparationWeight()), false));
            return true;
        }
    }

    return false;
}
void Flocking::Activate(const bool& option)
{
    this->isActive = option;
}
const bool Flocking::IsActive()
{
    return isActive;
}
bool Flocking::AddBoid(Entity* boid)
{
    auto it = std::find(boids.begin(), boids.end(), boid);
    //auto it = std::find_if(boids.begin(), boids.end(), [&boid](Entity* go) { return go->GetName() == boid->GetName(); });

    if (it == boids.end())
    {
        boids.push_back(boid);
        return true;
    }

    return false;
}
bool Flocking::RemoveBoid(Entity* boid)
{
    auto it = std::find(boids.begin(), boids.end(), boid);
    if (it != boids.end())
    {
        boids.erase(it);
        return true;
    }

    return false;
}
/*A value from 0 to 1*/
void Flocking::SetCohesionWeight(const float& value)
{
    float newtotalWeight = value + this->alignmentWeight + this->separationWeight;
    if (newtotalWeight < 1.0f)
    {
        this->cohesionWeight = value;
    }
    else
    {
        this->cohesionWeight = 1.0f - this->alignmentWeight - this->separationWeight;
    }
}
const float Flocking::GetCohesionWeight()
{
    return cohesionWeight;
}
/*A value from 0 to 1*/
void Flocking::SetAlignmentWeight(const float& value)
{
    float newtotalWeight = value + this->cohesionWeight + this->separationWeight;
    if (newtotalWeight < 1.0f)
    {
        this->alignmentWeight = value;
    }
    else
    {
        this->alignmentWeight = 1.0f - this->cohesionWeight - this->separationWeight;
    }
}
const float Flocking::GetAlignmentWeight()
{
    return alignmentWeight;
}
/*A value from 0 to 1*/
void Flocking::SetSeparationWeight(const float& value)
{
    float newtotalWeight = value + this->cohesionWeight + this->alignmentWeight;
    if (newtotalWeight < 1.0f)
    {
        this->separationWeight = value;
    }
    else
    {
        this->separationWeight = 1.0f - this->cohesionWeight - this->alignmentWeight;
    }
}
const float Flocking::GetSeparationWeight()
{
    return separationWeight;
}
void Flocking::SetTopSpeed(const float& value)
{
    this->topSpeed = value;
}
const float Flocking::GetTopSpeed()
{
    return topSpeed;
}
void Flocking::Dispose()
{

}
ICloneable* Flocking::Clone()
{
    Flocking* clone = new Flocking();
    clone->SetAlignmentWeight(this->GetAlignmentWeight());
    clone->SetCohesionWeight(this->GetCohesionWeight());
    clone->SetSeparationWeight(this->GetSeparationWeight());
    clone->boids = this->boids;

    return clone;
}
const std::string Flocking::GetType()
{
    return "flocking";
}
void Flocking::Update(const FrameTime& deltaTime)
{
    this->isUpdateComplete = false;

    SetFrameTime(deltaTime);

    Entity* parent = GetParent<Entity*>();
    if (parent)
    {
        CalculateSteering(parent);
    }
    else
    {
        Activate(false);
    }

    this->isUpdateComplete = true;
}
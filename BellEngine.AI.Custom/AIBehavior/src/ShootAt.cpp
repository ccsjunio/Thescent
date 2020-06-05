#include "../include/ShootAt.h"
#include <IRigidbody.h>
#include <BellEngineID.h>
#include <Scene.h>
#include <IRender.h>

const bool ShootAt::IsThereAnObstacle(Entity*& parent)
{
    if (this->scene)
    {
        std::vector<Entity*> entities;
        if (this->scene->GetEntities(entities))
        {
            IRigidbody* parentRB = parent->GetComponent<IRigidbody*>();
            float distToTarget = glm::distance(parentRB->GetPosition(), target->GetComponent<IRigidbody*>()->GetPosition());
            Vector3D rayDirection = glm::normalize(target->GetComponent<IRigidbody*>()->GetPosition() - parentRB->GetPosition());

            for (auto& entity : entities)
            {
                if (!entity || entity == target) continue;
                //if (entity->GetName().find("PLANE") == std::string::npos) continue;

                auto rb = entity->GetComponent<IRigidbody*>();
                if (!rb) continue;
                auto col = rb->GetCollider();
                if (col->GetTypeID() != ENGINE_OBJ_ID_PHY_COLLIDERPLANE) continue;

                float dist = glm::distance(parentRB->GetPosition(), rb->GetPosition());

                if (dist < radius)
                {
                    if (dist < distToTarget)
                    {
                        if (col->Raycast(parentRB->GetPosition(), rayDirection, radius))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}
/*Shoots a projectile towards a target.*/
void ShootAt::Shoot(Entity*& parent)
{
    if (!target || !projectile || !scene) return;

    IRigidbody* parentRB = parent->GetComponent<IRigidbody*>();
    if (!parentRB) return;

    IRigidbody* targetRB = target->GetComponent<IRigidbody*>();
    if (!targetRB) return;

    if (GetRadius() == 0.0f || glm::distance(parentRB->GetPosition(), targetRB->GetPosition()) < GetRadius())
    {
        /*Check if entity is facing target.*/
        float dot = glm::dot(parentRB->GetForwardInWorld(), glm::normalize(targetRB->GetPosition() - parentRB->GetPosition()));
        if (dot > 0.7f)
        {
            for (Vector3D point : startPositions)
            {
                Entity* clone = projectileBuffer.Next();
                if (clone)
                {                   
                    /*Wake entity*/
                    clone->SetDisposing(false);
                    std::vector<IComponent*> components;
                    if (clone->GetComponents(components))
                    {
                        for (auto& component : components)
                        {
                            component->Activate(true);
                        }
                    }

                    IRender* cloneVR = clone->GetComponent<IRender*>();
                    if (cloneVR)
                    {
                        cloneVR->SetVisibility(true);
                    }

                    IRigidbody* cloneRB = clone->GetComponent<IRigidbody*>();
                    if (cloneRB)
                    {
                        /*Transform the starting point for the trajectory, which is the position of the barrel.*/
                        Vector3D transformedbarrelPosition = parentRB->GetWorldMatrix() * Vector4D(point, 1.0f);
                        cloneRB->SetRotation(parentRB->GetRotation());
                        cloneRB->SetPosition(transformedbarrelPosition);
                        cloneRB->ToggleCollisionDetection(true);

                        /*Find the direction of the projectile and velocity.*/
                        Vector3D farpoint = parentRB->GetWorldMatrix() * Vector4D(pointAheadOfParent, 1.0f);
                        Vector3D direction = glm::normalize(farpoint - transformedbarrelPosition);
                        cloneRB->SetVelocity(direction * projectileImpulse);

                        /*Rotate projectile towards direction of movement*/
                        cloneRB->SetRotation(glm::quatLookAt(direction, cloneRB->GetUp()));

                        cloneRB->Transform();
                    }

                    if (cloneVR && cloneRB)
                    {
                        if (aiEvent)
                        {
                            IAIBehavior::AIEvent_s ai_event;
                            ai_event.ai = this;
                            ai_event.eventDescription = "shoot";

                            aiEvent(ai_event);
                        }
                    }
                }
            }
        }
    }
}

ShootAt::~ShootAt()
{
    Dispose();
}

void ShootAt::SetProjectileImpulse(const float& value)
{
    this->projectileImpulse = value;
}
const float ShootAt::GetProjectileImpulse()
{
    return this->projectileImpulse;
}
void ShootAt::BindAIEvent(std::function<void(const IAIBehavior::AIEvent_s&)> fn)
{
    aiEvent = std::bind(fn, std::placeholders::_1);
}
/*Sets a point ahead of the parent. This is the 'target' for the caclulation of the bullet trajectory.*/
void ShootAt::SetCrosshair(const Vector3D& value)
{
    this->pointAheadOfParent = value;
}
const Vector3D ShootAt::GetCrosshair()
{
    return this->pointAheadOfParent;
}
const bool ShootAt::AddBarrelPosition(const Vector3D& value)
{
    startPositions.push_back(value);
    return true;
}
const bool ShootAt::GetBarrelPositions(std::vector<Vector3D>& value)
{
    value = this->startPositions;
    return !value.empty();
}
void ShootAt::SetRadius(const float& radius)
{
    this->radius = radius;
}
const float ShootAt::GetRadius()
{
    return this->radius;
}
void ShootAt::SetIntervalBetweenShots(const float& value)
{
    this->intervalBetweenShots = value;
}
const float ShootAt::GetIntervalBetweenShots()
{
    return this->intervalBetweenShots;
}
void ShootAt::SetTarget(Entity*& target)
{
    this->target = target;
}
Entity* ShootAt::GetTarget()
{
    return target;
}
void ShootAt::SetProjectile(Entity*& value)
{
    this->projectile = value;

    if (this->projectile)
    {
        Entity* parent = GetParent<Entity*>();
        if (parent)
        {
            projectileBuffer.Reserve(10u);

            for (unsigned int i = 0; i < 10u; i++)
            {
                auto clone = (Entity*)this->projectile->Clone();
                if (clone)
                {
                    clone->SetName("CLONED_" + clone->GetName() + "_" + parent->GetName() + "_" + std::to_string(i));
                    projectileBuffer.Add(clone);
                    scene->Append(clone);
                }
            }
        }
    }    
}
Entity* ShootAt::GetProjectile()
{
    return this->projectile;
}
void ShootAt::SetScene(Scene*& value)
{
    this->scene = value;
}
Scene* ShootAt::GetScene()
{
    return this->scene;
}
const bool ShootAt::IsUpdateComplete()
{
    return this->isUpdateComplete;
}
const std::string ShootAt::GetName()
{
    return this->name;
}
void ShootAt::SetName(const std::string& name)
{
    this->name = name;
}
const unsigned long long ShootAt::GetTypeID()
{
    return ENGINE_OBJ_ID_AI_BEHAVIORSHOOTAT;
}
const bool ShootAt::RunCommand(ParameterContainer& input, ParameterContainer& output)
{
    return false;
}
void ShootAt::Activate(const bool& option)
{
    this->isActive = option;
}
const bool ShootAt::IsActive()
{
    return isActive;
}
ICloneable* ShootAt::Clone()
{
    ShootAt* clone = new ShootAt();
    clone->startPositions = startPositions;
    clone->name = name;
    clone->scene = scene;
    clone->projectile = projectile;
    clone->radius = radius;
    clone->target = target;

    return clone;
}
const std::string ShootAt::GetType()
{
    return "shootat";
}
void ShootAt::Dispose()
{
    /*for (auto& proj : projectileBuffer)
    {
        scene->Remove(proj, false);
    }

   DisposeVector(projectileBuffer);*/
}
void ShootAt::Update(const FrameTime& frameTime)
{
    if (!this->isActive) return;
    this->isUpdateComplete = false;

    SetFrameTime(frameTime);

    Entity* parent = GetParent<Entity*>();
    if (parent && !parent->IsDisposing())
    {
        timeAcc_intervalBetweenShots += this->frameTime.GetDeltaTime();
        if (timeAcc_intervalBetweenShots > intervalBetweenShots)
        {
            if (!IsThereAnObstacle(parent))
            {
                Shoot(parent);
            }

            timeAcc_intervalBetweenShots = 0.0f;
        }
    }
    else
    {
        this->isActive = false;
    }

    this->isUpdateComplete = true;
}
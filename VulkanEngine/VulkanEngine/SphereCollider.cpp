#include "pch.h"
#include "SphereCollider.h"

#include "EntityManager.h"
#include "DebugManager.h"

#pragma region Contructor

SphereCollider::SphereCollider(float radius)
{
    this->radius = radius;

    colliderType = ColliderTypes::Sphere;
}

#pragma endregion

#pragma region Accessors

float SphereCollider::getRadius()
{
    return radius;
}

void SphereCollider::SetRadius(float value)
{
    radius = value;
}

#pragma endregion

#pragma region Collider Generation

void SphereCollider::GenerateFromMesh(std::shared_ptr<Mesh> mesh)
{
    //Find the center of the mesh
    std::vector<Vertex> vertices = mesh->GetVertices();
    glm::vec3 position = vertices[0].position * GetParentTransform()->GetScale();
    glm::vec3 max = position;
    glm::vec3 min = position;

    //Keeps track of the points furthest from the center of the mesh
    glm::vec3 edgePoints[6] = {
        position,
        position,
        position,
        position,
        position,
        position
    };

    for (int i = 1; i < vertices.size(); i++) {
        position = vertices[i].position * GetParentTransform()->GetScale();

        if (position.x < min.x) {
            min.x = position.x;
            edgePoints[0] = position;
        }
        else if (position.x > max.x) {
            max.x = position.x;
            edgePoints[1] = position;
        }

        if (position.y < min.y) {
            min.y = position.y;
            edgePoints[2] = position;
        }
        else if (position.y > max.y) {
            max.y = position.y;
            edgePoints[3] = position;
        }

        if (position.z < min.z) {
            min.z = position.z;
            edgePoints[4] = position;
        }
        else if (position.z > max.z) {
            max.z = position.z;
            edgePoints[5] = position;
        }
    }

    glm::vec3 center = glm::vec3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);

    float furthestDistance = glm::distance(center, edgePoints[0]);

    //Find the furthest point of the mesh from the center
    for (int i = 1; i < 6; i++) {
        float distance = glm::distance(center, edgePoints[i]);

        if (distance > furthestDistance) {
            furthestDistance = distance;
        }
    }

    radius = furthestDistance;
    //Set scale to match the radius of the sphere
    transform->SetScale(2.0f * radius * glm::vec3(1.0f, 1.0f, 1.0f));
    SetOffset(center);
    Update();
}

#pragma endregion

#pragma region Helper Methods

bool SphereCollider::ContainsPoint(glm::vec3 point)
{
    //Check distance to point
    float distance = glm::distance(transform->GetPosition(), point);

    return distance <= radius;
}

glm::vec3 SphereCollider::ClosestToPoint(glm::vec3 point)
{
    //Get direction to point
    glm::vec3 direction = point - transform->GetPosition();
    float distance = glm::distance(point, transform->GetPosition());

    //Make sure the point isn't inside of the collider
    if (distance < radius) {
        return point;
    }
    
    //Normalize direction
    direction = direction / distance;

    //Scale to radius
    direction = direction * radius;

    return transform->GetPosition() + direction;
}

ProjectionData SphereCollider::ProjectOntoAxis(glm::vec3 axis)
{
    ProjectionData data = {};
    float projectionMult = glm::dot(transform->GetPosition(), axis) / glm::dot(axis, axis);

    data.minMax = glm::vec2(projectionMult - radius, projectionMult + radius);
    data.minPoints = std::vector<glm::vec3>();
    data.minPoints.push_back(transform->GetPosition() + axis * data.minMax.x);
    data.maxPoints = std::vector<glm::vec3>();
    data.maxPoints.push_back(transform->GetPosition() + axis * data.minMax.y);

    return data;
}

glm::vec3 SphereCollider::FindSurfaceNormal(glm::vec3 surfacePoint)
{
    return glm::normalize(surfacePoint - transform->GetPosition());
}

void SphereCollider::DrawHandles()
{
    DebugManager::GetInstance()->DrawWireSphere(transform->GetPosition(), glm::vec3(1.0f, 1.0f, 0.0f), radius, 0.0f);
}

#pragma endregion
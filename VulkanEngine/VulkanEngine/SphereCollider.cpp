#include "pch.h"
#include "SphereCollider.h"

#include "EntityManager.h"

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

#pragma region Collision Detection

bool SphereCollider::CheckCollision(Collider* other)
{
    glm::vec3 center = GetParentTransform()->GetPosition() + GetOffset();

    glm::vec3 closest = other->ClosestToPoint(center);

    return glm::distance(center, closest) <= radius;
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
    //Set scale to counteract parent scaling and match the radius of the sphere
    transform->SetScale(2.0f * radius * glm::vec3(1.0f, 1.0f, 1.0f));
    SetOffset(center);
    Update();
}

#pragma endregion

#pragma region Helper Methods

bool SphereCollider::ContainsPoint(glm::vec3 point)
{
    //Find the global center of the collider
    glm::vec3 center = GetParentTransform()->GetPosition() + GetOffset();

    //Check distance to point
    float distance = glm::distance(center, point);

    return distance <= radius;
}

glm::vec3 SphereCollider::ClosestToPoint(glm::vec3 point)
{
    //Find global center of the collider
    glm::vec3 center = GetParentTransform()->GetPosition() + GetOffset();

    //Get direction to point
    glm::vec3 direction = glm::normalize(point - center);

    //Scale to radius
    direction = direction * radius;

    return center + direction;
}

void SphereCollider::ToggleVisible(bool visible)
{
    if (visible) {
        entityID = EntityManager::GetInstance()->GetMeshes()[MeshTypes::SphereCollider]->AddInstance(transform);
    }
    else {
        EntityManager::GetInstance()->GetMeshes()[MeshTypes::SphereCollider]->RemoveInstance(entityID);
    }
}

#pragma endregion
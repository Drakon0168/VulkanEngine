#include "pch.h"
#include "AABBCollider.h"

#include "EntityManager.h"

#pragma region Constructor

AABBCollider::AABBCollider(glm::vec3 extents)
{
    this->extents = extents;

    colliderType = ColliderTypes::AABB;
}

#pragma endregion

#pragma region Accessors

glm::vec3 AABBCollider::GetExtents()
{
    return extents;
}

void AABBCollider::SetExtents(glm::vec3 value)
{
    extents = value;
}

#pragma endregion

#pragma region Collision Detection

bool AABBCollider::CheckCollision(Collider* other)
{
    glm::vec3 closest = other->ClosestToPoint(transform->GetPosition());
    return ContainsPoint(closest);
}

#pragma endregion

#pragma region GenerateFromMesh

void AABBCollider::GenerateFromMesh(std::shared_ptr<Mesh> mesh)
{
    //Find the center of the mesh
    std::vector<Vertex> vertices = mesh->GetVertices();
    glm::vec3 position = GetParentTransform()->GetOrientation() * vertices[0].position * GetParentTransform()->GetScale();
    glm::vec3 max = position;
    glm::vec3 min = position;

    for (int i = 1; i < vertices.size(); i++) {
        position = GetParentTransform()->GetOrientation() * vertices[i].position * GetParentTransform()->GetScale();

        if (position.x < min.x) {
            min.x = position.x;
        }
        else if (position.x > max.x) {
            max.x = position.x;
        }

        if (position.y < min.y) {
            min.y = position.y;
        }
        else if (position.y > max.y) {
            max.y = position.y;
        }

        if (position.z < min.z) {
            min.z = position.z;
        }
        else if (position.z > max.z) {
            max.z = position.z;
        }
    }

    glm::vec3 center = glm::vec3(min.x + max.x, min.y + max.y, min.z + max.z) * 0.5f;
    extents = max - center;

    //Set scale to match the extents of the collider
    transform->SetScale(2.0f * extents);
    SetOffset(center);
    Update();
}

#pragma endregion

#pragma region Helper Methods

bool AABBCollider::ContainsPoint(glm::vec3 point)
{
    glm::vec3 distance = point - transform->GetPosition();
    return (abs(distance.x) < extents.x && abs(distance.y) < extents.y && abs(distance.z) < extents.z);
}

glm::vec3 AABBCollider::ClosestToPoint(glm::vec3 point)
{
    glm::vec3 closest;
    glm::vec3 distance = point - transform->GetPosition();

    //Check x axis
    if (distance.x < -extents.x) {
        closest.x = -extents.x;
    }
    else if (distance.x > extents.x) {
        closest.x = extents.x;
    }
    else {
        closest.x = distance.x;
    }

    //Check y axis
    if (distance.y < -extents.y) {
        closest.y = -extents.y;
    }
    else if (distance.y > extents.y) {
        closest.y = extents.y;
    }
    else {
        closest.y = distance.y;
    }

    //Check z axis
    if (distance.z < -extents.z) {
        closest.z = -extents.z;
    }
    else if (distance.z > extents.z) {
        closest.z = extents.z;
    }
    else {
        closest.z = distance.z;
    }

    closest += transform->GetPosition();
    return closest;
}

void AABBCollider::ToggleVisible(bool visible)
{
    if (visible) {
        entityID = EntityManager::GetInstance()->GetMeshes()[MeshTypes::CubeCollider]->AddInstance(transform);
    }
    else {
        EntityManager::GetInstance()->GetMeshes()[MeshTypes::CubeCollider]->RemoveInstance(entityID);
    }
}

#pragma endregion
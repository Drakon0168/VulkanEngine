#include "pch.h"
#include "ARBBCollider.h"

#include "EntityManager.h"

#pragma region Accessors

glm::vec3 ARBBCollider::GetExtents()
{
    return extents;
}

void ARBBCollider::SetExtents(glm::vec3 value)
{
    extents = value;
}

#pragma endregion

#pragma region Collision Detection

bool ARBBCollider::CheckCollision(Collider* other)
{
    glm::vec3 closest = other->ClosestToPoint(transform->GetPosition());
    return ContainsPoint(closest);
}

#pragma endregion

#pragma region Collider Generation

void ARBBCollider::GenerateFromMesh(std::shared_ptr<Mesh> mesh)
{
    //Find the center of the mesh
    std::vector<Vertex> vertices = mesh->GetVertices();
    glm::vec3 position = vertices[0].position * GetParentTransform()->GetScale();
    glm::vec3 max = position;
    glm::vec3 min = position;

    for (int i = 1; i < vertices.size(); i++) {
        position = vertices[i].position * GetParentTransform()->GetScale();

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

#pragma region Update

void ARBBCollider::Update()
{
    if (parent != nullptr) {
        transform->SetPosition(parent->GetPosition() + GetOffset());
        transform->SetOrientation(parent->GetOrientation());
    }
}

#pragma endregion

#pragma Helper Methods

bool ARBBCollider::ContainsPoint(glm::vec3 point)
{
    glm::vec3 localPoint = ConvertToLocalSpace(point);
    return (abs(localPoint.x) < extents.x && abs(localPoint.y) < extents.y && abs(localPoint.z) < extents.z);
}

glm::vec3 ARBBCollider::ClosestToPoint(glm::vec3 point)
{
    //Convert to local space
    glm::vec3 localPoint = ConvertToLocalSpace(point);

    //Find closest point within extents
    glm::vec3 closestPoint;

    if (localPoint.x > extents.x) {
        closestPoint.x = extents.x;
    }
    else if (localPoint.x < -extents.x) {
        closestPoint.x = -extents.x;
    }
    else {
        closestPoint.x = localPoint.x;
    }

    if (localPoint.y > extents.y) {
        closestPoint.y = extents.y;
    }
    else if (localPoint.y < -extents.y) {
        closestPoint.y = -extents.y;
    }
    else {
        closestPoint.y = localPoint.y;
    }

    if (localPoint.z > extents.z) {
        closestPoint.z = extents.z;
    }
    else if (localPoint.z < -extents.z) {
        closestPoint.z = -extents.z;
    }
    else {
        closestPoint.z = localPoint.z;
    }

    //Convert back to global space
    return ConvertToGlobalSpace(closestPoint);
}

glm::vec3 ARBBCollider::ConvertToLocalSpace(glm::vec3 point)
{
    //Find distance from local origin
    glm::vec3 distance = point - transform->GetPosition();

    //Find Local Axis
    glm::vec3 localAxis[3] = {
        transform->GetOrientation() * glm::vec3(1, 0, 0),
        transform->GetOrientation() * glm::vec3(0, 1, 0),
        transform->GetOrientation() * glm::vec3(0, 0, 1)
    };

    //Project onto local axis
    glm::vec3 localPoint = glm::vec3(0, 0, 0);
    
    for (int i = 0; i < 3; i++) {
        localPoint[i] = glm::dot(distance, localAxis[i]) / glm::dot(localAxis[i], localAxis[i]);
    }

    return localPoint;
}

glm::vec3 ARBBCollider::ConvertToGlobalSpace(glm::vec3 point)
{
    //Find local axis
    glm::vec3 localAxis[3] = {
        transform->GetOrientation() * glm::vec3(1, 0, 0),
        transform->GetOrientation() * glm::vec3(0, 1, 0),
        transform->GetOrientation() * glm::vec3(0, 0, 1)
    };

    return transform->GetPosition() + localAxis[0] * point.x + localAxis[1] * point.y + localAxis[2] * point.z;
}

void ARBBCollider::ToggleVisible(bool visible)
{
    if (visible) {
        entityID = EntityManager::GetInstance()->GetMeshes()[MeshTypes::CubeCollider]->AddInstance(transform);
    }
    else {
        EntityManager::GetInstance()->GetMeshes()[MeshTypes::CubeCollider]->RemoveInstance(entityID);
    }
}

#pragma endregion
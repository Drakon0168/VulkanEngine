#include "pch.h"
#include "Collider.h"

#pragma region Accessors

Collider::Collider()
{
    parent = nullptr;
    transform = std::make_shared<Transform>();
    offset = glm::vec3(0.0f, 0.0f, 0.0f);
}

std::shared_ptr<Transform> Collider::GetParentTransform()
{
    return parent;
}

void Collider::SetParentTransform(std::shared_ptr<Transform> value)
{
    parent = value;
}

std::shared_ptr<Transform> Collider::GetTransform()
{
    return transform;
}

void Collider::SetTransform(std::shared_ptr<Transform> value)
{
    transform = value;
}

glm::vec3 Collider::GetOffset()
{
    //Return rotated offset
    if (parent != nullptr) {
        return parent->GetOrientation() * offset;
    }

    return offset;
}

void Collider::SetOffset(glm::vec3 value)
{
    offset = value;
}

#pragma endregion

#pragma region Update

void Collider::Update()
{
    if (parent != nullptr) {
        transform->SetPosition(parent->GetPosition() + GetOffset());
    }
}

#pragma endregion
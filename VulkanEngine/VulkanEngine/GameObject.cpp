#include "pch.h"
#include "GameObject.h"

#include "DebugManager.h"
#include "EntityManager.h"
#include "PhysicsManager.h"

#pragma region Constructor

GameObject::GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Transform> transform)
{
	this->mesh = mesh;
	this->transform = transform;
	
	physicsObject = nullptr;
	instanceId = -1;
	active = false;
	name = (char*)"gameObject";
}

#pragma endregion

#pragma region Accessors

std::shared_ptr<Transform> GameObject::GetTransform()
{
	return transform;
}

void GameObject::SetTransform(std::shared_ptr<Transform> value)
{
	transform = value;

	if (physicsObject != nullptr) {
		physicsObject->SetTransform(value);
	}
}

std::shared_ptr<PhysicsObject> GameObject::GetPhysicsObject()
{
	return physicsObject;
}

void GameObject::SetPhysicsObject(std::shared_ptr<PhysicsObject> value)
{
	physicsObject = value;
}

void GameObject::SetPhysicsObject(PhysicsLayers layer, ColliderTypes::ColliderTypes colliderType, float mass, bool affectedByGravity, bool alive)
{
	int ID = PhysicsManager::GetInstance()->AddPhysicsObject(transform, mesh, layer, colliderType, mass, affectedByGravity, alive);
	physicsObject = PhysicsManager::GetInstance()->GetPhysicsObject(ID);
	physicsObject->SetGameObject(this);
}

std::shared_ptr<Mesh> GameObject::GetMesh()
{
	return mesh;
}

bool GameObject::GetActive()
{
	return active;
}

char* GameObject::GetName()
{
	return name;
}

void GameObject::SetName(const char* value)
{
	name = (char*)value;
}

#pragma endregion

#pragma region Spawning

void GameObject::Init()
{
}

void GameObject::Spawn()
{
	if (transform == nullptr) {
		transform = std::make_shared<Transform>();
	}
	
	if (physicsObject != nullptr) {
		physicsObject->SetAlive(true);
	}

	instanceId = mesh->AddInstance(transform);
	active = true;
}

void GameObject::Despawn()
{
	if (physicsObject != nullptr) {
		physicsObject->SetAlive(false);
	}

	mesh->RemoveInstance(instanceId);
	instanceId = -1;
	active = false;
}

void GameObject::OnCollision(GameObject* other)
{
	std::cout << name << " collided with " << other->GetName() << std::endl;
}

#pragma endregion

#pragma region Update

void GameObject::Update()
{
	if (active && DebugManager::GetInstance()->GetDrawHandles()) {
		transform->DrawHandles();
	}
}

#pragma endregion
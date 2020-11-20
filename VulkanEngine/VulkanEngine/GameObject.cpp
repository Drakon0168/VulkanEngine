#include "pch.h"
#include "GameObject.h"

#include "DebugManager.h"
#include "EntityManager.h"
#include "PhysicsManager.h"

#pragma region Constructor

GameObject::GameObject(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
	physicsObject = nullptr;
	transform = nullptr;

	instanceId = -1;
	active = false;
}

#pragma endregion

#pragma region Accessors

std::shared_ptr<Transform> GameObject::GetTransform()
{
	if (transform == nullptr) {
		transform = GetComponent<Transform>();
	}

	return transform;
}

std::shared_ptr<PhysicsObject> GameObject::GetPhysicsObject()
{
	return physicsObject;
}

void GameObject::SetPhysicsObject(PhysicsLayers layer, ColliderTypes::ColliderTypes colliderType, float mass, bool affectedByGravity, bool alive)
{
	int ID = PhysicsManager::GetInstance()->AddPhysicsObject(GetTransform(), mesh, layer, colliderType, mass, affectedByGravity, alive);
	physicsObject = PhysicsManager::GetInstance()->GetPhysicsObject(ID);
	physicsObject->SetGameObject(this);
	AddComponent<PhysicsObject>(physicsObject);
}

std::shared_ptr<Mesh> GameObject::GetMesh()
{
	return mesh;
}

std::string GameObject::GetName()
{
	return name;
}

void GameObject::SetName(std::string value)
{
	name = value;
}

bool GameObject::GetActive()
{
	return active;
}

void GameObject::RemoveComponent(int ID)
{
	components[ID]->SetGameObject(nullptr);
	components[ID]->SetID(-1);
	components[ID] = nullptr;
}

#pragma endregion

#pragma region Spawning

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

	Init();
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
	//std::cout << name << " collided with " << other->GetName() << std::endl;
}

#pragma endregion

#pragma region Update

void GameObject::Init()
{
	for (int i = 0; i < components.size(); i++) {
		components[i]->Init();
	}
}

void GameObject::Update()
{
	for (int i = 0; i < components.size(); i++) {
		components[i]->Update();
	}
}

#pragma endregion
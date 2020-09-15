#include "pch.h"
#include "PhysicsObject.h"

#include "PhysicsManager.h"

#include "Collider.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "ARBBCollider.h"

#pragma region Constructor

PhysicsObject::PhysicsObject(std::shared_ptr<Transform> transform, PhysicsLayers physicsLayer, ColliderTypes::ColliderTypes colliderType, float mass, bool affectedByGravity, bool alive)
{
	this->transform = transform;
	this->mass = mass;
	this->affectedByGravity = affectedByGravity;
	this->physicsLayer = physicsLayer;
	this->alive = alive;

	velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

	switch (colliderType) {
	case ColliderTypes::Sphere:
		collider = std::make_shared<SphereCollider>();
		break;
	case ColliderTypes::AABB:
		collider = std::make_shared<AABBCollider>();
		break;
	case ColliderTypes::ARBB:
		collider = std::make_shared<ARBBCollider>();
		break;
	}

	collider->SetParentTransform(transform);
}

#pragma endregion

#pragma region Accessors

float PhysicsObject::GetMass()
{
	return mass;
}

void PhysicsObject::SetMass(float value)
{
	mass = value;
}

glm::vec3 PhysicsObject::GetAcceleration()
{
	return acceleration;
}

glm::vec3 PhysicsObject::GetVelocity()
{
	return velocity;
}

void PhysicsObject::SetVelocity(glm::vec3 value)
{
	velocity = value;
}

PhysicsLayers PhysicsObject::GetPhysicsLayer()
{
	return physicsLayer;
}

std::shared_ptr<Transform> PhysicsObject::GetTransform()
{
	return transform;
}

void PhysicsObject::SetTransform(std::shared_ptr<Transform> value)
{
	transform = value;
}

bool PhysicsObject::GetAlive()
{
	return alive;
}

void PhysicsObject::SetAlive(bool value)
{
	alive = value;
}

std::shared_ptr<Collider> PhysicsObject::GetCollider()
{
	return collider;
}

#pragma endregion

#pragma region Physics

void PhysicsObject::ApplyForce(glm::vec3 force, bool applyMass)
{
	if (applyMass) {
		force /= mass;
	}

	acceleration += force;
}

#pragma endregion

#pragma region Update

void PhysicsObject::Update()
{
	if (alive) {
		if (affectedByGravity) {
			ApplyForce(PhysicsManager::GetInstance()->GetGravity() * PhysicsManager::GetInstance()->GetGravityDirection());
		}

		//Apply acceleration
		velocity += acceleration * Time::GetDeltaTime();
		acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

		//Apply velocity
		transform->Translate(velocity * Time::GetDeltaTime());
	}

	//Update collider
	if (collider != nullptr) {
		collider->Update();
	}
}

#pragma endregion
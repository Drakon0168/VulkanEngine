#include "pch.h"
#include "PhysicsObject.h"

#include "DebugManager.h"
#include "PhysicsManager.h"

#define DebugInstance DebugManager::GetInstance()

#pragma region Constructor

PhysicsObject::PhysicsObject(std::shared_ptr<Transform> transform, PhysicsLayers physicsLayer, float mass, bool affectedByGravity, bool alive)
{
	this->transform = transform;
	this->mass = mass;
	this->affectedByGravity = affectedByGravity;
	this->physicsLayer = physicsLayer;
	this->alive = alive;

	velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
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

#pragma endregion

#pragma Physics

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

		//TODO: remove this once actual collisions are working
		glm::vec3 position = transform->GetPosition();
		if (position.y <= 0.5f) {
			velocity = glm::vec3(velocity.x, velocity.y * -0.8f, velocity.z);
			transform->SetPosition(glm::vec3(position.x, 0.5f, position.z));
		}
	}

	if (DebugInstance->GetDrawHandles()) {
		DrawHandles();
	}
}

void PhysicsObject::DrawHandles()
{
	//Draw velocity
	DebugInstance->DrawLine(transform->GetPosition(), transform->GetPosition() + velocity, glm::vec3(1.0f, 1.0f, 0.0f), 0.0f);
	//Draw acceleration
	DebugInstance->DrawLine(transform->GetPosition() + velocity, transform->GetPosition() + velocity + acceleration, glm::vec3(1.0f, 0.0f, 0.0f), 0.0f);

	//TODO: Draw collider when this is merged with Physics Branch

	transform->DrawHandles();
}

#pragma endregion
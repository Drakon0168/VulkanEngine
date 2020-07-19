#include "pch.h"
#include "PhysicsObject.h"

#include "PhysicsManager.h"

#pragma region Constructor

PhysicsObject::PhysicsObject(std::shared_ptr<Transform> transform, float mass, bool affectedByGravity)
{
	this->transform = transform;
	this->mass = mass;
	this->affectedByGravity = true;

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

#pragma endregion

#pragma Physics

void PhysicsObject::ApplyForce(glm::vec3 force, bool applyMass)
{
	if (applyMass) {
		force /= mass;
	}

	acceleration += force * Time::GetDeltaTime();
}

#pragma endregion

#pragma region Update

void PhysicsObject::Update()
{
	if (affectedByGravity) {
		ApplyForce(PhysicsManager::GetInstance()->GetGravity() * PhysicsManager::GetInstance()->GetGravityDirection());
	}

	//Apply acceleration
	velocity += acceleration;
	acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

	//Apply velocity
	transform->Translate(velocity * Time::GetDeltaTime());
}

#pragma endregion
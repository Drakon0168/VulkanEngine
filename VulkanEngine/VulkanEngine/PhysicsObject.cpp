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
	angularVelocity = glm::quat(glm::vec3(0, 0, 0));
	angularAcceleration = glm::quat(glm::vec3(0, 0, 0));

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

glm::vec3 PhysicsObject::GetVelocityAtPoint(glm::vec3 point)
{
	glm::vec3 direction = point - transform->GetPosition();
	float distance = direction.length();
	direction /= distance;
	float angle = 2 * glm::acos(angularVelocity.w);
	glm::vec3 axis;

	if (angle != 0) {
		axis = glm::vec3(angularVelocity.x, angularVelocity.y, angularVelocity.z) / glm::sin(angle / 2);
	}
	else { //There is no angular velocity to deal with
		return velocity;
	}

	glm::vec3 tangentDirection = glm::cross(axis, direction);
	glm::vec3 pointVelocity = tangentDirection * distance * angle;

	return velocity + pointVelocity;
}

void PhysicsObject::SetVelocity(glm::vec3 value)
{
	velocity = value;
}

glm::quat PhysicsObject::GetAngularVelocity()
{
	return angularVelocity;
}

void PhysicsObject::SetAngularVelocity(glm::quat value)
{
	angularVelocity = value;
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
	if (physicsLayer == PhysicsLayers::Static) {
		std::cout << "Tried to apply force to static object" << std::endl;
		return;
	}

	if (applyMass) {
		force /= mass;
	}

	acceleration += force;
}

void PhysicsObject::ApplyForce(glm::vec3 force, glm::vec3 point, bool applyMass)
{
	if (physicsLayer == PhysicsLayers::Static) {
		std::cout << "Tried to apply force to static object" << std::endl;
		return;
	}

	if (applyMass) {
		force /= mass;
	}

	glm::vec3 direction = point - transform->GetPosition();
	glm::vec3 axis = glm::normalize(glm::cross(-direction, force));
	glm::vec3 perpendicularForce = force - ((glm::dot(direction, force) / glm::dot(direction, direction)) * direction);
	float angularForce = perpendicularForce.length() * direction.length();

	angularAcceleration *= glm::angleAxis(angularForce, axis);
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

		angularVelocity = glm::slerp(angularVelocity, angularVelocity * angularVelocity, Time::GetDeltaTime());
		angularAcceleration = glm::quat(glm::vec3(0, 0, 0));

		//Apply velocity
		transform->Translate(velocity * Time::GetDeltaTime());

		glm::quat orientation = transform->GetOrientation();
		transform->Rotate(glm::slerp(orientation, angularVelocity * orientation, Time::GetDeltaTime()));
	}

	//Update collider
	if (collider != nullptr) {
		collider->Update();
	}
}

#pragma endregion
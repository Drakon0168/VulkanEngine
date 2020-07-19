#pragma once
#include "pch.h"

#include "PhysicsObject.h"

class PhysicsManager
{
private:
	static PhysicsManager* instance;

	float gravity = 9.8f;
	glm::vec3 gravityDirection = glm::vec3(0.0f, -1.0f, 0.0f);

	std::vector<std::vector<std::shared_ptr<PhysicsObject>>> physicsObjects;

public:

#pragma region Singleton

	static PhysicsManager* GetInstance();

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the strength of gravity in the physics system
	/// </summary>
	/// <returns>The strength of the force due to gravity</returns>
	float GetGravity();

	/// <summary>
	/// Sets the strength of the gravitational force to the specified value
	/// </summary>
	/// <param name="value">The value to set to</param>
	void SetGravity(float value);

	/// <summary>
	/// Returns the direction of the gravity force
	/// </summary>
	/// <returns>The direction fo the gravity force</returns>
	glm::vec3 GetGravityDirection();

	/// <summary>
	/// Sets the direction of gravity to the specified value
	/// </summary>
	/// <param name="value">The value to set the direction to</param>
	void SetGravityDirection(glm::vec3 value);

#pragma endregion

#pragma region Collision Detection

	/// <summary>
	/// Detects collisions between all of the physics objects
	/// </summary>
	void DetectCollisions();

	/// <summary>
	/// Checks forr a collision between two physics objects
	/// </summary>
	/// <param name="physicsObject1">The first object to check</param>
	/// <param name="physicsObject2">The second object to check</param>
	/// <returns>True if the objects are colliding</returns>
	bool CheckCollision(std::shared_ptr<PhysicsObject> physicsObject1, std::shared_ptr<PhysicsObject> physicsObject2);

#pragma endregion

#pragma region Collision Resolution

	/// <summary>
	/// Resolves a collision between two physics objects
	/// </summary>
	/// <param name="physicsObject1">The first object in the collision</param>
	/// <param name="physicsObject2">The second object in the collision</param>
	void ResolveCollision(std::shared_ptr<PhysicsObject> physicsObject1, std::shared_ptr<PhysicsObject> physicsObject2);

#pragma endregion
};
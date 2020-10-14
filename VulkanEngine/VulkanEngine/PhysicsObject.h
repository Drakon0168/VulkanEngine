#pragma once
#include "pch.h"

#include "Transform.h"

class PhysicsObject
{
private:
	std::shared_ptr<Transform> transform;

	float mass;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	bool affectedByGravity;
	bool alive;
	PhysicsLayers physicsLayer;
public:

#pragma region Constructor

	PhysicsObject(std::shared_ptr<Transform> transform, PhysicsLayers physicsLayer = PhysicsLayers::Dynamic, float mass = 1.0f, bool affectedByGravity = true, bool alive = false);

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the mass of the object
	/// </summary>
	/// <returns>The object's mass</returns>
	float GetMass();

	/// <summary>
	/// Sets the mass of the object to the specified value
	/// </summary>
	/// <param name="value">The value to set the mass to</param>
	void SetMass(float value);

	/// <summary>
	/// Returns the acceleration of the object
	/// </summary>
	/// <returns>The object's acceleration</returns>
	glm::vec3 GetAcceleration();

	/// <summary>
	/// Returns the velocity of the object
	/// </summary>
	/// <returns>The object's velocity</returns>
	glm::vec3 GetVelocity();

	/// <summary>
	/// Sets the velocity of the object to the specified value
	/// </summary>
	/// <param name="value">The value to set velocity to</param>
	void SetVelocity(glm::vec3 value);

	/// <summary>
	/// Returns the physics layer that this object belongs to
	/// </summary>
	/// <returns>The object's physics layer</returns>
	PhysicsLayers GetPhysicsLayer();

	/// <summary>
	/// Returns the transform used by this physics object
	/// </summary>
	/// <returns>The physics object's transform</returns>
	std::shared_ptr<Transform> GetTransform();

	/// <summary>
	/// Sets the object's transform to the specified value
	/// </summary>
	/// <param name="value">The transform to set to</param>
	void SetTransform(std::shared_ptr<Transform> value);

	/// <summary>
	/// Returns whether or not the object is alive (currently part of the physics system)
	/// </summary>
	/// <returns>Whether the object is alive</returns>
	bool GetAlive();

	/// <summary>
	/// Sets whether or not the object is alive (currently part of the physics system)
	/// </summary>
	/// <param name="value">The value to set to</param>
	void SetAlive(bool value);

#pragma endregion

#pragma region Physics

	/// <summary>
	/// Applies the specified force to the object
	/// </summary>
	/// <param name="force">The force to apply</param>
	/// <param namme="applyMass">Whether or not the force is affected by the mass of the object</param>
	void ApplyForce(glm::vec3 force, bool applyMass = true);

#pragma endregion

#pragma region Update

	/// <summary>
	/// Updates the velocity and position of the object
	/// </summary>
	void Update();

	/// <summary>
	/// Draws the physics object's handles
	/// </summary>
	void DrawHandles();

#pragma endregion
};
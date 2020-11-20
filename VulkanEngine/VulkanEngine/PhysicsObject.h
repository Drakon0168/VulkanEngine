#pragma once
#include "pch.h"

#include "Transform.h"
#include "Collider.h"

//Forward declaration to get around circular include
class GameObject;

class PhysicsObject : public Component
{
private:
	std::shared_ptr<Transform> transform;

	float mass;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::quat angularVelocity;
	glm::quat angularAcceleration;

	bool affectedByGravity;
	bool alive;
	PhysicsLayers physicsLayer;
	int ID;

	std::shared_ptr<Collider> collider;

	// std::vector<size_t> dimensions;
	size_t* dimensionArray;

	glm::vec3 colliderColor;

	void SortDimensions(void);
public:
	size_t dimensionCount = 0;

	bool SharesDimension(std::shared_ptr<PhysicsObject> other);

#pragma region Constructor

	PhysicsObject(std::shared_ptr<Transform> transform = nullptr, PhysicsLayers physicsLayer = PhysicsLayers::Dynamic, ColliderTypes::ColliderTypes colliderType = ColliderTypes::ARBB, float mass = 1.0f, bool affectedByGravity = true, bool alive = false);

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
	/// Returns the linear velocity of the object at a point
	/// </summary>
	/// <param name="point">The point to check the velocity at</param>
	/// <returns>The velocity at the specified point</returns>
	glm::vec3 GetVelocityAtPoint(glm::vec3 point);

	/// <summary>
	/// Sets the velocity of the object to the specified value
	/// </summary>
	/// <param name="value">The value to set velocity to</param>
	void SetVelocity(glm::vec3 value);

	/// <summary>
	/// Returns the angular velocity of the object
	/// </summary>
	/// <returns>The objects angular velocity</returns>
	glm::quat GetAngularVelocity();

	/// <summary>
	/// Sets the object's angular velocity
	/// </summary>
	/// <param name="value">The value to set the angular velocity to</param>
	void SetAngularVelocity(glm::quat value);

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

	/// <summary>
	/// Returns the collider used by this physics object
	/// </summary>
	/// <returns>The collider that is used by this physics object</returns>
	std::shared_ptr<Collider> GetCollider();

	void AddDimension(unsigned int d);
	void RemoveDimension(unsigned int d);
	bool ContainsDimension(unsigned int d);
	void SetColliderColor(glm::vec3 c = { 1.0f, 1.0f, 1.0f });

	// std::vector<size_t> GetDimensions();

#pragma endregion

#pragma region Physics

	/// <summary>
	/// Applies the specified force to the object
	/// </summary>
	/// <param name="force">The force to apply</param>
	/// <param namme="applyMass">Whether or not the force is affected by the mass of the object</param>
	void ApplyForce(glm::vec3 force, bool applyMass = true);

	/// <summary>
	/// Applies a force to the object at the specified point
	/// </summary>
	/// <param name="force">The force to apply</param>
	/// <param name="point">The point to apply the force to</param>
	/// <param name="applyMass">Whether or not the force is affected by the mass of the object</param>
	void ApplyForce(glm::vec3 force, glm::vec3 point, bool applyMass = true);

	/// <summary>
	/// Applies an angular force to the object
	/// </summary>
	/// <param name="torque">The torque to apply to the object</param>
	void ApplyTorque(glm::quat torque, bool applyMass = true);

#pragma endregion

#pragma region Component

	/// <summary>
	/// Initializes variables
	/// </summary>
	virtual void Init() override;

	/// <summary>
	/// Updates the velocity and position of the object
	/// </summary>
	virtual void Update() override;

#pragma endregion

#pragma region Debug

	/// <summary>
	/// Draws the physics object's handles
	/// </summary>
	void DrawHandles();

#pragma endregion
};
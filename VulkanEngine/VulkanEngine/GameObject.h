#pragma once
#include "pch.h"

#include "Transform.h"
#include "Mesh.h"
#include "PhysicsObject.h"

class GameObject
{
private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<PhysicsObject> physicsObject;

	std::shared_ptr<Mesh> mesh;
	int instanceId;

	bool active;

	char* name;

public:
#pragma region Constructor

	GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Transform> transform = nullptr);

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the transform that is being used by this game object
	/// </summary>
	/// <returns>The game object's transform</returns>
	std::shared_ptr<Transform> GetTransform();

	/// <summary>
	/// Sets the game object's transform to the specified value
	/// </summary>
	/// <param name="value">The transform to set to</param>
	void SetTransform(std::shared_ptr<Transform> value);

	/// <summary>
	/// Returns the physics object that is being used by this game object
	/// </summary>
	/// <returns>The game object's physics object</returns>
	std::shared_ptr<PhysicsObject> GetPhysicsObject();

	/// <summary>
	/// Sets the game object's physics object
	/// </summary>
	/// <param name="value">The physics object to set to</param>
	void SetPhysicsObject(std::shared_ptr<PhysicsObject> value);

	/// <summary>
	/// Creates a new physics object to set this gameObject's physicsObject to
	/// </summary>
	/// <param name="layer">The physics layer to set the object to, Dynamic by default</param>
	/// <param name="colliderType">The type of collider to use for this object, ARBB by default</param>
	void SetPhysicsObject(PhysicsLayers layer = PhysicsLayers::Dynamic, ColliderTypes::ColliderTypes colliderType = ColliderTypes::ARBB, float mass = 1.0f, bool affectedByGravity = true, bool alive = true);

	/// <summary>
	/// Returns the mesh that is being used by this game object
	/// </summary>
	/// <returns>The gameobject's mesh</returns>
	std::shared_ptr<Mesh> GetMesh();

	/// <summary>
	/// Returns whether this object is active (visible and interactable)
	/// </summary>
	/// <returns>True if the object is active</returns>
	bool GetActive();

	/// <summary>
	/// Returns the object's name
	/// </summary>
	/// <returns>The name of the game object</returns>
	char* GetName();

	/// <summary>
	/// Sets the object's name
	/// </summary>
	/// <param name="">The value to set the name to</param>
	void SetName(const char* value);

#pragma endregion

#pragma region Spawning

	/// <summary>
	/// Initializes the gameobjects variables
	/// </summary>
	virtual void Init();

	/// <summary>
	/// Sets the object to active and spawns it into the entity manager
	/// </summary>
	virtual void Spawn();

	/// <summary>
	/// Sets the object as inactive and despawns it with the entity manager
	/// </summary>
	virtual void Despawn();

	/// <summary>
	/// Called by the physics system when this object collides with another
	/// </summary>
	/// <param name="other">The object that was collided with</param>
	virtual void OnCollision(GameObject* other);

#pragma endregion

#pragma region Update

	/// <summary>
	/// Updates this objects variables once per frame
	/// </summary>
	virtual void Update();

#pragma endregion
};
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

	std::string name;

	bool active;

public:
#pragma region Constructor

	GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Transform> transform = nullptr, std::shared_ptr<PhysicsObject> physicsObject = nullptr);

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
	/// Returns the mesh that is being used by this game object
	/// </summary>
	/// <returns>The gameobject's mesh</returns>
	std::shared_ptr<Mesh> GetMesh();

	/// <summary>
	/// Returns the name of this gameobject
	/// </summary>
	/// <returns>The gameobject's name</returns>
	std::string GetName();

	/// <summary>
	/// Sets the name of this gameobject used to display the object and access it in game manager
	/// </summary>
	/// <param name="value">The string to set the name to</param>
	void SetName(std::string value);

	/// <summary>
	/// Returns whether this object is active (visible and interactable)
	/// </summary>
	/// <returns>True if the object is active</returns>
	bool GetActive();

#pragma endregion

#pragma region Spawning

	/// <summary>
	/// Sets the object to active and spawns it into the entity manager
	/// </summary>
	virtual void Spawn();

	/// <summary>
	/// Sets the object as inactive and despawns it with the entity manager
	/// </summary>
	virtual void Despawn();

#pragma endregion

#pragma region Update

	/// <summary>
	/// Updates this objects variables once per frame
	/// </summary>
	virtual void Update();

#pragma endregion
};
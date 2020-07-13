#pragma once
#include "pch.h"

#include "Transform.h"
#include "Mesh.h"

class GameObject
{
private:
	std::shared_ptr<Transform> transform;

	std::shared_ptr<Mesh> mesh;
	int instanceId;

	float timer;
	bool active;

public:
#pragma region Constructor

	GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Transform> transform = nullptr);

#pragma endregion

#pragma Accessors

	/// <summary>
	/// Returns the transform that is being used by this game object
	/// </summary>
	/// <returns>The gameobject's transform</returns>
	std::shared_ptr<Transform> GetTransform();

	/// <summary>
	/// Sets the gameobject's transform to the specified value
	/// </summary>
	/// <param name="value">The transform to set to</param>
	void SetTransform(std::shared_ptr<Transform> value);

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

#pragma endregion

#pragma region Spawning

	/// <summary>
	/// Sets the object to active and spawns it into the entity manager
	/// </summary>
	void Spawn();

	/// <summary>
	/// Sets the object as inactive and despawns it with the entity manager
	/// </summary>
	void Despawn();

#pragma endregion

#pragma region Update

	/// <summary>
	/// Updates this objects variables once per frame
	/// </summary>
	void Update();

#pragma endregion
};
#pragma once

#include "pch.h"

#include "Mesh.h"

class Collider
{
protected:

	std::shared_ptr<Transform> parent;
	std::shared_ptr<Transform> transform;
	glm::vec3 offset;
	int entityID;

public:

#pragma region Constructor

	Collider();

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// The transform of the object that this collider is attached to
	/// </summary>
	/// <returns>The parent transform of the collider</returns>
	std::shared_ptr<Transform> GetParentTransform();

	/// <summary>
	/// Sets the parent transform of the collider
	/// </summary>
	/// <param name="value">The transform to set the parent to</param>
	void SetParentTransform(std::shared_ptr<Transform> value);

	/// <summary>
	/// Returns the transform that this collider is attached to
	/// </summary>
	/// <returns>The transform that the collider is attached to</returns>
	std::shared_ptr<Transform> GetTransform();

	/// <summary>
	/// Sets the collider's transform to the specified value
	/// </summary>
	/// <param name="value">The value to set the transform to</param>
	void SetTransform(std::shared_ptr<Transform> value);

	/// <summary>
	/// Returns the offset from the parent transform position of the center of this collider
	/// </summary>
	/// <returns>The offset from the parent transform's position of the collider</returns>
	glm::vec3 GetOffset();

	/// <summary>
	/// Sets the offset of the collider
	/// </summary>
	/// <param name="value">The value to set the offset to</param>
	void SetOffset(glm::vec3 value);

#pragma endregion

#pragma region Collision Detection

	/// <summary>
	/// Checks for a collision with another collider and returns true if there is one.
	/// </summary>
	/// <param name="other">The collider to check against</param>
	/// <returns>True if this collider is colliding with the other collider</returns>
	virtual bool CheckCollision(Collider* other) = 0;

#pragma endregion

#pragma region Collider Generation

	/// <summary>
	/// Generates a collider from a mesh
	/// </summary>
	/// <param name="mesh">The mesh to generate the collider from</param>
	virtual void GenerateFromMesh(std::shared_ptr<Mesh> mesh) = 0;

#pragma endregion

#pragma region Update

	/// <summary>
	/// Updates the collider's transform
	/// </summary>
	virtual void Update();

#pragma endregion

#pragma Helper Methods

	/// <summary>
	/// Returns whether or not the specified point is in the collider
	/// </summary>
	/// <param name="point">The point to test</param>
	/// <returns>True if the specified point is inside of the collider</returns>
	virtual bool ContainsPoint(glm::vec3 point) = 0;

	/// <summary>
	/// Returns the closest point on the collider to the specified point
	/// </summary>
	/// <param name="point">The point to check distance to</param>
	/// <returns>The point in the collider closest to the given point</returns>
	virtual glm::vec3 ClosestToPoint(glm::vec3 point) = 0;

	/// <summary>
	/// Makes this collider visible or invisible to the user
	/// </summary>
	/// <param name="visible">Whether to show or hide the collider true to show</param>
	virtual void ToggleVisible(bool visible) = 0;

#pragma endregion
};
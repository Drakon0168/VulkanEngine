#pragma once

#include "pch.h"

#include "Mesh.h"

class Collider
{
protected:

	std::shared_ptr<Transform> parent;
	std::shared_ptr<Transform> transform;
	glm::vec3 offset;
	ColliderTypes::ColliderTypes colliderType;

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

	/// <summary>
	/// Returns the type of collider
	/// </summary>
	/// <returns>The collider type of this collider</returns>
	ColliderTypes::ColliderTypes GetColliderType();

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
	/// Projects the collider onto the given axis and returns the min and max projection points
	/// </summary>
	/// <param name="axis">The normalized axis to project against</param>
	/// <returns>A vector 2 representing the min and max projection points (x * axis = min point, y * axis = max point)</returns>
	virtual ProjectionData ProjectOntoAxis(glm::vec3 axis) = 0;

	/// <summary>
	/// Finds the normal vector of a point on the surface of the collider
	/// </summary>
	/// <param name="surfacePoint">The point on the surface of the collider to check</param>
	/// <returns>The normal vector at that point</returns>
	virtual glm::vec3 FindSurfaceNormal(glm::vec3 surfacePoint) = 0;

	/// <summary>
	/// Draws the collider with the debugger
	/// </summary>
	virtual void DrawHandles() = 0;

#pragma endregion
};
#pragma once

#include "Collider.h"

class AABBCollider : public Collider
{
private:
	glm::vec3 extents;

public:

#pragma region Constructor

	AABBCollider(glm::vec3 extents = glm::vec3(0, 0, 0));

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the extents of the collider
	/// </summary>
	/// <returns>The collider's extents</returns>
	glm::vec3 GetExtents();

	/// <summary>
	/// Sets the extents to the specified value
	/// </summary>
	/// <param name="value">The value to set the extents to</param>
	void SetExtents(glm::vec3 value);

#pragma endregion

#pragma region Collider Generation

	/// <summary>
	/// Generates a collider from a mesh
	/// </summary>
	/// <param name="mesh"></param>
	void GenerateFromMesh(std::shared_ptr<Mesh> mesh) override;

#pragma endregion

#pragma Helper Methods

	/// <summary>
	/// Returns whether or not the specified point is in the collider
	/// </summary>
	/// <param name="point">The point to test</param>
	/// <returns>True if the specified point is inside of the collider</returns>
	bool ContainsPoint(glm::vec3 point) override;

	/// <summary>
	/// Returns the closest point on the collider to the specified point
	/// </summary>
	/// <param name="point">The point to check distance to</param>
	/// <returns>The point in the collider closest to the given point</returns>
	glm::vec3 ClosestToPoint(glm::vec3 point) override;

	/// <summary>
	/// Projects the collider onto the given axis and returns the min and max projection points
	/// </summary>
	/// <param name="axis">The normalized axis to project against</param>
	/// <returns>A vector 2 representing the min and max projection points (x * axis = min point, y * axis = max point)</returns>
	ProjectionData ProjectOntoAxis(glm::vec3 axis) override;

	/// <summary>
	/// Finds the normal vector of a point on the surface of the collider
	/// </summary>
	/// <param name="surfacePoint">The point on the surface of the collider to check</param>
	/// <returns>The normal vector at that point</returns>
	glm::vec3 FindSurfaceNormal(glm::vec3 surfacePoint) override;

	/// <summary>
	/// Draws the collider with the debugger
	/// </summary>
	void DrawHandles() override;

#pragma endregion
};


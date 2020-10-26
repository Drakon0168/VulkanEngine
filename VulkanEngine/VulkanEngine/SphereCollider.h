#pragma once
#include "pch.h"

#include "Mesh.h"
#include "Collider.h"

class SphereCollider : public Collider
{
private:

	float radius;

public:

#pragma region Contructor

	SphereCollider(float radius = 0.0f);

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the radius of the sphere collider
	/// </summary>
	/// <returns>The sphere collider's radius</returns>
	float getRadius();

	/// <summary>
	/// Sets the radius of the sphere collider to the specified value
	/// </summary>
	/// <param name="value">The value to set the radius to</param>
	void SetRadius(float value);

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
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

#pragma region Collision Detection

	/// <summary>
	/// Checks for a collision with another collider and returns true if there is one.
	/// </summary>
	/// <param name="other">The collider to check against</param>
	/// <returns>True if this collider is colliding with the other collider</returns>
	bool CheckCollision(Collider* other) override;

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
	/// Makes this collider visible or invisible to the user
	/// </summary>
	/// <param name="visible">Whether to show or hide the collider true to show</param>
	void ToggleVisible(bool visible) override;

#pragma endregion
};
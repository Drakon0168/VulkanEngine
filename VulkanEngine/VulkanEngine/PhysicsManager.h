#pragma once
#include "pch.h"

#include "PhysicsObject.h"

#include "Collider.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "ARBBCollider.h"

class PhysicsManager
{
private:
	static PhysicsManager* instance;

	float gravity = 9.8f;
	glm::vec3 gravityDirection = glm::vec3(0.0f, -1.0f, 0.0f);

	std::vector<std::vector<std::shared_ptr<PhysicsObject>>> physicsObjects;

public:

#pragma region Singleton

	static PhysicsManager* GetInstance();

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the strength of gravity in the physics system
	/// </summary>
	/// <returns>The strength of the force due to gravity</returns>
	float GetGravity();

	/// <summary>
	/// Sets the strength of the gravitational force to the specified value
	/// </summary>
	/// <param name="value">The value to set to</param>
	void SetGravity(float value);

	/// <summary>
	/// Returns the direction of the gravity force
	/// </summary>
	/// <returns>The direction fo the gravity force</returns>
	glm::vec3 GetGravityDirection();

	/// <summary>
	/// Sets the direction of gravity to the specified value
	/// </summary>
	/// <param name="value">The value to set the direction to</param>
	void SetGravityDirection(glm::vec3 value);

	/// <summary>
	/// Adds a physics object to the list of physics objects
	/// </summary>
	/// <param name="object">The object to add</param>
	void AddPhysicsObject(std::shared_ptr<PhysicsObject> object);

#pragma endregion

#pragma region Update

	/// <summary>
	/// Updates dynamic physics objects and checks for collisions
	/// </summary>
	void Update();

#pragma endregion

#pragma region Collision Detection

	/// <summary>
	/// Detects collisions between all of the physics objects
	/// </summary>
	void DetectCollisions();

	/// <summary>
	/// Checks for a collision between two physics objects
	/// </summary>
	/// <param name="physicsObject1">The first object to check</param>
	/// <param name="physicsObject2">The second object to check</param>
	/// <param name="data">The collision data used to resolve the collision</param>
	/// <returns>True if the objects are colliding</returns>
	bool CheckCollision(std::shared_ptr<PhysicsObject> physicsObject1, std::shared_ptr<PhysicsObject> physicsObject2, CollisionData& data);

	/// <summary>
	/// Checks for a collision between a sphere collider and any other type of collider
	/// </summary>
	/// <param name="sphereCollider">The sphere collider to check</param>
	/// <param name="other">The other collider to check against</param>
	/// <param name="data">The collision data used to resolve the collision</param>
	/// <returns></returns>
	bool CheckSphereCollision(std::shared_ptr<SphereCollider> sphereCollider, std::shared_ptr<Collider> other, CollisionData& data);

	/// <summary>
	/// Checks for a collision between two Axis Aligned Bounding Boxes
	/// </summary>
	/// <param name="collider1">The first collider to check</param>
	/// <param name="collider2">The second collider to check</param>
	/// <param name="data">The collision data used to resolve the collision</param>
	/// <returns>True if the two objects are in collision</returns>
	bool CheckAABBCollision(std::shared_ptr<AABBCollider> collider1, std::shared_ptr<AABBCollider> collider2, CollisionData& data);

	/// <summary>
	/// Uses the separating axis Theorem to check for a collision between two colliders
	/// </summary>
	/// <param name="collider1">The first collider to check</param>
	/// <param name="collider2">The second collider to check</param>
	/// <param name="data">The collision data used to resolve the collision</param>
	/// <returns>True if the two objects are in collision</returns>
	bool SAT(std::shared_ptr<Collider> collider1, std::shared_ptr<Collider> collider2, CollisionData& data);

#pragma endregion

#pragma region Collision Resolution

	/// <summary>
	/// Resolves a collision between two physics objects
	/// </summary>
	/// <param name="physicsObject1">The first object in the collision</param>
	/// <param name="physicsObject2">The second object in the collision</param>
	/// <param name="data">The collision data used to resolve the collision</param>
	void ResolveCollision(std::shared_ptr<PhysicsObject> physicsObject1, std::shared_ptr<PhysicsObject> physicsObject2, CollisionData data);

	/// <summary>
	/// Alters the velocity of two physics objects when they collide
	/// </summary>
	/// <param name="physicsObject1">The first object in the collision</param>
	/// <param name="physicsObject2">The second object in the collision</param>
	void ResolveVelocity(std::shared_ptr<PhysicsObject> physicsObject1, std::shared_ptr<PhysicsObject> physicsObject2, CollisionData data);

#pragma endregion

#pragma region Helper Functions

	/// <summary>
	/// Projects a point onto an edge
	/// </summary>
	/// <param name="point">The point to project</param>
	/// <param name="edge">The edge to project onto</param>
	/// <param name="sizeClamp">whether or not to clamp the point onto the given line segment</param>
	/// <returns>The point projected onto the edge</returns>
	glm::vec3 ProjectPointOnEdge(glm::vec3 point, glm::vec3 edge[2], bool sizeClamp = false);

	/// <summary>
	/// Projects a point onto a face
	/// </summary>
	/// <param name="point">The point to project</param>
	/// <param name="face">The face to project onto</param>
	/// <param name="sizeClamp">Whether or not to clamp the projected point to the bounds of the face</param>
	/// <returns>THe point projected onto the face</returns>
	glm::vec3 ProjectPointOnFace(glm::vec3 point, glm::vec3 face[3], bool sizeClamp = false);

	/// <summary>
	/// Projects an edge onto another edge
	/// </summary>
	/// <param name="edge1">The edge to project</param>
	/// <param name="edge2">The edge to project onto</param>
	/// <param name="sizeClamp">whether or not to clamp the edge onto the given line segment</param>
	/// <returns>The endpoints of the projected edge (glm::vec3[2])</returns>
	glm::vec3* ProjectEdgeOnEdge(glm::vec3 edge1[2], glm::vec3 edge2[2], bool sizeClamp = false);

	/// <summary>
	/// Projects an edge onto a face
	/// </summary>
	/// <param name="edge">The edge to project</param>
	/// <param name="face">The face to project onto</param>
	/// <param name="sizeClamp">Whether or not to clamp the edge onto the bounds of the face</param>
	/// <returns>The endpoints of the edge after being projected (glm::vec3[2])</returns>
	glm::vec3* ProjectEdgeOnFace(glm::vec3 edge[2], glm::vec3 face[3], bool sizeClamp = false);

	/// <summary>
	/// Projects a face onto another face
	/// </summary>
	/// <param name="face1">The face to project</param>
	/// <param name="face2">The face to project onto</param>
	/// <param name="sizeClamp">Whether or not to clamp the bounds of the projected face onto the bounds of the other face</param>
	/// <returns>The points of face1 projected onto face2 (glm::vec3[3])</returns>
	glm::vec3* ProjectFaceOnFace(glm::vec3 face1[3], glm::vec3 face2[3], bool sizeClamp = false);

#pragma endregion
};
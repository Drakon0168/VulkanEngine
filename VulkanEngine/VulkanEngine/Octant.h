#pragma once
#include "pch.h"

#include "Transform.h"
#include "Mesh.h"
#include "PhysicsObject.h"

class Octant
{
private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<PhysicsObject> physicsObject;

	std::shared_ptr<Mesh> mesh;
	int instanceId;
	bool active;

	std::vector<Octant*> children;
	glm::vec3 center;
	float size;

public:
	Octant(unsigned int maxLevel = 2, unsigned int idealEntityCount = 5);
	Octant(std::shared_ptr<Mesh> mesh, 
		glm::vec3 center,
		float size,
		std::shared_ptr<Transform> transform = nullptr, 
		std::shared_ptr<PhysicsObject> physicsObject = nullptr);

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

	void Subdivide();

#pragma endregion

};


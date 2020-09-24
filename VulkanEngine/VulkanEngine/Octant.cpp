#include "pch.h"
#include "Octant.h"

#include "EntityManager.h"
#include "PhysicsManager.h"
Octant::Octant(unsigned int maxLevel, unsigned int idealEntityCount)
{
}

Octant::Octant(std::shared_ptr<Mesh> mesh, glm::vec3 center, float size, std::shared_ptr<Transform> transform, std::shared_ptr<PhysicsObject> physicsObject)
{
	this->mesh = mesh;
	this->center = center;
	this->transform = transform;
	this->physicsObject = physicsObject;
	this->size = size;

	transform = std::make_shared<Transform>(center);
	transform->SetScale(glm::vec3(size, size, size));
	this->transform = transform;
	this->physicsObject = std::make_shared<PhysicsObject>(transform, PhysicsLayers::Static, ColliderTypes::AABB, 1.0f, false, true);
	if (physicsObject != nullptr) {
		PhysicsManager::GetInstance()->AddPhysicsObject(physicsObject);
	}
	active = false;
	Init();
	Spawn();
	
	if (size > 10)
	{
		 Subdivide();
	}


}

void Octant::Init()
{
	physicsObject->GetCollider()->GenerateFromMesh(mesh);
	physicsObject->GetCollider()->ToggleVisible(true);
}

void Octant::Spawn()
{
	if (transform == nullptr) {
		transform = std::make_shared<Transform>();
	}

	if (physicsObject == nullptr) {
		physicsObject = std::make_shared<PhysicsObject>(transform);
		PhysicsManager::GetInstance()->AddPhysicsObject(physicsObject);
	}

	instanceId = mesh->AddInstance(transform);
	physicsObject->SetAlive(true);
	active = true;
}

void Octant::Despawn()
{
	mesh->RemoveInstance(instanceId);
	instanceId = -1;
	physicsObject->SetAlive(false);
	active = false;
}

void Octant::Subdivide()
{
	float newSize = size / 2;
	glm::vec3 newCenter;
	for (int i = 0; i < 8; i++) {
		newCenter = center;
		children.resize(8);
		if (i < 4)						//Children 0, 1, 2, 3
			newCenter.x += newSize / 2.0f;
		else							//Children 4, 5, 6, 7
			newCenter.x -= newSize / 2.0f;

		if (i < 2 || (i > 3 && i < 6))	//Children 0, 1, 4, 5
			newCenter.y += newSize / 2.0f;
		else							//Children 2, 3, 6, 7
			newCenter.y -= newSize / 2.0f;

		if (i % 2 == 0)					//Children 0, 2, 4, 6
			newCenter.z += newSize / 2.0f;
		else							//Children 1, 3, 5, 7
			newCenter.z -= newSize / 2.0f;
		children[i] = new Octant(mesh, newCenter, size / 2);
	}
}

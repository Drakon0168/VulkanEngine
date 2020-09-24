#include "pch.h"
#include "Octant.h"

#include "EntityManager.h"
#include "PhysicsManager.h"

unsigned int Octant::octantCount = 0;

//Octant::Octant(unsigned int maxLevel, unsigned int idealEntityCount)
//{
//}

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
	octantCount++;
	octantId = octantCount;
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
	if (children[0] == nullptr) {

		float newSize = size / 2;
		glm::vec3 newCenter;
		for (unsigned int i = 0; i < 8; i++) {
			newCenter = center;
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
			children[i] = std::make_shared<Octant>(mesh, newCenter, size / 2);
			children[i]->octantId = octantId + i;
		}
	}
}

void Octant::AddObject(std::shared_ptr<PhysicsObject> g)
{
	Octant* temp = GetSmallestContainingOctant(g);
	// If the smallest octant has no children
	 if (temp->children[0] == nullptr) {
	 	// then the object ONLY exists in this octant
	 	g->AddDimension(octantId);
	 }
	else { // otherwise, octant has children
		// check if this object COLLIDES with any of the octant children
		// if it does, then it exists in those dimensions, as well
		for (unsigned int i = 0; i < 8; i++) {
			// object collides with octant
			if (children[i]->IsColliding(g)) {
				// add octant to object's dimensions (at most will be 8)
				g->AddDimension(children[i]->octantId);
			}
		}
	}
}

Octant* Octant::GetSmallestContainingOctant(std::shared_ptr<PhysicsObject> g)
{
	std::shared_ptr<AABBCollider> temp = std::static_pointer_cast<AABBCollider>(g->GetCollider());
	// glm::vec3 min = -temp->GetExtents();
	if (children[0] != nullptr) {
		for (unsigned int i = 0; i < 8; i++) {
			if (children[i]->IsContained(g)) {
				return children[i]->GetSmallestContainingOctant(g);
			}
		}
	}
	return this;
}

bool Octant::IsContained(std::shared_ptr<PhysicsObject> g)
{
	return false;
}

bool Octant::IsColliding(std::shared_ptr<PhysicsObject> g)
{
	return false;
}

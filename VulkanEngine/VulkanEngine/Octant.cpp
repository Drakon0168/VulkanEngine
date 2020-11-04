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

	int physicsObjectID = PhysicsManager::GetInstance()->AddPhysicsObject(transform, mesh, PhysicsLayers::Static, ColliderTypes::AABB, 1.0f, false);
	this->physicsObject = PhysicsManager::GetInstance()->GetPhysicsObject(physicsObjectID);

	active = false;
	octantCount++;
	octantId = octantCount;
	Init();
	Spawn();
	
	if (size > 5)
	{
		 Subdivide();
	}
}

void Octant::Init()
{
	physicsObject->GetCollider()->GenerateFromMesh(mesh);
}

void Octant::Spawn()
{
	if (transform == nullptr) {
		transform = std::make_shared<Transform>();
	}

	if (physicsObject == nullptr) {
		physicsObject = PhysicsManager::GetInstance()->GetPhysicsObject(PhysicsManager::GetInstance()->AddPhysicsObject(transform, mesh));
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

void Octant::AddObject(std::shared_ptr<PhysicsObject> g, std::shared_ptr<Octant> self)
{
	std::shared_ptr<Octant> temp = GetSmallestContainingOctant(g, self);
	// If the smallest octant has no children
	 if (temp->children[0] == nullptr) {
	 	// then the object ONLY exists in this octant
	 	g->AddDimension(temp->octantId);
		// glm::vec3 t = g->GetTransform()->GetPosition();
		// std::cout << "Added object at position: (" << t.x << "," << t.y << "," << t.z << ") with ID: " << temp->octantId << " because there are no children" << std::endl;
	 }
	else { // otherwise, octant has children
		// check if this object COLLIDES with any of the octant children
		// if it does, then it may exist in those dimensions, as well (and the chidren's children)
		AddCollidingChildren(g, temp);
	}
}

void Octant::AddCollidingChildren(std::shared_ptr<PhysicsObject> g, std::shared_ptr<Octant> self)
{
	for (unsigned int i = 0; i < 8; i++) {
		// object collides with octant
		if (self->children[i]->IsColliding(g)) {
			// add octant to object's dimensions (at most will be 8)
			g->AddDimension(self->children[i]->octantId);
			if (self->children[i]->children[0] != nullptr) AddCollidingChildren(g, self->children[i]);
			// glm::vec3 t = g->GetTransform()->GetPosition();
			// std::cout << "Added object at position: (" << t.x << "," << t.y << "," << t.z << ") with ID: " << children[i]->octantId << std::endl;
		}
	}
}

std::shared_ptr<Octant> Octant::GetSmallestContainingOctant(std::shared_ptr<PhysicsObject> g, std::shared_ptr<Octant> self)
{
	if (children[0] != nullptr) {
		for (unsigned int i = 0; i < 8; i++) {
			if (children[i]->IsContained(g)) {
				return children[i]->GetSmallestContainingOctant(g, children[i]);
			}
		}
	}
	return self;
}

bool Octant::IsContained(std::shared_ptr<PhysicsObject> g)
{
	std::shared_ptr<AABBCollider> temp = std::static_pointer_cast<AABBCollider>(g->GetCollider());
	glm::vec3 max = temp->GetExtents() + temp->GetTransform()->GetPosition();
	glm::vec3 min = -temp->GetExtents() + temp->GetTransform()->GetPosition();
	temp = std::static_pointer_cast<AABBCollider>(physicsObject->GetCollider());
	glm::vec3 octMax = temp->GetExtents() + temp->GetTransform()->GetPosition();
	glm::vec3 octMin = -temp->GetExtents() + temp->GetTransform()->GetPosition();

	if (max.x > octMax.x || max.y > octMax.y || max.z > octMax.z) return false;
	if (min.x < octMin.x || min.y < octMin.y || min.z < octMin.z) return false;

	return true;
}

bool Octant::IsColliding(std::shared_ptr<PhysicsObject> g)
{
	std::shared_ptr<AABBCollider> temp = std::static_pointer_cast<AABBCollider>(g->GetCollider());
	glm::vec3 max = temp->GetExtents() + temp->GetTransform()->GetPosition();
	glm::vec3 min = -temp->GetExtents() + temp->GetTransform()->GetPosition();
	temp = std::static_pointer_cast<AABBCollider>(physicsObject->GetCollider());
	glm::vec3 octMax = temp->GetExtents() + temp->GetTransform()->GetPosition();
	glm::vec3 octMin = -temp->GetExtents() + temp->GetTransform()->GetPosition();

	if (max.x > octMin.x && max.y > octMin.y && max.z > octMin.z &&
		min.x < octMax.x && min.y < octMax.y && min.z < octMax.z) return true;
	return false;
}

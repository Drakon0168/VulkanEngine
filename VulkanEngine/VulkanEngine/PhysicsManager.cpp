#include "pch.h"
#include "PhysicsManager.h"

#pragma region Singleton

PhysicsManager* PhysicsManager::instance = nullptr;

PhysicsManager* PhysicsManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new PhysicsManager();
        instance->physicsObjects.resize(PhysicsLayers::PhysicsLayerCount);

        for (size_t i = 0; i < instance->physicsObjects.size(); i++) {
            instance->physicsObjects[i] = std::vector<std::shared_ptr<PhysicsObject>>();
        }
    }

    return instance;
}

#pragma endregion

#pragma region Accessors

float PhysicsManager::GetGravity()
{
    return gravity;
}

void PhysicsManager::SetGravity(float value)
{
    gravity = value;
}

glm::vec3 PhysicsManager::GetGravityDirection()
{
    return gravityDirection;
}

void PhysicsManager::SetGravityDirection(glm::vec3 value)
{
    gravityDirection = value;
}

void PhysicsManager::AddPhysicsObject(std::shared_ptr<PhysicsObject> object)
{
    physicsObjects[object->GetPhysicsLayer()].push_back(object);
}

#pragma endregion

#pragma region Update

void PhysicsManager::Update()
{
    //Update dynamic objects
    for (size_t i = 0; i < physicsObjects[PhysicsLayers::Dynamic].size(); i++) {
        physicsObjects[PhysicsLayers::Dynamic][i]->Update();
    }

    for (size_t i = 0; i < physicsObjects[PhysicsLayers::Static].size(); i++) {
        physicsObjects[PhysicsLayers::Static][i]->Update();
    }

    //Check for collisions
    DetectCollisions();
}

#pragma endregion

#pragma region Collision Detection

void PhysicsManager::DetectCollisions()
{
    CollisionData data = {};

    //Check Dynamic objects against all objects
    for (size_t i = 0; i < physicsObjects[PhysicsLayers::Dynamic].size(); i++) {
        for (size_t j = i + 1; j < physicsObjects[PhysicsLayers::Dynamic].size(); j++) {
            if (CheckCollision(physicsObjects[PhysicsLayers::Dynamic][i], physicsObjects[PhysicsLayers::Dynamic][j], data)) {
                ResolveCollision(physicsObjects[PhysicsLayers::Dynamic][i], physicsObjects[PhysicsLayers::Dynamic][j], data);
            }
        }

        for (size_t j = 0; j < physicsObjects[PhysicsLayers::Static].size(); j++) {
            if (CheckCollision(physicsObjects[PhysicsLayers::Dynamic][i], physicsObjects[PhysicsLayers::Static][j], data)) {
                ResolveCollision(physicsObjects[PhysicsLayers::Dynamic][i], physicsObjects[PhysicsLayers::Static][j], data);
            }
        }

        for (size_t j = 0; j < physicsObjects[PhysicsLayers::Trigger].size(); j++) {
            if (CheckCollision(physicsObjects[PhysicsLayers::Dynamic][i], physicsObjects[PhysicsLayers::Trigger][j], data)) {
                //TODO: Call the trigger's on collide function
            }
        }
    }

    //Check Static objects against triggers
    for (size_t i = 0; i < physicsObjects[PhysicsLayers::Static].size(); i++) {
        for (size_t j = 0; j < physicsObjects[PhysicsLayers::Trigger].size(); j++) {
            if (CheckCollision(physicsObjects[PhysicsLayers::Static][i], physicsObjects[PhysicsLayers::Trigger][j], data)) {
                //TODO: Call the trigger's on collide function
            }
        }
    }
}

bool PhysicsManager::CheckCollision(std::shared_ptr<PhysicsObject> physicsObject1, std::shared_ptr<PhysicsObject> physicsObject2, CollisionData& data)
{
    //If one of the objects is a sphere do the sphere collider check
    if (physicsObject1->GetCollider()->GetColliderType() == ColliderTypes::Sphere) {
        return CheckSphereCollision(std::static_pointer_cast<SphereCollider>(physicsObject1->GetCollider()), physicsObject2->GetCollider(), data);
    }

    if (physicsObject2->GetCollider()->GetColliderType() == ColliderTypes::Sphere) {
        return CheckSphereCollision(std::static_pointer_cast<SphereCollider>(physicsObject2->GetCollider()), physicsObject1->GetCollider(), data);
    }

    //If both objects are AABB do the AABB check
    if (physicsObject1->GetCollider()->GetColliderType() == ColliderTypes::AABB && physicsObject2->GetCollider()->GetColliderType() == ColliderTypes::AABB) {
        return CheckAABBCollision(std::static_pointer_cast<AABBCollider>(physicsObject1->GetCollider()), std::static_pointer_cast<AABBCollider>(physicsObject2->GetCollider()), data);
    }

    //Otherwise do the SAT check
    return SAT(physicsObject1->GetCollider(), physicsObject2->GetCollider(), data);
}

bool PhysicsManager::CheckSphereCollision(std::shared_ptr<SphereCollider> sphereCollider, std::shared_ptr<Collider> other, CollisionData& data)
{
    glm::vec3 closestPoint = other->ClosestToPoint(sphereCollider->GetTransform()->GetPosition());

    if (sphereCollider->ContainsPoint(closestPoint)) {
        data.collisionNormal = closestPoint - sphereCollider->GetTransform()->GetPosition();
        //Normalize collision normal
        float normalLength = glm::distance(closestPoint, sphereCollider->GetTransform()->GetPosition());
        data.collisionNormal = data.collisionNormal / normalLength;
        data.intersectionDistance = sphereCollider->getRadius() - normalLength;
        data.contactPoint = closestPoint + data.collisionNormal * data.intersectionDistance * 0.5f;
        return true;
    }

    return false;
}

bool PhysicsManager::CheckAABBCollision(std::shared_ptr<AABBCollider> collider1, std::shared_ptr<AABBCollider> collider2, CollisionData& data)
{
    if (collider1->GetTransform()->GetPosition().x + collider1->GetExtents().x < collider2->GetTransform()->GetPosition().x - collider2->GetExtents().x ||
        collider1->GetTransform()->GetPosition().x - collider1->GetExtents().x > collider2->GetTransform()->GetPosition().x + collider2->GetExtents().x ||
        collider1->GetTransform()->GetPosition().y + collider1->GetExtents().y < collider2->GetTransform()->GetPosition().y - collider2->GetExtents().y ||
        collider1->GetTransform()->GetPosition().y - collider1->GetExtents().y > collider2->GetTransform()->GetPosition().y + collider2->GetExtents().y || 
        collider1->GetTransform()->GetPosition().z + collider1->GetExtents().z < collider2->GetTransform()->GetPosition().z - collider2->GetExtents().z ||
        collider1->GetTransform()->GetPosition().z - collider1->GetExtents().z > collider2->GetTransform()->GetPosition().z + collider2->GetExtents().z) {
        return false;
    }

    glm::vec3 col1Min = collider1->GetTransform()->GetPosition() - collider1->GetExtents();
    glm::vec3 col1Max = collider1->GetTransform()->GetPosition() + collider1->GetExtents();
    glm::vec3 col2Min = collider1->GetTransform()->GetPosition() - collider1->GetExtents();
    glm::vec3 col2Max = collider1->GetTransform()->GetPosition() + collider1->GetExtents();

    glm::vec3 totalMax = glm::vec3(glm::max(col1Max.x, col2Max.x), glm::max(col1Max.y, col2Max.y), glm::max(col1Max.z, col2Max.z));
    glm::vec3 totalMin = glm::vec3(glm::min(col1Min.x, col2Min.x), glm::min(col1Min.y, col2Min.y), glm::min(col1Min.z, col2Min.z));

    glm::vec3 overlap = (totalMax - totalMin) - (glm::abs(col1Min - col2Min) + abs(col1Max - col2Max));

    data = {};
    //Set contact point as the center of the overlap
    data.contactPoint = totalMin + glm::abs(col1Min - col2Min) + overlap * 0.5f;
    //Set normal as the axis with the least overlap
    data.intersectionDistance = glm::min(overlap.x, glm::min(overlap.y, overlap.z));
    if (data.intersectionDistance == overlap.x) {
        data.collisionNormal = glm::vec3(1, 0, 0);
    }
    else if (data.intersectionDistance == overlap.y) {
        data.collisionNormal = glm::vec3(0, 1, 0);
    }
    else {
        data.collisionNormal = glm::vec3(0, 0, 1);
    }

    return true;
}

bool PhysicsManager::SAT(std::shared_ptr<Collider> collider1, std::shared_ptr<Collider> collider2, CollisionData& data)
{
    std::vector<glm::vec3> SATaxis = std::vector<glm::vec3>();
    SATaxis.push_back(collider1->GetTransform()->GetOrientation() * glm::vec3(1.0f, 0.0f, 0.0f));
    SATaxis.push_back(collider1->GetTransform()->GetOrientation() * glm::vec3(0.0f, 1.0f, 0.0f));
    SATaxis.push_back(collider1->GetTransform()->GetOrientation() * glm::vec3(0.0f, 0.0f, 1.0f));
    SATaxis.push_back(collider2->GetTransform()->GetOrientation() * glm::vec3(1.0f, 0.0f, 0.0f));
    SATaxis.push_back(collider2->GetTransform()->GetOrientation() * glm::vec3(0.0f, 1.0f, 0.0f));
    SATaxis.push_back(collider2->GetTransform()->GetOrientation() * glm::vec3(0.0f, 0.0f, 1.0f));

    for (int i = 0; i < 3; i++) {
        for (int j = 3; j < 6; j++) {
            SATaxis.push_back(glm::cross(SATaxis[i], SATaxis[j]));
        }
    }

    for (int i = 0; i < SATaxis.size(); i++) {
        glm::vec2 projection1 = collider1->ProjectOntoAxis(SATaxis[i]);
        glm::vec2 projection2 = collider2->ProjectOntoAxis(SATaxis[i]);

        if (projection1.y < projection2.x || projection1.x > projection2.y) {
            return false;
        }
    }

    return true;
}

#pragma endregion

#pragma region Collision Resolution

void PhysicsManager::ResolveCollision(std::shared_ptr<PhysicsObject> physicsObject1, std::shared_ptr<PhysicsObject> physicsObject2, CollisionData data)
{
    //Double Dynamic
    if (physicsObject1->GetPhysicsLayer() == PhysicsLayers::Dynamic && physicsObject2->GetPhysicsLayer() == PhysicsLayers::Dynamic) {
        //TODO: scale resolution movement based on velocity and mass
        physicsObject1->GetTransform()->Translate(data.collisionNormal * data.intersectionDistance * -0.5f);
        physicsObject2->GetTransform()->Translate(data.collisionNormal * data.intersectionDistance * 0.5f);
    }
    else { //One Dynamic and One Static
        //Figure out which object is static and which is dynamic
        std::shared_ptr<PhysicsObject> staticObject;
        std::shared_ptr<PhysicsObject> dynamicObject;

        if (physicsObject1->GetPhysicsLayer() == PhysicsLayers::Dynamic) {
            dynamicObject = physicsObject1;
            staticObject = physicsObject2;
        }
        else {
            dynamicObject = physicsObject2;
            staticObject = physicsObject1;
        }

        dynamicObject->GetTransform()->Translate(glm::normalize(dynamicObject->GetTransform()->GetPosition() - data.contactPoint) * data.intersectionDistance);
    }

    //Resolve Velocities after positions have been fixed
    ResolveVelocity(physicsObject1, physicsObject2);

    //TODO: Call both object's on collision methods
}

void PhysicsManager::ResolveVelocity(std::shared_ptr<PhysicsObject> physicsObject1, std::shared_ptr<PhysicsObject> physicsObject2)
{
    //TODO: Calculate elasticity coefficient from physics objects
    float elasticityCoefficient = 0.9f;

    if (physicsObject1->GetPhysicsLayer() == PhysicsLayers::Dynamic && physicsObject2->GetPhysicsLayer() == PhysicsLayers::Dynamic) { //Two Dynamic Objects
        glm::vec3 obj1StartingVelocity = physicsObject1->GetVelocity();
        glm::vec3 obj2StartingVelocity = physicsObject2->GetVelocity();

        //Calculate the coefficient representing the difference in mass between the two objects
        float massCoefficient = physicsObject1->GetMass() / (physicsObject1->GetMass() + physicsObject2->GetMass());

        if (obj1StartingVelocity == glm::vec3(0, 0, 0) || obj2StartingVelocity == glm::vec3(0, 0, 0)) {
            //If one of the objects is still bounce based on the other's velocity
            if (obj1StartingVelocity == glm::vec3(0, 0, 0)) {
                physicsObject1->SetVelocity(obj2StartingVelocity * massCoefficient * elasticityCoefficient);
                physicsObject2->SetVelocity(-obj2StartingVelocity * (1 - massCoefficient) * elasticityCoefficient);
            }
            else {
                physicsObject1->SetVelocity(obj1StartingVelocity * massCoefficient * elasticityCoefficient);
                physicsObject2->SetVelocity(-obj1StartingVelocity * (1 - massCoefficient) * elasticityCoefficient);
            }
            return;
        }
        else {
            //Calculate the vector to reflect off of using the object's velocities scaled by the difference in their mass
            glm::vec3 reflectDirection = obj1StartingVelocity * massCoefficient + obj2StartingVelocity * (1 - massCoefficient);

            glm::vec3 projMult = (1.0f / glm::dot(reflectDirection, reflectDirection)) * reflectDirection;
            glm::vec3 obj1ProjectedVelocity = glm::dot(obj1StartingVelocity, reflectDirection) * projMult;
            glm::vec3 obj2ProjectedVelocity = glm::dot(obj2StartingVelocity, reflectDirection) * projMult;

            physicsObject1->SetVelocity(obj1ProjectedVelocity + (obj1StartingVelocity - obj1ProjectedVelocity) * elasticityCoefficient);
            physicsObject2->SetVelocity(obj2ProjectedVelocity + (obj2StartingVelocity - obj2ProjectedVelocity) * elasticityCoefficient);
        }
    }
    else { //One Dynamic One Static
        //Find which object is Static and which is Dynamic
        std::shared_ptr<PhysicsObject> dynamicObject;
        std::shared_ptr<PhysicsObject> staticObject;

        if (physicsObject1->GetPhysicsLayer() == PhysicsLayers::Dynamic) {
            dynamicObject = physicsObject1;
            staticObject = physicsObject2;
        }
        else {
            dynamicObject = physicsObject2;
            staticObject = physicsObject1;
        }

        //Bounce the dynamic object off of the static object
        //  Find the point of contact on the static object to the dynamic object
        glm::vec3 dynamicPosition = dynamicObject->GetTransform()->GetPosition();
        glm::vec3 startingVelocity = dynamicObject->GetVelocity();
        glm::vec3 contactPoint = staticObject->GetCollider()->ClosestToPoint(dynamicPosition);
        glm::vec3 reflectDirection = contactPoint - dynamicPosition;
        glm::vec3 velocityProjection = (glm::dot(reflectDirection, startingVelocity) / glm::dot(reflectDirection, reflectDirection)) * reflectDirection;
        dynamicObject->SetVelocity(velocityProjection * -elasticityCoefficient + (startingVelocity - velocityProjection));
    }
}

#pragma endregion
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
    glm::vec3 col2Min = collider2->GetTransform()->GetPosition() - collider2->GetExtents();
    glm::vec3 col2Max = collider2->GetTransform()->GetPosition() + collider2->GetExtents();

    glm::vec3 overlapStart = glm::vec3(glm::max(col1Min.x, col2Min.x), glm::max(col1Min.y, col2Min.y), glm::max(col1Min.z, col2Min.z));
    glm::vec3 overlapEnd = glm::vec3(glm::min(col1Max.x, col2Max.x), glm::min(col1Max.y, col2Max.y), glm::min(col1Max.z, col2Max.z));
    glm::vec3 overlap = overlapEnd - overlapStart;

    data = {};
    //Set contact point as the center of the overlap
    data.contactPoint = (overlapStart + overlapEnd) * 0.5f;
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

    //Make sure collision normal points from object 1 to object 2
    if (glm::dot(data.collisionNormal, collider2->GetTransform()->GetPosition() - collider1->GetTransform()->GetPosition()) < 0) {
        data.collisionNormal *= -1;
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

    ProjectionData closestData[2] = {
        ProjectionData(),
        ProjectionData()
    };
    float minOverlap = -1;
    glm::vec3 closestAxis = SATaxis[0];
    float time = Time::GetTotalTime();
    for (int i = 0; i < SATaxis.size(); i++) {
        ProjectionData projection1 = collider1->ProjectOntoAxis(SATaxis[i]);
        ProjectionData projection2 = collider2->ProjectOntoAxis(SATaxis[i]);

        //Calculate overlap
        float overlap = glm::min(projection1.minMax.y, projection2.minMax.y) - glm::max(projection1.minMax.x, projection2.minMax.x);

        //Exit if there is no overlap
        if (overlap <= 0) {
            return false;
        }

        //Compare with min overlap and set min overlap if it hasn't been set yet
        if (minOverlap == -1 || overlap < minOverlap) {
            minOverlap = overlap;
            closestAxis = SATaxis[i];
            closestData[0] = projection1;
            closestData[1] = projection2;
        }
    }

    //Calculate collision data
    data.collisionNormal = closestAxis;

    //Ensure collision normal points from object 1 to object 2
    if (glm::dot(closestAxis, collider2->GetTransform()->GetPosition() - collider1->GetTransform()->GetPosition()) < 0) {
        data.collisionNormal *= -1;
    }

    data.intersectionDistance = minOverlap;

    // Figure out the type of contact
    std::vector<glm::vec3> collisionPoints[2]{
        std::vector<glm::vec3>(),
        std::vector<glm::vec3>()
    };

    if (closestData[0].minMax.x < closestData[1].minMax.x) {
        collisionPoints[0] = closestData[0].maxPoints;
        collisionPoints[1] = closestData[1].minPoints;
    }
    else {
        collisionPoints[0] = closestData[1].maxPoints;
        collisionPoints[1] = closestData[0].minPoints;
    }

    switch (collisionPoints[0].size() + collisionPoints[1].size()) {
    case 2: //Point on Point collision
        data.contactPoint = (collisionPoints[0][0] + collisionPoints[1][0]) * 0.5f;
        break;
    case 3: //Point on Edge collision
    {
        glm::vec3 point;
        glm::vec3 edge[2];

        if (collisionPoints[0].size() == 1) {
            point = collisionPoints[0][0];
            edge[0] = collisionPoints[1][0];
            edge[1] = collisionPoints[1][1];
        }
        else {
            point = collisionPoints[1][0];
            edge[0] = collisionPoints[0][0];
            edge[1] = collisionPoints[0][1];
        }

        data.contactPoint = (point + ProjectPointOnEdge(point, edge, true)) * 0.5f;
        break;
    }
    case 4: //Edge on Edge or Point on Face collision
    {
        if (collisionPoints[0].size() == 2) { //Edge on Edge
            glm::vec3* projectedEdge1 = ProjectEdgeOnEdge(collisionPoints[0].data(), collisionPoints[1].data(), true);
            glm::vec3* projectedEdge2 = ProjectEdgeOnEdge(collisionPoints[1].data(), collisionPoints[0].data(), true);

            data.contactPoint = ((projectedEdge1[0] + projectedEdge1[1]) * 0.5f + (projectedEdge1[0] + projectedEdge1[1]) * 0.5f) * 0.5f;
        }
        else {
            glm::vec3 point;
            glm::vec3 face[3];

            if (collisionPoints[0].size() == 1) {
                point = collisionPoints[0][0];
                face[0] = collisionPoints[1][0];
                face[1] = collisionPoints[1][1];
                face[2] = collisionPoints[1][2];
            }
            else {
                point = collisionPoints[1][0];
                face[0] = collisionPoints[0][0];
                face[1] = collisionPoints[0][1];
                face[2] = collisionPoints[0][2];
            }

            data.contactPoint = (point + ProjectPointOnFace(point, face, true)) * 0.5f;
        }
        break;
    }
    case 5: //Edge on Face collision
    {
        glm::vec3 edge[2];
        glm::vec3 face[3];

        if (collisionPoints[0].size() == 2) {
            edge[0] = collisionPoints[0][0];
            edge[1] = collisionPoints[0][1];
            face[0] = collisionPoints[1][0];
            face[1] = collisionPoints[1][1];
            face[2] = collisionPoints[1][2];
        }
        else {
            edge[0] = collisionPoints[1][0];
            edge[1] = collisionPoints[1][1];
            face[0] = collisionPoints[0][0];
            face[1] = collisionPoints[0][1];
            face[2] = collisionPoints[0][2];
        }

        glm::vec3* projectedEdge = ProjectEdgeOnFace(edge, face, true);

        data.contactPoint = ((projectedEdge[0] + projectedEdge[1]) * 0.5f + (edge[0] + edge[1]) * 0.5f) * 0.5f;
        break;
    }
    default: //Face on Face collision
    {
        glm::vec3* projectedFace1;
        glm::vec3* projectedFace2;

        projectedFace1 = ProjectFaceOnFace(collisionPoints[0].data(), collisionPoints[1].data(), true);
        projectedFace2 = ProjectFaceOnFace(collisionPoints[1].data(), collisionPoints[0].data(), true);

        glm::vec3 midpoints[2] = {
            glm::vec3(0,0,0),
            glm::vec3(0,0,0)
        };
        for (int i = 0; i < 3; i++) {
            midpoints[0] += projectedFace1[i];
            midpoints[1] += projectedFace2[i];
        }

        midpoints[0] /= 3.0f;
        midpoints[1] /= 3.0f;

        data.contactPoint = (midpoints[0] + midpoints[1]) * 0.5f;
        break;
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
        if (physicsObject1->GetPhysicsLayer() == PhysicsLayers::Dynamic) {
            physicsObject1->GetTransform()->Translate(data.collisionNormal * data.intersectionDistance * -1.0f);
        }
        else {
            physicsObject2->GetTransform()->Translate(data.collisionNormal * data.intersectionDistance);
        }
    }

    //Resolve Velocities after positions have been fixed
    ResolveVelocity(physicsObject1, physicsObject2, data);

    //TODO: Call both object's on collision methods
}

void PhysicsManager::ResolveVelocity(std::shared_ptr<PhysicsObject> physicsObject1, std::shared_ptr<PhysicsObject> physicsObject2, CollisionData data)
{
    float projectionMult[2];
    projectionMult[0] = glm::dot(physicsObject1->GetVelocity(), data.collisionNormal) / glm::dot(data.collisionNormal, data.collisionNormal);
    projectionMult[1] = glm::dot(physicsObject2->GetVelocity(), data.collisionNormal) / glm::dot(data.collisionNormal, data.collisionNormal);

    glm::vec3 force[2];
    
    if (physicsObject1->GetPhysicsLayer() == PhysicsLayers::Dynamic && projectionMult[0] > 0) {
        force[0] = (data.collisionNormal * projectionMult[0] * -1.0f) / Time::GetDeltaTime();
        physicsObject1->ApplyForce(force[0]);

        if (physicsObject2->GetPhysicsLayer() == PhysicsLayers::Dynamic) {
            physicsObject2->ApplyForce(-force[0]);
        }
    }

    if (physicsObject2->GetPhysicsLayer() == PhysicsLayers::Dynamic && projectionMult[1] < 0) {
        force[1] = (data.collisionNormal * projectionMult[1] * -1.0f) / Time::GetDeltaTime();
        physicsObject2->ApplyForce(force[1]);

        if (physicsObject1->GetPhysicsLayer() == PhysicsLayers::Dynamic) {
            physicsObject1->ApplyForce(-force[1]);
        }
    }
}

#pragma endregion

#pragma region Helper Functions

glm::vec3 PhysicsManager::ProjectPointOnEdge(glm::vec3 point, glm::vec3 edge[2], bool sizeClamp)
{
    glm::vec3 direction = point - edge[0];
    glm::vec3 edgeDirection = edge[1] - edge[0];
    float projectionMult = glm::dot(edgeDirection, direction) / glm::dot(edgeDirection, edgeDirection);

    if (sizeClamp) {
        projectionMult = glm::clamp(projectionMult, 0.0f, 1.0f);
    }

    return edge[0] + projectionMult * edgeDirection;
}

glm::vec3 PhysicsManager::ProjectPointOnFace(glm::vec3 point, glm::vec3 face[3], bool sizeClamp)
{
    glm::vec3 axis[2];
    axis[0] = face[1] - face[0];
    axis[1] = face[2] - face[0];

    glm::vec3 direction = point - face[0];

    float projectionMult[2];
    projectionMult[0] = glm::dot(axis[0], direction) / glm::dot(axis[0], axis[0]);
    projectionMult[1] = glm::dot(axis[1], direction) / glm::dot(axis[1], axis[1]);

    if (sizeClamp) {
        projectionMult[0] = glm::clamp(projectionMult[0], 0.0f, 1.0f);
        projectionMult[1] = glm::clamp(projectionMult[1], 0.0f, 1.0f);
    }

    return face[0] + axis[0] * projectionMult[0] + axis[1] * projectionMult[1];
}

glm::vec3* PhysicsManager::ProjectEdgeOnEdge(glm::vec3 edge1[2], glm::vec3 edge2[2], bool sizeClamp)
{
    glm::vec3 projectedEdge[2];

    projectedEdge[0] = ProjectPointOnEdge(edge1[0], edge2, sizeClamp);
    projectedEdge[1] = ProjectPointOnEdge(edge1[1], edge2, sizeClamp);

    return projectedEdge;
}

glm::vec3* PhysicsManager::ProjectEdgeOnFace(glm::vec3 edge[2], glm::vec3 face[3], bool sizeClamp)
{
    glm::vec3 projectedEdge[2];

    projectedEdge[0] = ProjectPointOnFace(edge[0], face, sizeClamp);
    projectedEdge[1] = ProjectPointOnFace(edge[1], face, sizeClamp);

    return projectedEdge;
}

glm::vec3* PhysicsManager::ProjectFaceOnFace(glm::vec3 face1[3], glm::vec3 face2[3], bool sizeClamp)
{
    glm::vec3 projectedFace[3];

    for (int i = 0; i < 3; i++) {
        projectedFace[i] = ProjectPointOnFace(face1[0], face2, sizeClamp);
    }

    return projectedFace;
}

#pragma endregion
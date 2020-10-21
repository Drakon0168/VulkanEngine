#include "pch.h"
#include "GameManager.h"

#include "DebugManager.h"
#include "EntityManager.h"
#include "InputManager.h"
#include "Camera.h"
#include "Octant.h"

#pragma region Singleton

GameManager* GameManager::instance = nullptr;

GameManager* GameManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new GameManager();
    }

    return instance;
}

#pragma endregion

#pragma region Accessors

std::vector<std::shared_ptr<Light>> GameManager::GetLights()
{
    return lights;
}

#pragma endregion

#pragma region Game Loop

void GameManager::Init()
{
    Camera::GetMainCamera()->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 50));
    //Setup Lights
    lights.push_back(std::make_shared<Light>(glm::vec3(1.5f, 1.1f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 5.0f));
    lights.push_back(std::make_shared<Light>(glm::vec3(0.0f, 2.0f, -1.5f), glm::vec3(1.0f, 0.988f, 0.769f), 3.0f));
    /**
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Plane]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Sphere]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Model]));
    // gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::CubeCollider]));
    //gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Sphere]));

    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Skybox]));
    
    //Setup Plane
    gameObjects[0]->SetTransform(std::make_shared<Transform>(glm::vec3(0.0f, 0.0f, 0.0f)));
    gameObjects[0]->GetTransform()->SetScale(glm::vec3(5.0f, 1.0f, 5.0f));
    gameObjects[0]->SetPhysicsObject(std::make_shared<PhysicsObject>(gameObjects[0]->GetTransform(), PhysicsLayers::Static, ColliderTypes::ARBB, 1.0f, false, true));

    gameObjects[0]->SetPhysicsObject(std::make_shared<PhysicsObject>(gameObjects[0]->GetTransform(), PhysicsLayers::Static, 1.0f, false, true));
    
    //Setup Cube
    gameObjects[1]->SetTransform(std::make_shared<Transform>(glm::vec3()));
    gameObjects[1]->GetTransform()->SetPosition(glm::vec3(-1.5f, 0, 0));
    gameObjects[1]->GetTransform()->SetOrientation(glm::vec3(0.0f, 45.0f, 0.0f));
    gameObjects[1]->SetPhysicsObject(std::make_shared<PhysicsObject>(gameObjects[1]->GetTransform(), PhysicsLayers::Static, ColliderTypes::ARBB, 1.0f, false, true));

    //Setup Sphere
    gameObjects[2]->SetTransform(std::make_shared<Transform>(glm::vec3(-1.5f, 2.5f, 0.0f)));
    gameObjects[2]->SetPhysicsObject(std::make_shared<PhysicsObject>(gameObjects[2]->GetTransform(), PhysicsLayers::Dynamic, ColliderTypes::ARBB, 1.0f, true, true));

    gameObjects[3]->SetTransform(std::make_shared<Transform>(glm::vec3(0.85f, 2.5f, -1.5f)));
    gameObjects[3]->SetPhysicsObject(std::make_shared<PhysicsObject>(gameObjects[3]->GetTransform(), PhysicsLayers::Dynamic, ColliderTypes::AABB, 1.0f, true, true));

    gameObjects[4]->SetTransform(std::make_shared<Transform>(glm::vec3(1.5f, 2.5f, 0.0f)));
    gameObjects[4]->SetPhysicsObject(std::make_shared<PhysicsObject>(gameObjects[4]->GetTransform(), PhysicsLayers::Dynamic, ColliderTypes::Sphere, 1.0f, true, true));

    //setup model
    gameObjects[5]->SetTransform(std::make_shared<Transform>(glm::vec3(0.0f, 0.1f, -1.5f)));
    gameObjects[5]->GetTransform()->SetOrientation(glm::vec3(-90.0f, -90.0f, 0.0f));
    gameObjects[5]->SetPhysicsObject(std::make_shared<PhysicsObject>(gameObjects[5]->GetTransform(), PhysicsLayers::Static, ColliderTypes::AABB, 1.0f, false, true));

    // setup collider
    // gameObjects[6]->SetTransform(std::make_shared<Transform>(glm::vec3(0.0f, 0.0f, 0.0f)));
    // gameObjects[6]->GetTransform()->SetScale(glm::vec3(20, 20, 20));
    // gameObjects[6]->SetPhysicsObject(std::make_shared<PhysicsObject>(gameObjects[6]->GetTransform(), PhysicsLayers::Static, ColliderTypes::AABB, 1.0f, false, true));
    //Dynamic v Dynamic collision test
    //gameObjects[6]->SetTransform(std::make_shared<Transform>(glm::vec3(1.5f, 5.0f, 0.0f)));
    //gameObjects[6]->SetPhysicsObject(std::make_shared<PhysicsObject>(gameObjects[6]->GetTransform(), PhysicsLayers::Dynamic, ColliderTypes::Sphere, 1.0f, true, true));
    

    */

    // Setup octant
    octree = std::make_shared<Octant>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::CubeCollider], glm::vec3(0.0f, 0.0f, 0.0f), 20.0f);
    
    // octree = new Octant(EntityManager::GetInstance()->GetMeshes()[MeshTypes::CubeCollider], glm::vec3(0.0f, 0.0f, 0.0f), 20.0f);
    for (int i = 0; i < 300; i++) {
        octObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
        octObjects[i]->SetTransform(std::make_shared<Transform>(glm::vec3(rand() % 20 - 10, rand() % 20 - 10, rand() % 20 - 10)));
        octObjects[i]->SetPhysicsObject(std::make_shared<PhysicsObject>(octObjects[i]->GetTransform(), PhysicsLayers::Dynamic, ColliderTypes::AABB, 1.0f, false, true));

        octObjects[i]->Init();
        octObjects[i]->Spawn();
        octObjects[i]->GetPhysicsObject()->GetCollider()->ToggleVisible(false);
        octree->AddObject(octObjects[i]->GetPhysicsObject(), octree);
    }
    // gameObjects[0]->GetPhysicsObject()->GetCollider()
    // octree->AddObject(gameObjects[0]);
    //Initialize GameObjects
    for (size_t i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Init();
        gameObjects[i]->Spawn();
    }

    //Reset time so that it doesn't include initialization in totalTime
    Time::Reset();
}

void GameManager::Update()
{
    //Rotate Camera
    //  Toggle camera lock on right click
    if (InputManager::GetInstance()->GetKeyPressed(Controls::RightClick)) {
        lockCamera = !lockCamera; 
    }

    //  Rotate camera if not locked
    if (!lockCamera) {
        glm::vec2 deltaMouse = InputManager::GetInstance()->GetDeltaMouse();
        if (deltaMouse.x != 0 || deltaMouse.y != 0) {
            deltaMouse = glm::normalize(deltaMouse);
        }

        glm::quat orientation = Camera::GetMainCamera()->GetTransform()->GetOrientation();
        glm::vec3 rotation = orientation * glm::vec3(deltaMouse.y, 0.0f, 0.0f) + glm::vec3(0.0f, -deltaMouse.x, 0.0f);

        Camera::GetMainCamera()->GetTransform()->Rotate(rotation);
    }

    //Move Camera
    glm::vec3 moveDirection = glm::vec3(0.0f, 0.0f, 0.0f);

    if (InputManager::GetInstance()->GetKey(Controls::Forward)) {
        moveDirection += glm::vec3(0.0f, 0.0f, 1.0f);
    }
    if (InputManager::GetInstance()->GetKey(Controls::Back)) {
        moveDirection += glm::vec3(0.0f, 0.0f, -1.0f);
    }
    if (InputManager::GetInstance()->GetKey(Controls::Up)) {
        moveDirection += glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (InputManager::GetInstance()->GetKey(Controls::Down)) {
        moveDirection += glm::vec3(0.0f, -1.0f, 0.0f);
    }
    if (InputManager::GetInstance()->GetKey(Controls::Left)) {
        moveDirection += glm::vec3(1.0f, 0.0f, 0.0f);
    }
    if (InputManager::GetInstance()->GetKey(Controls::Right)) {
        moveDirection += glm::vec3(-1.0f, 0.0f, 0.0f);
    }

    if (moveDirection.x != 0 || moveDirection.y != 0 || moveDirection.z != 0) {
        moveDirection = glm::normalize(moveDirection);
    }

    Camera::GetMainCamera()->GetTransform()->Translate(moveDirection * cameraSpeed * Time::GetDeltaTime(), true);

    //Update Lights
    float scaledTime = Time::GetTotalTime() / 2.5f;
    lights[0]->position = glm::vec3(0.0f, 1.1f, 0.0f) + glm::vec3(cos(scaledTime), 0.0f, sin(scaledTime)) * 1.5f;

    //gameObjects[0]->GetTransform()->Rotate(glm::vec3(0.0f, 10.0f, 0.0f) * Time::GetDeltaTime());
    
    //Update Game Objects
    for (size_t i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Update();
    }

    if (InputManager::GetInstance()->GetKeyPressed(Controls::Jump)) {
        gameObjects[2]->GetPhysicsObject()->ApplyForce(glm::vec3(0.0f, 5000.0f, 0.0f));

        //Spawn Object Sample Code:
        /*
        std::shared_ptr<GameObject> newObject = std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Sphere]);
        gameObjects.push_back(newObject);

        newObject->SetTransform(std::make_shared<Transform>(glm::vec3(0.0f, 2.5f, 0.0f)));
        newObject->SetPhysicsObject(std::make_shared<PhysicsObject>(newObject->GetTransform(), PhysicsLayers::Dynamic, ColliderTypes::Sphere, 1.0f, true, true));

        newObject->Init();
        newObject->Spawn();
        */
    }

    //Update Game Objects
    for (size_t i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Update();
    }
}

#pragma endregion
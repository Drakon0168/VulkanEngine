#include "pch.h"
#include "GameManager.h"

#include "EntityManager.h"
#include "InputManager.h"
#include "Camera.h"

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
    //Setup Lights
    lights.push_back(std::make_shared<Light>(glm::vec3(1.5f, 1.1f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 20.0f));
    lights.push_back(std::make_shared<Light>(glm::vec3(0.0f, 2.0f, -1.5f), glm::vec3(1.0f, 0.988f, 0.769f), 3.0f));

    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Sphere]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Model]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Sphere]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Skybox]));

    //Setup Plane
    gameObjects[0]->SetTransform(std::make_shared<Transform>(glm::vec3(0.0f, 0.0f, 0.0f)));
    gameObjects[0]->GetTransform()->SetScale(glm::vec3(5.0f, 0.1f, 5.0f));
    gameObjects[0]->SetPhysicsObject(PhysicsLayers::Static, ColliderTypes::ARBB, 1.0f, false);
    gameObjects[0]->SetName("Floor");

    //Setup Cube
    gameObjects[1]->SetTransform(std::make_shared<Transform>(glm::vec3(-1.5f, 0.5f, 0.0f)));
    gameObjects[1]->GetTransform()->SetOrientation(glm::vec3(0.0f, 45.0f, 0.0f));
    gameObjects[1]->SetPhysicsObject(PhysicsLayers::Static, ColliderTypes::ARBB, 1.0f, false);
    gameObjects[1]->SetName("StaticCube");

    //Setup Dynamic Objects
    gameObjects[2]->SetTransform(std::make_shared<Transform>(glm::vec3(-1.5f, 2.5f, 0.0f)));
    gameObjects[2]->SetPhysicsObject(PhysicsLayers::Static, ColliderTypes::ARBB, 1.0f, false);
    gameObjects[2]->SetName("FloatingCube");

    gameObjects[3]->SetTransform(std::make_shared<Transform>(glm::vec3(0.95f, 2.5f, -1.5f)));
    gameObjects[3]->SetPhysicsObject();
    gameObjects[3]->SetName("DynamicCube");

    gameObjects[4]->SetTransform(std::make_shared<Transform>(glm::vec3(1.5f, 2.5f, 0.0f)));
    gameObjects[4]->SetPhysicsObject(PhysicsLayers::Dynamic, ColliderTypes::Sphere);
    gameObjects[4]->SetName("DynamicSphere0");

    //setup model
    gameObjects[5]->SetTransform(std::make_shared<Transform>(glm::vec3(0.0f, 0.1f, -1.5f)));
    gameObjects[5]->GetTransform()->SetOrientation(glm::vec3(-90.0f, -90.0f, 0.0f));
    gameObjects[5]->SetPhysicsObject(PhysicsLayers::Static, ColliderTypes::AABB, 1.0f, false);
    gameObjects[5]->SetName("Model");

    //Dynamic v Dynamic collision test
    gameObjects[6]->SetTransform(std::make_shared<Transform>(glm::vec3(1.2f, 5.0f, 0.0f)));
    gameObjects[6]->SetPhysicsObject(PhysicsLayers::Dynamic, ColliderTypes::Sphere);
    gameObjects[6]->SetName("DynamicSphere1");

    // setup skybox
    gameObjects[7]->SetTransform(std::make_shared<Transform>(glm::vec3(0)));
    gameObjects[7]->SetName("Skybox");

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

    if (InputManager::GetInstance()->GetKeyPressed(Controls::Left)) {
        gameObjects[2]->GetPhysicsObject()->ApplyTorque(glm::angleAxis(300.0f, glm::vec3(0, 1, 0)), false);
    }
    if (InputManager::GetInstance()->GetKeyPressed(Controls::Right)) {
        gameObjects[2]->GetPhysicsObject()->ApplyTorque(glm::angleAxis(-300.0f, glm::vec3(0, 1, 0)), false);
    }

    if (InputManager::GetInstance()->GetKeyPressed(Controls::Jump)) {
        gameObjects[2]->GetPhysicsObject()->ApplyForce(glm::vec3(0.0f, 5000.0f, 0.0f));
    }

    //Update Game Objects
    for (size_t i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Update();
    }
}

#pragma endregion
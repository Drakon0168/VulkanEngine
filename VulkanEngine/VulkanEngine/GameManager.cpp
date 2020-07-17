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

#pragma region Game Loop

void GameManager::Init()
{
    //Reset time so that it doesn't include initialization in totalTime
    Time::Reset();

    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Plane]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Sphere]));

    //Setup plane
    gameObjects[0]->SetTransform(std::make_shared<Transform>());
    gameObjects[0]->GetTransform()->SetScale(glm::vec3(5.0f, 1.0f, 5.0f));

    //Setup cube
    gameObjects[1]->SetTransform(std::make_shared<Transform>(glm::vec3(-1.5f, 0.5f, 0.0f)));
    gameObjects[1]->GetTransform()->SetOrientation(glm::vec3(0.0f, 45.0f, 0.0f));

    //Setup Sphere
    gameObjects[2]->SetTransform(std::make_shared<Transform>(glm::vec3(1.0f, 0.5f, 0.0f)));

    for (size_t i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Spawn();
    }
}

void GameManager::Update()
{
    //Rotate Camera
    glm::vec2 deltaMouse = InputManager::GetInstance()->GetDeltaMouse();
    if (deltaMouse.x != 0 || deltaMouse.y != 0) {
        deltaMouse = glm::normalize(deltaMouse);
    }

    glm::quat orientation = Camera::GetMainCamera()->GetTransform()->GetOrientation();
    glm::vec3 rotation = orientation * glm::vec3(deltaMouse.y, 0.0f, 0.0f) + glm::vec3(0.0f, -deltaMouse.x, 0.0f);

    Camera::GetMainCamera()->GetTransform()->Rotate(rotation);

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

    //Update Game Objects
    for (size_t i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Update();
    }
}

#pragma endregion
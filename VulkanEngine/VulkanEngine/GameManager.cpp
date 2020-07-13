#include "pch.h"
#include "GameManager.h"

#include "EntityManager.h"

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

#pragma region GameLoop

void GameManager::Init()
{
    //Reset time so that it doesn't include initialization in totalTime
    Time::Reset();

    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[0]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[1]));
    gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[2]));

    for (size_t i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->SetTransform(std::make_shared<Transform>(glm::vec3(-1.5f + i * 1.5f, 1.0f, 0.0f)));
        gameObjects[i]->Spawn();
    }
}

void GameManager::Update()
{
    for (size_t i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Update();
    }
}

#pragma endregion
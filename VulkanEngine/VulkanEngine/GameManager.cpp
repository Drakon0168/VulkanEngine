#include "pch.h"
#include "GameManager.h"

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
}

void GameManager::Update()
{
    Time::Update();
}

void GameManager::Draw()
{
}

#pragma endregion
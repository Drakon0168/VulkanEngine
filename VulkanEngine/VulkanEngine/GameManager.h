#pragma once
#include "pch.h"

#include "GameObject.h"

class GameManager
{
private:
	static GameManager* instance;

	std::vector<std::shared_ptr<GameObject>> gameObjects;

	float cameraSpeed = 2.5f;
public:
#pragma region Singleton

	static GameManager* GetInstance();

#pragma endregion

#pragma region Game Loop

	void Init();
	void Update();

#pragma endregion
};
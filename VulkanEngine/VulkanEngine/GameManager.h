#pragma once
#include "pch.h"

#include "GameObject.h"

class GameManager
{
private:
	static GameManager* instance;

	std::vector<std::shared_ptr<GameObject>> gameObjects;
public:
#pragma region Singleton

	static GameManager* GetInstance();

#pragma endregion

#pragma region Game Loop

	void Init();
	void Update();

#pragma endregion
};
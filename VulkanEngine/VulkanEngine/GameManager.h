#pragma once
#include "pch.h"

#include "GameObject.h"
#include "Octant.h";

class GameManager
{
private:
	static GameManager* instance;

	std::vector<std::shared_ptr<GameObject>> gameObjects;
	std::vector<std::shared_ptr<GameObject>> octObjects;
	std::vector<std::shared_ptr<Light>> lights;

	std::shared_ptr<Octant> octree = nullptr;

	float cameraSpeed = 2.5f;
	bool lockCamera = true;
public:
#pragma region Singleton

	static GameManager* GetInstance();

#pragma endregion

#pragma region Accessors

	std::vector<std::shared_ptr<Light>> GetLights();

#pragma endregion

#pragma region Game Loop

	void Init();
	void Update();

#pragma endregion
};
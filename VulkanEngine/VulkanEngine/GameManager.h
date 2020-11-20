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
	// Octant* octree = nullptr;

	float cameraSpeed = 2.5f;
	bool lockCamera = true;
public:
#pragma region Singleton

	static GameManager* GetInstance();

#pragma endregion

#pragma region Accessors

	std::vector<std::shared_ptr<Light>> GetLights();

	/// <summary>
	/// Finds a gameobject with the specified name
	/// </summary>
	/// <param name="name">The name of the object to find</param>
	/// <returns>The first object created with that name or null if there is no object found with the specified name</returns>
	std::shared_ptr<GameObject> GetObjectByName(std::string name);

#pragma endregion

#pragma region Game Loop

	/// <summary>
	/// Called before the first update, initializes the gameobjects
	/// </summary>
	void Init();

	/// <summary>
	/// Called once per frame, updates the gameobjects
	/// </summary>
	void Update();

#pragma endregion
};
#pragma once

#include "pch.h"

//Forward declaration of Gameobject to bypass circular includes
class GameObject;

class Component
{
protected:
	GameObject* gameObject;
	int ID;

public:
#pragma region Constructor

	Component();

#pragma endregion

#pragma region Accessors

	GameObject* GetGameObject();

	void SetGameObject(GameObject* value);

	int GetID();

	void SetID(int value);

#pragma endregion

#pragma region Component

	/// <summary>
	/// Called before the first update 
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// Called once per frame used to update the component's variables
	/// </summary>
	virtual void Update() = 0;

#pragma endregion
};
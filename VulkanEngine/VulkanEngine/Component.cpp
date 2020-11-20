#include "pch.h"
#include "Component.h"
#include "GameObject.h"

#pragma region Constructor

Component::Component()
{
	gameObject = nullptr;
	ID = -1;
}

#pragma endregion

#pragma region Accessors

GameObject* Component::GetGameObject()
{
	return gameObject;
}

void Component::SetGameObject(GameObject* value)
{
	gameObject = value;
}

int Component::GetID()
{
	return ID;
}

void Component::SetID(int value)
{
	ID = value;
}

#pragma endregion
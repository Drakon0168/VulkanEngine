#pragma once
#include "pch.h"

class Input
{
private:
	InputStates state;
	int keyCode;

public:

#pragma region Constructor

	Input(int keyCode = 0);

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the current state of the input
	/// </summary>
	/// <returns>The input's current state</returns>
	InputStates GetState();

	/// <summary>
	/// Returns the key that this input is monitoring
	/// </summary>
	/// <returns>The key associated with this input</returns>
	int GetKeyCode();

	/// <summary>
	/// Sets the key that this input uses
	/// </summary>
	/// <param name="value">The key to set to</param>
	void SetKeyCode(int value);

#pragma endregion

#pragma region Update

	/// <summary>
	/// Updated the inputs state
	/// </summary>
	void Update();

#pragma endregion
};
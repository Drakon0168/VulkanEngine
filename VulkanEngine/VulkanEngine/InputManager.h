#pragma once
#include "pch.h"

#include "Input.h"
#include "InputAxis.h"

class InputManager
{
private:
	static InputManager* instance;

	std::vector<Input> controls;
	std::vector<InputAxis> axes;
	glm::vec2 mousePosition;
	glm::vec2 deltaMouse;

public:

#pragma region Singleton

	/// <summary>
	/// Returns the singleton instance of this class
	/// </summary>
	/// <returns>The singleton instance</returns>
	static InputManager* GetInstance();

#pragma endregion

#pragma region Constructor

	InputManager();

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the list of controls
	/// </summary>
	/// <returns>The list of controls used by the input manager</returns>
	std::vector<Input> GetControls();

	/// <summary>
	/// Returns the list of axes
	/// </summary>
	/// <returns>The list of axes used by the input manager</returns>
	std::vector<InputAxis> GetAxes();

	/// <summary>
	/// Returns the position of the mouse in screen coordinates
	/// </summary>
	/// <returns>The position of the mouse in screen coordinates</returns>
	glm::vec2 GetMousePosition();

	/// <summary>
	/// The delta position of the mouse (how much the mouse has moved since the last frame)
	/// </summary>
	/// <returns>The mouse's change in position from the last frame</returns>
	glm::vec2 GetDeltaMouse();

#pragma endregion

#pragma region Input States

	/// <summary>
	/// Returns whether or not the specified key is down
	/// </summary>
	/// <param name="key">The key to check</param>
	/// <returns>True if the key is down false if it is up</returns>
	bool GetKey(Controls::Controls key);

	/// <summary>
	/// Returns whether or not the specified key was pressed this frame
	/// </summary>
	/// <param name="key">The key to check</param>
	/// <returns>True the first frame that the key was pressed</returns>
	bool GetKeyPressed(Controls::Controls key);

	/// <summary>
	/// Returns whether or not the specified key was released this frame
	/// </summary>
	/// <param name="key">The key to check</param>
	/// <returns>True if the key was released this frame</returns>
	bool GetKeyReleased(Controls::Controls key);

	/// <summary>
	/// Returns the current input state of the specified key
	/// </summary>
	/// <param name="key">The key to check</param>
	/// <returns>The current state of the key</returns>
	InputStates GetKeyState(Controls::Controls key);

	/// <summary>
	/// Returns the value of the specified axis
	/// </summary>
	/// <param name="axis">The axis to check</param>
	/// <returns>The value of the axis</returns>
	float GetAxis(Controls::InputAxes axis);

#pragma endregion

#pragma region Update

	/// <summary>
	/// Updates the inputs, axes, and mouse position variables
	/// </summary>
	void Update();

#pragma endregion
};
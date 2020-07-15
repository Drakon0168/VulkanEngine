#include "pch.h"
#include "Input.h"

#include <Windows.h>

#pragma region Constructor

Input::Input(int keyCode)
{
	this->keyCode = keyCode;
	state = InputStates::Up;
}

#pragma endregion

#pragma region Accessors

InputStates Input::GetState()
{
	return state;
}

int Input::GetKeyCode()
{
	return keyCode;
}

void Input::SetKeyCode(int value)
{
	keyCode = value;
}

#pragma endregion

#pragma region Update

void Input::Update()
{
	//TODO: Change this to work on multiple operating systems not just windows
	bool currentState = GetAsyncKeyState(keyCode) & 0x8000;

	if (state == InputStates::Up || state == InputStates::Released) { //Key was up
		if (currentState) { //Key is down
			state = InputStates::Pressed;
		}
		else { //Key is up
			state = InputStates::Up;
		}
	}
	else { //Key was down
		if (currentState) { //Key is down
			state = InputStates::Down;
		}
		else { //Key is Up
			state = InputStates::Released;
		}
	}
}

#pragma endregion
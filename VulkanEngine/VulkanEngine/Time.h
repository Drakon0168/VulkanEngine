#pragma once

#include "pch.h"

class Time
{
private:
	static std::chrono::steady_clock::time_point currentTime;
	static std::chrono::steady_clock::time_point lastTime;
	static float totalTime;
	static float deltaTime;

public:
#pragma region Accessors

	/// <summary>
	/// Returns the amount of time passed since the application was launched
	/// </summary>
	/// <returns>The time passed in seconds</returns>
	static float GetTotalTime();

	/// <summary>
	/// Returns the amount of time passed since the last update
	/// </summary>
	/// <returns>The time passed in seconds</returns>
	static float GetDeltaTime();

#pragma endregion

#pragma region Update

	/// <summary>
	/// Updates the total time and delta time, should be called once per frame
	/// </summary>
	static void Update();

	/// <summary>
	/// Resets the total time to zero and the current and last times to now
	/// </summary>
	static void Reset();

#pragma endregion
};
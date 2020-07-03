#include "pch.h"
#include "Time.h"

std::chrono::steady_clock::time_point Time::currentTime = std::chrono::high_resolution_clock::now();
std::chrono::steady_clock::time_point Time::lastTime = std::chrono::high_resolution_clock::now();
float Time::totalTime = 0.0f;
float Time::deltaTime = 0.0f;

#pragma region Accessors

float Time::GetTotalTime()
{
    return totalTime;
}

float Time::GetDeltaTime()
{
    return deltaTime;
}

#pragma endregion

#pragma region Update

void Time::Update()
{
    currentTime = std::chrono::high_resolution_clock::now();
    deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
    totalTime += deltaTime;
    lastTime = currentTime;
}

void Time::Reset()
{
    currentTime = std::chrono::high_resolution_clock::now();
    lastTime = currentTime;
    totalTime = 0.0f;
}

#pragma endregion
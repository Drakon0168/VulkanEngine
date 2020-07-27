#include "pch.h"
#include "InputAxis.h"

#pragma region Constructor

InputAxis::InputAxis(float min, float max)
{
	this->min = min;
	this->max = max;

	rawValue = (min + max) / 2;
}

#pragma endregion

#pragma region Accessors

float InputAxis::GetValue()
{
	return (((rawValue - min) / (min - max)) * 2.0f) - 1.0f;
}

float InputAxis::GetRawValue()
{
	return rawValue;
}

float InputAxis::GetMin()
{
	return min;
}

void InputAxis::SetMin(float value)
{
	min = value;
}

float InputAxis::GetMax()
{
	return max;
}

void InputAxis::SetMax(float value)
{
	max = value;
}

#pragma endregion
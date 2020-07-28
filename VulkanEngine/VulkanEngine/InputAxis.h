#pragma once

class InputAxis
{
private:
	float rawValue;
	float min;
	float max;

public:

#pragma region Constructor


	InputAxis(float min = 0.0f, float max = 1.0f);

	//InputAxis(float min, float max);


#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the value of the axis scaled to go from -1 to 1
	/// </summary>
	/// <returns>The scaled value of the axis (-1.0f to 1.0f)</returns>
	float GetValue();

	/// <summary>
	/// The raw value of the axis from min to max
	/// </summary>
	float GetRawValue();

	/// <summary>
	/// Returns the minimum value of the axis
	/// </summary>
	/// <returns>The minimum value</returns>
	float GetMin();

	/// <summary>
	/// Sets the minimum value of the the axis
	/// </summary>
	/// <param name="value">The value to set to</param>
	void SetMin(float value);

	/// <summary>
	/// Returns the maximum value of the axis
	/// </summary>
	/// <returns>The maximum value of the axis</returns>
	float GetMax();

	/// <summary>
	/// Sets the maximum value of the axis
	/// </summary>
	/// <param name="value">The value to set to</param>
	void SetMax(float value);

#pragma endregion
};
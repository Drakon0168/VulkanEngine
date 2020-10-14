#pragma once

#include "pch.h"

class Transform
{
private:

	glm::vec3 position;
	glm::vec3 scale;
	glm::quat orientation;

	glm::mat4 model;
	bool isDirty; //Keeps track of whether position rotation or scale have changed to know when to regenerate the model matrix

#pragma region Model Matrix

/// <summary>
/// Regenerates the model matrix
/// </summary>
	void GenerateModelMatrix();

#pragma endregion

public:

#pragma region Constructor

	Transform(
		glm::vec3 postition = glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::quat orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), 
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the position of the transform
	/// </summary>
	/// <returns>A vector3 representing the position</returns>
	glm::vec3 GetPosition();

	/// <summary>
	/// Sets the position to the specified value
	/// </summary>
	/// <param name="value">The position to set to</param>
	void SetPosition(glm::vec3 value);

	/// <summary>
	/// Returns a quaternion representing the orientation of the transform
	/// </summary>
	/// <returns>A quaternion representing the orientation</returns>
	glm::quat GetOrientation();

	/// <summary>
	/// Sets the orientation to the specified value
	/// </summary>
	/// <param name="value">The orientation to set to</param>
	void SetOrientation(glm::quat value);

	/// <summary>
	/// Sets the orientation to the specified value
	/// </summary>
	/// <param name="value">Euler angles representation of the orientation</param>
	/// <param name="degrees">Whether the value is in degrees or radians (degrees by default)</param>
	void SetOrientation(glm::vec3 value, bool degrees = true);

	/// <summary>
	/// Returns the scale of the transform
	/// </summary>
	/// <returns>A vector3 representing the scale</returns>
	glm::vec3 GetScale();

	/// <summary>
	/// Sets the scale to the specified value
	/// </summary>
	/// <param name="value">The value to set the scale to</param>
	void SetScale(glm::vec3 value);

	/// <summary>
	/// Returns the model matrix of the transform and updates it if necessary
	/// </summary>
	/// <returns>The updated mat4 model matrix</returns>
	glm::mat4 GetModelMatrix();

#pragma endregion

#pragma region Transformations

	/// <summary>
	/// Translates the position by the specified amount
	/// </summary>
	/// <param name="translation">The amount to move by</param>
	/// <param name="relative">Whether to move in local or global space (false by default)</param>
	void Translate(glm::vec3 translation, bool localSpace = false);

	/// <summary>
	/// Rotates the orientation by the specified amount
	/// </summary>
	/// <param name="rotation">The amount to rotate by</param>
	void Rotate(glm::quat rotation);

	/// <summary>
	/// Rotates the orientation by the specified amount
	/// </summary>
	/// <param name="eulerRotation">Euler representation of the rotation</param>
	/// <param name="degrees">Whether the rotation is in degrees or radians (degrees by default)</param>
	void Rotate(glm::vec3 eulerRotation, bool degrees = true);

	/// <summary>
	/// Rotates the transform to look at the target position
	/// </summary>
	/// <param name="target">The position to look towards</param>
	/// <param name="up">The up axis of the rotation (+y by default)</param>
	void LookAt(glm::vec3 target, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));;

#pragma endregion

#pragma region Handles

	void DrawHandles();

#pragma endregion
};
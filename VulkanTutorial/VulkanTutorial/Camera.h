#pragma once

#include "pch.h"
#include "Transform.h"

class Camera
{
private:
	static Camera* mainCamera;

	glm::mat4 projection;
	glm::mat4 view;
	std::shared_ptr<Transform> transform;
	bool projectionDirty;
	bool viewDirty;
	bool perspective;
	float FOV;
	float aspectRatio;
	float orthographicSize;
	glm::vec2 clippingPlanes;

#pragma region Matrix Management

	/// <summary>
	/// Updates the camera's projection if something has changed
	/// </summary>
	void UpdateProjection();

	/// <summary>
	/// Updates the view matrix if something has changed
	/// </summary>
	void UpdateView();

#pragma endregion
public:
#pragma region Constructor

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)),
		bool perspective = true,
		float FOV = glm::radians(45.0f),
		float orthographicSize = 5.0,
		float aspectRatio = 1.0f,
		float nearPlane = 0.1f,
		float farPlane = 100.0f);

	~Camera();

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns a shared ptr to the main camera
	/// </summary>
	/// <returns>The main camera</returns>
	static Camera* GetMainCamera();

	/// <summary>
	/// Sets the main camera to the passed in value
	/// </summary>
	/// <param name="value">The camera to set as main</param>
	static void SetMainCamera(Camera* value);

	/// <summary>
	/// Returns the projection matrix and regenerates it if necessary
	/// </summary>
	/// <returns>The mat4 projection matrix</returns>
	glm::mat4 GetProjection();

	/// <summary>
	/// Returns the view matrix and regenerates it if necessary
	/// </summary>
	/// <returns>The mat4 projection matrix</returns>
	glm::mat4 GetView();

	/// <summary>
	/// Returns the camera's transform
	/// </summary>
	/// <returns>A shared_ptr to the transform</returns>
	std::shared_ptr<Transform> GetTransform();

	/// <summary>
	/// Returns whether or not the projection is perspective or orthographic
	/// </summary>
	/// <returns>True for perspective false for orthographic</returns>
	bool GetPerspective();

	/// <summary>
	/// Sets whether or not the projection is perspective or orthographic
	/// </summary>
	/// <param name="value">True for perspective false for orthographic</param>
	void SetPerspective(bool value);

	/// <summary>
	/// Returns the field of view of the camera
	/// </summary>
	/// <returns>The field of view in radians</returns>
	float GetFOV();

	/// <summary>
	/// Sets the field of view of the camera
	/// </summary>
	/// <param name="value">Float representing the field of view</param>
	/// <param name="degrees">Whether the value is in degrees or radians (degrees by default)</param>
	void SetFOV(float value, bool degrees = true);

	/// <summary>
	/// Returns the orthographic size of the camera (half height when in orthographic projection)
	/// </summary>
	float GetOrthographicSize();

	/// <summary>
	/// Sets the orthographic size of the camera to the specified value (half height when in orthographic projection)
	/// </summary>
	/// <param name="value">The value to set the orthographic size to</param>
	void SetOrthographicSize(float value);

	/// <summary>
	/// Returns the camera's aspect ratio
	/// </summary>
	/// <returns>The aspect ratio</returns>
	float GetAspectRatio();

	/// <summary>
	/// Sets the camera's aspect ratio to the specified value
	/// </summary>
	/// <param name="value">The value to set the aspect ratio to</param>
	void SetAspectRatio(float value);

	/// <summary>
	/// Returns the camera's near clipping plane
	/// </summary>
	/// <returns>The near clipping plane of the camera</returns>
	float GetNearPlane();

	/// <summary>
	/// Sets the near clipping plane to the specified value
	/// </summary>
	/// <param name="value">The value to set the near clipping plane to</param>
	void SetNearPlane(float value);

	/// <summary>
	/// Returns the camera's far clipping plane
	/// </summary>
	/// <returns>The far clipping plance of the camera</returns>
	float GetFarPlane();

	/// <summary>
	/// Sets the camera's far clipping plane to the specified value
	/// </summary>
	/// <param name="value">The calue to set the far clipping plane to</param>
	void SetFarPlane(float value);

#pragma endregion
};
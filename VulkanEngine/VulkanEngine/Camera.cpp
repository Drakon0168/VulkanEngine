#include "pch.h"
#include "Camera.h"

Camera* Camera::mainCamera = nullptr;

#pragma region Constructor

Camera::Camera(glm::vec3 position, glm::quat orientation, bool perspective, float FOV, float orthographicSize, float aspectRatio, float nearPlane, float farPlane)
{
	//Set initial values for matrices and Transform
	projection = glm::mat4(1.0f);
	view = glm::mat4(1.0f);
	transform = std::make_shared<Transform>(position, orientation);

	this->perspective = perspective;
	this->FOV = FOV;
	this->orthographicSize = orthographicSize;
	this->aspectRatio = aspectRatio;
	clippingPlanes = glm::vec2(nearPlane, farPlane);

	//Mark matrices for regeneration
	projectionDirty = true;
	viewDirty = true;

	//Set this to the main camera if no main camera has been set
	if (mainCamera == nullptr) {
		mainCamera = this;
	}
}

Camera::~Camera()
{
}

#pragma endregion

#pragma region Accessors

Camera* Camera::GetMainCamera()
{
	if (mainCamera == nullptr) {
		mainCamera = new Camera();
	}

	return mainCamera;
}

void Camera::SetMainCamera(Camera* value)
{
	mainCamera = value;
}

glm::mat4 Camera::GetProjection()
{
	//Regenerate the projection if it is dirty
	if (projectionDirty) {
		UpdateProjection();
	}

	return projection;
}

glm::mat4 Camera::GetView()
{
	//Regenerate the view matrix if it is dirty
	if (viewDirty) {
		UpdateView();
	}

	return view;
}

std::shared_ptr<Transform> Camera::GetTransform()
{
	//HACK: Sets the view matrix to dirty whenever the transform is accessed, not great as just accessing the transform doesn't necessarily mean anything was changed
	viewDirty = true;

	return transform;
}

bool Camera::GetPerspective()
{
	return perspective;
}

void Camera::SetPerspective(bool value)
{
	perspective = value;

	//Mark the projection matrix for regeneration
	projectionDirty = true;
}

float Camera::GetFOV()
{
	return FOV;
}

void Camera::SetFOV(float value, bool degrees)
{
	if (degrees) {
		value = glm::radians(value);
	}

	FOV = value;

	//Mark the projection matrix for regeneration
	projectionDirty = true;
}

float Camera::GetOrthographicSize()
{
	return orthographicSize;
}

void Camera::SetOrthographicSize(float value)
{
	orthographicSize = value;
}

float Camera::GetAspectRatio()
{
	return aspectRatio;
}

void Camera::SetAspectRatio(float value)
{
	aspectRatio = value;

	//Mark the projection matrix for regeneration
	projectionDirty = true;
}

float Camera::GetNearPlane()
{
	return clippingPlanes.x;
}

void Camera::SetNearPlane(float value)
{
	clippingPlanes.x = value;

	//Mark the projection matrix for regeneration
	projectionDirty = true;
}

float Camera::GetFarPlane()
{
	return clippingPlanes.y;
}

void Camera::SetFarPlane(float value)
{
	clippingPlanes.y = value;

	//Mark the projection matrix for regeneration
	projectionDirty = true;
}

#pragma endregion

#pragma region Matrix Management

void Camera::UpdateProjection()
{
	if (perspective) {
		projection = glm::perspective(FOV, aspectRatio, clippingPlanes.x, clippingPlanes.y);
	}
	else {
		float width = orthographicSize * aspectRatio;
		projection = glm::ortho(-width, width, orthographicSize, -orthographicSize, clippingPlanes.x, clippingPlanes.y);
	}

	projection[1][1] *= -1; //Correct for flipped y-axis from OpenGL
}

void Camera::UpdateView()
{
	//Find the direction to look at by rotating the forward vector by the camera's orientation
	glm::vec3 lookDirection = transform->GetOrientation() * glm::vec3(0.0f, 0.0f, 1.0f);

	//Look in the target direction
	view = glm::lookAt(transform->GetPosition(), transform->GetPosition() + lookDirection, glm::vec3(0.0f, 1.0f, 0.0f));
}

#pragma endregion
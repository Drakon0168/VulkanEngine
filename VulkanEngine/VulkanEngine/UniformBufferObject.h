#pragma once
#include "pch.h"

struct UniformBufferObject {
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 cameraPosition;
	Light lights[5];
};
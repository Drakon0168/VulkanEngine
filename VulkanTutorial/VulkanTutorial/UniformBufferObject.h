#pragma once

#include "pch.h"

struct Light {
	glm::vec3 position;
	glm::vec3 color;
	float range;
};

struct UniformBufferObject {
	glm::mat4 view;
	glm::mat4 projection;
};
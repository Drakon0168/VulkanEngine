#pragma once
#include "pch.h"

struct Light {
public:
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 color;
	alignas(4) float range;

	Light(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f), float range = 0.0f) {
		this->position = position;
		this->color = color;
		this->range = range;
	}
};
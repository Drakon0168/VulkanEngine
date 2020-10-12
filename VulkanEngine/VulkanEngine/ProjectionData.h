#pragma once
#include "pch.h"

struct ProjectionData {
	glm::vec2 minMax;
	std::vector<glm::vec3> minPoints;
	std::vector<glm::vec3> maxPoints;
};
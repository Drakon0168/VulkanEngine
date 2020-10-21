#pragma once
#include "pch.h"

struct CollisionData {
public:
	glm::vec3 contactPoint;
	glm::vec3 collisionNormal;
	float intersectionDistance;
};
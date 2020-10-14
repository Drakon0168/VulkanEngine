#pragma once
#include "pch.h"

#include "Transform.h"

struct DebugShape {
public:
	std::shared_ptr<Transform> transform;
	glm::vec3 color;
	float duration;
	int meshID;

	bool operator==(const DebugShape& other) {
		return (transform == other.transform && color == other.color && duration == other.duration && meshID == other.meshID);
	}

	bool operator!=(const DebugShape& other) {
		return (transform != other.transform || color != other.color || duration != other.duration || meshID != other.meshID);
	}
};
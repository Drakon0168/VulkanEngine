#pragma once
#include "pch.h"

#include "Transform.h"

struct MeshData {
public:
	std::shared_ptr<Transform> transform;
	glm::vec3 color;
	// float duration;
	int meshID;

	bool operator==(const MeshData& other) {
		return (transform == other.transform && color == other.color  && meshID == other.meshID);
	}

	bool operator!=(const MeshData& other) {
		return (transform != other.transform || color != other.color  || meshID != other.meshID);
	}
};

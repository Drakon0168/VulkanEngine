#pragma once

#include "pch.h"

struct AngleAxis {
public:
	glm::vec3 axis;
	float angle;

	AngleAxis(glm::vec3 axis = glm::vec3(0, 1, 0), float angle = 0) {
		this->axis = axis;
		this->angle = angle;
	}

	AngleAxis(glm::quat quat) {
		angle = 2 * glm::acos(quat.w);
		axis = glm::vec3(quat.x, quat.y, quat.z) / glm::sin(angle / 2);
	}

	glm::quat ToQuaternion() {
		return glm::angleAxis(angle, axis);
	}

	AngleAxis operator=(const glm::quat& value) {
		angle = 2 * glm::acos(value.w);
		axis = glm::vec3(value.x, value.y, value.z) / glm::sin(angle / 2);

		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const AngleAxis& value) {
		os << "Angle: " << value.angle << std::endl;
		os << "Axis: (" << value.axis.x << ", " << value.axis.y << ", " << value.axis.z << ")" << std::endl;
		return os;
	}
};
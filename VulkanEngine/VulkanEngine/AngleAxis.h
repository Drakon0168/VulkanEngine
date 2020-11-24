#pragma once

#include "pch.h"

struct AngleAxis {
public:
	glm::vec3 axis;
	float angle;

#pragma region Constructors

	AngleAxis(glm::vec3 axis = glm::vec3(0, 1, 0), float angle = 0) {
		this->axis = axis;
		this->angle = angle;
	}

	AngleAxis(glm::quat quat) {
		angle = 2 * glm::acos(quat.w);
		axis = glm::vec3(quat.x, quat.y, quat.z) / glm::sin(angle / 2);
	}

#pragma endregion

#pragma region Helper Methods

	glm::quat ToQuaternion() {
		glm::vec4 value = glm::vec4(axis * glm::sin(angle / 2), glm::cos(angle / 2));
		return {value.w, value.x, value.y, value.z};
	}

#pragma endregion

#pragma region Operator Overloads

	AngleAxis operator+(const AngleAxis& other) {
		glm::vec4 q1 = glm::vec4(axis, 0) * glm::sin(angle / 2);
		q1.w = glm::cos(angle / 2);

		glm::vec4 q2 = glm::vec4(other.axis, 0) * glm::sin(other.angle / 2);
		q2.w = glm::cos(other.angle / 2);

		AngleAxis sum;
		sum.angle = 2 * glm::acos(glm::dot(q1, glm::vec4(-q2.x, -q2.y, -q2.z, q2.w)));

		if (angle != 0) {
			sum.axis.x = glm::dot(q1, glm::vec4(q2.w, q2.z, -q2.y, q2.x)) / glm::sin(sum.angle / 2);
			sum.axis.y = glm::dot(q1, glm::vec4(-q2.z, q2.w, -q2.x, q2.y)) / glm::sin(sum.angle / 2);
			sum.axis.z = glm::dot(q1, glm::vec4(q2.y, -q2.x, -q2.w, q2.z)) / glm::sin(sum.angle / 2);
		}
		else {
			sum.axis = glm::vec3(0.0f, 1.0f, 0.0f);
		}

		return sum;
	}

	AngleAxis operator+=(const AngleAxis& other) {
		AngleAxis sum = *this + other;
		angle = sum.angle;
		axis = sum.axis;
		return *this;
	}

	AngleAxis operator*(const float& value) {
		return AngleAxis(axis, angle * value);
	}

	AngleAxis operator*=(const float& value) {
		this->angle *= value;
		return *this;
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

#pragma endregion
};
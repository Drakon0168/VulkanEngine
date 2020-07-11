#pragma once

#include "pch.h"


struct TransformData {
	glm::vec4 col1;
	glm::vec4 col2;
	glm::vec4 col3;
	glm::vec4 col4;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 1;
		bindingDescription.stride = sizeof(TransformData);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		//Setup attributes
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};
		attributeDescriptions[0].binding = 1;
		attributeDescriptions[0].location = 3;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(TransformData, col1);

		attributeDescriptions[1].binding = 1;
		attributeDescriptions[1].location = 4;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(TransformData, col2);

		attributeDescriptions[2].binding = 1;
		attributeDescriptions[2].location = 5;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(TransformData, col3);

		attributeDescriptions[3].binding = 1;
		attributeDescriptions[3].location = 6;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(TransformData, col4);

		return attributeDescriptions;
	}

	static TransformData LoadMat4(glm::mat4 value) {
		TransformData data = {};
		
		data.col1 = glm::vec4(value[0][0], value[0][1], value[0][2], value[0][3]);
		data.col2 = glm::vec4(value[1][0], value[1][1], value[1][2], value[1][3]);
		data.col3 = glm::vec4(value[2][0], value[2][1], value[2][2], value[2][3]);
		data.col4 = glm::vec4(value[3][0], value[3][1], value[3][2], value[3][3]);

		return data;
	}
};
#pragma once
#include "pch.h"


struct TransformData {
	glm::vec4 row1;
	glm::vec4 row2;
	glm::vec4 row3;
	glm::vec4 row4;

	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 1;
		bindingDescription.stride = sizeof(TransformData);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions() {
		//Setup attributes
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};
		attributeDescriptions[0].binding = 1;
		attributeDescriptions[0].location = 4;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(TransformData, row1);

		attributeDescriptions[1].binding = 1;
		attributeDescriptions[1].location = 5;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(TransformData, row2);

		attributeDescriptions[2].binding = 1;
		attributeDescriptions[2].location = 6;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(TransformData, row3);

		attributeDescriptions[3].binding = 1;
		attributeDescriptions[3].location = 7;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(TransformData, row4);

		return attributeDescriptions;
	}

	static TransformData LoadMat4(glm::mat4 value) {
		TransformData data = {};
		
		data.row1 = glm::vec4(value[0][0], value[0][1], value[0][2], value[0][3]);
		data.row2 = glm::vec4(value[1][0], value[1][1], value[1][2], value[1][3]);
		data.row3 = glm::vec4(value[2][0], value[2][1], value[2][2], value[2][3]);
		data.row4 = glm::vec4(value[3][0], value[3][1], value[3][2], value[3][3]);

		return data;
	}
};
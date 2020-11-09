#pragma once
#include "pch.h"

#include "Transform.h"

struct InstancedData {
public:
	TransformData transformData;
	glm::vec3 color;

	static VkVertexInputBindingDescription GetBindingDescription(int offset = 0) {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = offset;
		bindingDescription.stride = sizeof(InstancedData);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		return bindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(int offset = 0, int binding = 0) {
		//Setup attributes
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions = std::vector<VkVertexInputAttributeDescription>(5);
		attributeDescriptions[0].binding = binding;
		attributeDescriptions[0].location = offset;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(InstancedData, transformData.row1);

		attributeDescriptions[1].binding = binding;
		attributeDescriptions[1].location = offset + 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(InstancedData, transformData.row2);

		attributeDescriptions[2].binding = binding;
		attributeDescriptions[2].location = offset + 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(InstancedData, transformData.row3);

		attributeDescriptions[3].binding = binding;
		attributeDescriptions[3].location = offset + 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(InstancedData, transformData.row4);


		attributeDescriptions[4].binding = binding;
		attributeDescriptions[4].location = offset + 4;
		attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[4].offset = offsetof(InstancedData, color);

		return attributeDescriptions;
	}


};
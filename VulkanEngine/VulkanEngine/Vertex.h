#pragma once
#include "pch.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
struct Vertex {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 normal;
	alignas(16) glm::vec3 color;
	alignas(16) glm::vec3 textureCoordinate;
	// alignas(8) float zCoord;

	Vertex(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2 textureCoordinate = glm::vec2(0.0f, 0.0f), float z = 0.0f) {
		this->position = position;
		this->color = color;
		this->normal = normal;
		this->textureCoordinate = glm::vec3(textureCoordinate,z);
		// this->zCoord = z;
	}

	static VkVertexInputBindingDescription GetBindingDescription(int offset = 0) {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = offset;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(int offset = 0, int binding = 0) {
		//Setup attributes
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions = std::vector<VkVertexInputAttributeDescription>(4);
		attributeDescriptions[0].binding = binding;
		attributeDescriptions[0].location = offset;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = binding;
		attributeDescriptions[1].location = offset + 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = binding;
		attributeDescriptions[2].location = offset + 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, normal);

		attributeDescriptions[3].binding = binding;
		attributeDescriptions[3].location = offset + 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, textureCoordinate);

		/*attributeDescriptions[4].binding = binding;
		attributeDescriptions[4].location = offset + 4;
		attributeDescriptions[4].format = VK_FORMAT_R32_SFLOAT;
		attributeDescriptions[4].offset = offsetof(Vertex, zCoord);*/

		return attributeDescriptions;
	}
	bool operator==(const Vertex& other) const {
		return position == other.position && color == other.color && textureCoordinate == other.textureCoordinate;
	}
};
namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.textureCoordinate) << 1);
		}
	};
}
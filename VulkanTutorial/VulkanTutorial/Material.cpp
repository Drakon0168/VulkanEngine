#include "pch.h"
#include "Material.h"

#include "TriangleApp.h"

#define logicalDevice TriangleApp::logicalDevice
#define swapChainImages TriangleApp::swapChainImages
#define descriptorSetLayout TriangleApp::descriptorSetLayout
#define uniformBuffers TriangleApp::uniformBuffers

#pragma region Constructor

Material::Material(char* vertexShaderPath, char* fragmentShaderPath)
{
    this->vertexShaderPath = vertexShaderPath;
    this->fragmentShaderPath = fragmentShaderPath;
}

#pragma endregion

#pragma region Accessors

char* Material::GetVertexShaderPath()
{
    return vertexShaderPath;
}

void Material::SetVertexShaderPath(char* value)
{
    vertexShaderPath = value;
}

char* Material::GetFragmentShaderPath()
{
    return fragmentShaderPath;
}

void Material::SetFragmentShaderPath(char* value)
{
    fragmentShaderPath = value;
}

std::vector<VkDescriptorSet> Material::GetDescriptorSets()
{
    return descriptorSets;
}

VkDescriptorPool Material::GetDescriptorPool()
{
    return descriptorPool;
}

#pragma endregion

#pragma region Resource Management

void Material::Init()
{
    CreateDescriptorPool();
    CreateDescriptorSets();
}

void Material::CreateDescriptorPool()
{
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(swapChainImages.size());

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = 1;
    createInfo.pPoolSizes = &poolSize;
    createInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

    if (vkCreateDescriptorPool(logicalDevice, &createInfo, nullptr, &descriptorPool)) {
        throw std::runtime_error("Failed to create Descriptor Pool!");
    }
}

void Material::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = descriptorPool;
	allocateInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
	allocateInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(swapChainImages.size());

	if (vkAllocateDescriptorSets(logicalDevice, &allocateInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate Descriptor Sets!");
	}

	for (size_t i = 0; i < uniformBuffers.size(); i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i]->GetBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, nullptr);
	}
}

void Material::Cleanup()
{
    vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
}

#pragma endregion
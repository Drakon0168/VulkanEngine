#include "pch.h"
#include "Buffer.h"

#include "VulkanManager.h"
#include "CommandBuffer.h"

#define logicalDevice VulkanManager::GetInstance()->GetLogicalDevice()

#pragma region Constructor

Buffer::Buffer(VkBuffer buffer, VkDeviceMemory bufferMemory)
{
	this->buffer = buffer;
	this->bufferMemory = bufferMemory;
}

void Buffer::Cleanup()
{
	vkDestroyBuffer(logicalDevice, buffer, nullptr);
	vkFreeMemory(logicalDevice, bufferMemory, nullptr);
}

#pragma endregion

#pragma region Accessors

VkBuffer Buffer::GetBuffer()
{
	return buffer;
}

void Buffer::SetBuffer(VkBuffer value)
{
	buffer = value;
}

VkDeviceMemory Buffer::GetBufferMemory()
{
	return bufferMemory;
}

void Buffer::SetBufferMemory(VkDeviceMemory value)
{
	bufferMemory = value;
}

#pragma endregion

#pragma region Helper Methods

void Buffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, Buffer& buffer)
{
	//Setup Create Info
	VkBufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = size;
	createInfo.usage = usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	//Create Buffer
	if (vkCreateBuffer(logicalDevice, &createInfo, nullptr, &buffer.buffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Buffer!");
	}

	//Find memory requirements
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, buffer.buffer, &memoryRequirements);

	//Setup Vertex Buffer memory allocate info
	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = VulkanManager::GetInstance()->FindMemoryType(memoryRequirements.memoryTypeBits, properties);

	//Allocate memory
	if (vkAllocateMemory(logicalDevice, &allocateInfo, nullptr, &buffer.bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate Buffer memory!");
	}

	//Bind buffer memory
	vkBindBufferMemory(logicalDevice, buffer.buffer, buffer.bufferMemory, 0);
}

void Buffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	//Begin the command buffer
	VkCommandBuffer commandBuffer = CommandBuffer::BeginSingleTimeCommand();

	//Copy the buffer
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	//End the command buffer
	CommandBuffer::EndSingleTimeCommand(commandBuffer);
}

#pragma endregion
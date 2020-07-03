#pragma once

#include "pch.h"

class Buffer
{
private:
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
public:
#pragma region Constructor

	Buffer(VkBuffer buffer = VK_NULL_HANDLE, VkDeviceMemory bufferMemory = VK_NULL_HANDLE);

	/// <summary>
	/// Destroys the buffer and frees buffer memory
	/// </summary>
	void Cleanup();

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the VkBuffer associated with this buffer
	/// </summary>
	/// <returns>The VkBuffer associated with this buffer</returns>
	VkBuffer GetBuffer();

	/// <summary>
	/// Sets the VkBuffer
	/// </summary>
	/// <param name="value">The value to set the VkBuffer to</param>
	void SetBuffer(VkBuffer value);

	/// <summary>
	/// Returns the device memory associated with this buffer
	/// </summary>
	/// <returns>The VkDeviceMemory associated with this buffer</returns>
	VkDeviceMemory GetBufferMemory();

	/// <summary>
	/// Sets the VkDeviceMemory
	/// </summary>
	/// <param name="value">The value to set the VkDeviceMemory to</param>
	void SetBufferMemory(VkDeviceMemory value);

#pragma endregion

#pragma region Helper Methods

	/// <summary>
	/// Creates a buffer with the specified parameters
	/// </summary>
	/// <param name="size">The size of the data to be stored</param>
	/// <param name="usage">The intended VK_BUFFER_USAGE of this buffer</param>
	/// <param name="properties">The required memory properties for the created buffer</param>
	/// <param name="buffer">The buffer to create</param>
	/// <param name="bufferMemory">The device memory associated with the buffer</param>
	static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, Buffer& buffer);

	/// <summary>
	/// Copies data from the source buffer to the destination buffer
	/// </summary>
	/// <param name="srcBuffer">The buffer to copy data from</param>
	/// <param name="dstBuffer">The buffer to transfer the data to</param>
	/// <param name="size">The size of the data to be copied</param>
	static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

#pragma endregion
};
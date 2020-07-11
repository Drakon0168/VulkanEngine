#pragma once

#include "pch.h"

class Command
{
public:
	static VkCommandPool commandPool;

#pragma region Helper Methods

	/// <summary>
	/// Begins a command buffer for single time use
	/// </summary>
	/// <returns>The command that was created</returns>
	static VkCommandBuffer BeginSingleTimeCommand();

	/// <summary>
	/// Ends a single time command buffer
	/// </summary>
	/// <param name="commandBuffer">The command buffer to end</param>
	static void EndSingleTimeCommand(VkCommandBuffer commandBuffer);

#pragma endregion
};
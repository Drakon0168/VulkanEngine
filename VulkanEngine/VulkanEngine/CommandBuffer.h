#pragma once

class CommandBuffer
{
public:

#pragma region Helper Methods

	/// <summary>
	/// Begins a command buffer for one time use
	/// </summary>
	/// <returns>The command buffer that has been started</returns>
	static VkCommandBuffer BeginSingleTimeCommand();

	/// <summary>
	/// Ends a command buffer that was created for single time use
	/// </summary>
	/// <param name="commandBuffer">The single use command buffer to end</param>
	static void EndSingleTimeCommand(VkCommandBuffer commandBuffer);

#pragma endregion
};
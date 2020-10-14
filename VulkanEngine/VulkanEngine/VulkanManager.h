#pragma once

#include "pch.h"

#include "CommandBuffer.h"
#include "Image.h"

class VulkanManager
{
private:
	static VulkanManager* instance;

	// TO MOVE GUI STUFF
	std::vector<VkFramebuffer> guiFrameBuffers;
	VkRenderPass imGuiRenderPass;


	VkInstance vulkanInstance = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;

	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;

	std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

#pragma region Memory Management

	/// <summary>
	/// Initializes vulkan resources
	/// </summary>
	void InitVulkan();

	/// <summary>
	/// Creates and allocates the application instance
	/// </summary>
	void CreateInstance();

	/// <summary>
	/// Creates and allocates the logical device
	/// </summary>
	void CreateLogicalDevice();

	/// <summary>
	/// Creates and allocates the surface
	/// </summary>
	void CreateSurface();

	/// <summary>
	/// Cleans up application resources
	/// </summary>
	void Cleanup();

#pragma endregion

#pragma region MainLoop

	/// <summary>
	/// Handles the draw and update loops
	/// </summary>
	void MainLoop();

	/// <summary>
	/// Draws a frame of the application
	/// </summary>
	void Draw();

	/// <summary>
	/// Updates the application, called once per frame
	/// </summary>
	void Update();

#pragma endregion

#pragma region Physical Device Management

	/// <summary>
	/// Finds the extensions required for the application
	/// </summary>
	/// <returns>A std::vector<const char*> of the required extensions</returns>
	std::vector<const char*> GetRequiredExtensions();

	/// <summary>
	/// Selects a physical device to use
	/// </summary>
	void PickPhysicalDevice();

	/// <summary>
	/// Checks that the given physical device can support the required extensions
	/// </summary>
	/// <param name="physicalDevice">The physical device to check</param>
	/// <returns>True if the device supports the required extensions</returns>
	bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

	/// <summary>
	/// Scores the given physical device based on features and extension support
	/// </summary>
	/// <param name="physicalDevice">The physical device to score</param>
	/// <returns>The score given to the device</returns>
	int RateDevice(VkPhysicalDevice physicalDevice);

	/// <summary>
	/// Returns whether or not the device will be able to run the application
	/// </summary>
	/// <param name="physicalDevice">The device to check</param>
	/// <param name="deviceFeatures">The device's features</param>
	/// <returns>True if the device is suitable</returns>
	bool IsDeviceSuitable(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures deviceFeatures);

#pragma endregion



public:
	inline static bool initGui;

#pragma region Singleton

	/// <summary>
	/// Returns the singleton instance of this class
	/// </summary>
	/// <returns>The singleton instance</returns>
	static VulkanManager* GetInstance();

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the VkInstance that this application is running
	/// </summary>
	/// <returns>The VkInstance that is in use</returns>
	VkInstance GetVulkanInstance();

	/// <summary>
	/// Returns the physical device that is being used by the application
	/// </summary>
	/// <returns>The VkPhysicalDevice being used</returns>
	VkPhysicalDevice GetPhysicalDevice();

	/// <summary>
	/// Returns the logical device that is being used by the application
	/// </summary>
	/// <returns>The VkDevice being used</returns>
	VkDevice GetLogicalDevice();

	/// <summary>
	/// Returns the Surface that is being used by the application
	/// </summary>
	/// <returns>The VkSurfaceKHR being used</returns>
	VkSurfaceKHR GetSurface();

	/// <summary>
	/// Returns the graphics queue that is being used by the application
	/// </summary>
	/// <returns>The VkQueue being used for graphics operations</returns>
	VkQueue GetGraphicsQueue();

	/// <summary>
	/// Returns the present queue that is being used by the application
	/// </summary>
	/// <returns>The VkQueue being used for present operations</returns>
	VkQueue GetPresentQueue();

#pragma endregion

#pragma region Helper Methods

	/// <summary>
	/// Check support for the requested memory type
	/// </summary>
	/// <param name="typeFilter">The type filter of the memory</param>
	/// <param name="properties">Required memory properties</param>
	/// <returns>The index of the suitable memory properties</returns>
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	/// <summary>
	/// Finds the indices of the different queue families on the physical device
	/// </summary>
	/// <param name="physicalDevice">The physical device that is being used by the application</param>
	/// <returns>The indices of the queue families</returns>
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);

#pragma endregion

#pragma region Run

	/// <summary>
	/// Starts the application
	/// </summary>
	void Run();

#pragma endregion
};
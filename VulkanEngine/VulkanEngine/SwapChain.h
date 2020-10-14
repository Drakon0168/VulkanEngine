#pragma once
#include "pch.h"

#include "Image.h"
#include "Buffer.h"

class SwapChain
{
private:
	const int MAX_FRAMES_IN_FLIGHT = 2;

	static SwapChain* instance;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	size_t currentFrame = 0;
	bool frameBufferResized = false;

	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> frameBuffers;
	VkFormat imageFormat;
	VkExtent2D extent;

	VkRenderPass renderPass;

	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> commandBuffers;


	std::vector<Buffer> uniformBuffers;
	Image depthImage;

public:

#pragma region Singleton

	/// <summary>
	/// Returns the singleton instance of this class
	/// </summary>
	/// <returns>The singleton instance</returns>
	static SwapChain* GetInstance();

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the swap chain extents
	/// </summary>
	/// <returns>The VkExtent2D of the swap chain</returns>
	VkExtent2D GetExtents();

	/// <summary>
	/// Returns the render pass used by the swap chain
	/// </summary>
	/// <returns>The VkRenderPass being used</returns>
	VkRenderPass GetRenderPass();

	/// <summary>
	/// Returns the list of uniform buffers storing current camera position
	/// </summary>
	/// <returns>Buffer vector of the uniform buffers</returns>
	std::vector<Buffer> GetUniformBuffers();

	/// <summary>
	/// Returns the depth image
	/// </summary>
	/// <returns>Image representing depth data</returns>
	Image GetDepthImage();

	/// <summary>
	/// Returns the list of swap chain images
	/// </summary>
	/// <returns>VkImage vector of the images used by the swap chain</returns>
	std::vector<VkImage> GetImages();

	/// <summary>
	/// Returns the vector of swap chain image views
	/// </summary>
	/// <returns>VkImageView vector of the image views used by the swap chain</returns>
	std::vector<VkImageView> GetImageViews();

	/// <summary>
	/// Returns the frame buffers used by the swap chain
	/// </summary>
	/// <returns>VkFrameBuffer vector of the frame buffers used by the swap chain</returns>
	std::vector<VkFramebuffer> GetFrameBuffers();

	/// <summary>
	/// Returns the image format used by the swap chain
	/// </summary>
	/// <returns>The VkFormat used by the swap chain images</returns>
	VkFormat GetFormat();

	/// <summary>
	/// Returns whether or not the fram buffer has been resized since the last update
	/// </summary>
	/// <returns>True if the frame buffer has been resized</returns>
	bool GetFrameBufferResized();

	/// <summary>
	/// Sets the frame buffer resized boolean
	/// </summary>
	/// <param name="value">The value to set frameBufferResized to</param>
	void SetFrameBufferResized(bool value);

	/// <summary>
	/// Returns the command pool being used by the application
	/// </summary>
	/// <returns>The command pool that is in use</returns>
	VkCommandPool GetCommandPool();

	/// <summary>
	/// Returns the list of command buffers used by the swap chain
	/// </summary>
	/// <returns>The list of command buffers</returns>
	std::vector<VkCommandBuffer> GetCommandBuffers();

	/// <summary>
	/// Returns the command buffer at the specified index
	/// </summary>
	/// <param name="index">The index to get</param>
	/// <returns>The command buffer at index</returns>
	VkCommandBuffer* GetCommandBuffer(uint32_t index);

	VkCommandBuffer* GetGuiCommandBuffer(uint32_t index);

#pragma endregion

#pragma region Memory Management

	/// <summary>
	/// Creates and allocates swap chain resources
	/// </summary>
	void CreateSwapChainResources();

	/// <summary>
	/// Used for the first time creation of the swap chain
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// Recreates the swap chain after the window has been resized
	/// </summary>
	void RecreateSwapChain();

	/// <summary>
	/// Cleans up swap chain resources to be recreated
	/// </summary>
	void Cleanup();

	/// <summary>
	/// Cleans up swap chain resources when the application is closed
	/// </summary>
	void FullCleanup();

	/// <summary>
	/// Creates and allocates the swap chain images
	/// </summary>
	void CreateImageViews();

	/// <summary>
	/// Creates and allocates the frame buffers used by the swap chain
	/// </summary>
	void CreateFrameBuffers();

	/// <summary>
	/// Creates the resources used by the depth image
	/// </summary>
	void CreateDepthResources();

	/// <summary>
	/// Creates the semaphores and fences used by the swap chain
	/// </summary>
	void CreateSyncObjects();

	/// <summary>
	/// Creates and allocates the render pass used by the swap chain
	/// </summary>
	void CreateRenderPass();

	/// <summary>
	/// Creates and allocates the uniform buffer
	/// </summary>
	void CreateUniformBuffers();

	/// <summary>
	/// Creates and allocates the command pool
	/// </summary>
	void CreateCommandPool();


	/// <summary>
	/// Creates and allocates the command buffers
	/// </summary>
	void CreateCommandBuffers();

#pragma endregion

#pragma region Game Loop

	/// <summary>
	/// Updates the current uniform buffer
	/// </summary>
	/// <param name="imageIndex">The index of the next image in the swap chain</param>
	void UpdateUniformBuffer(uint32_t imageIndex);

	/// <summary>
	/// Begins drawing the current frame
	/// </summary>
	/// <returns>The index of the next image in the swap chain</returns>
	uint32_t BeginDraw();

	/// <summary>
	/// Submits the draw call of the current frame
	/// </summary>
	void EndDraw(uint32_t imageIndex);

#pragma endregion

#pragma region Depth Image

	/// <summary>
	/// Finds supported depth image formats
	/// </summary>
	/// <param name="candidates">Potential formats to check</param>
	/// <param name="tiling">Required tiling format</param>
	/// <param name="features">Required format features</param>
	/// <returns>A suitable VkFormat</returns>
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	/// <summary>
	/// Returns the format used for depth images
	/// </summary>
	/// <returns>The VkFormat used for depth images</returns>
	VkFormat FindDepthFormat();

	/// <summary>
	/// Returns whether the supplied format supports depth stencil
	/// </summary>
	/// <param name="format">The format to check</param>
	/// <returns>Whether or not the format supports the depth stencil</returns>
	bool HasDepthStencil(VkFormat format);

#pragma endregion

#pragma region Support Checks

	/// <summary>
	/// Creates a swap chain support details struct from the selected physical device
	/// </summary>
	/// <param name="device">The physical device to check</param>
	/// <returns>Swap chain support details of the selected device</returns>
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	/// <summary>
	/// Selects a surface format for the swap chain
	/// </summary>
	/// <param name="availableFormats">The possible formats to choose from</param>
	/// <returns>A VkSurfaceFormatKHR to use for the swap chain</returns>
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	/// <summary>
	/// Selects a surface present mode for the swap chain
	/// </summary>
	/// <param name="availablePresentModes"The possible present modes to choose from></param>
	/// <returns>A VkPresentModeKHR to use for the swap chain</returns>
	VkPresentModeKHR ChooseSwapSurfacePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	/// <summary>
	/// Selects the extents to use for the swap chain
	/// </summary>
	/// <param name="surfaceCapabilities">The capabilities of the surface</param>
	/// <returns>The extent size to use for the swap chain</returns>
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

#pragma endregion
};
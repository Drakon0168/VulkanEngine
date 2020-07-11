#pragma once

#include "pch.h"
#include "Vertex.h"
#include "TransformData.h"
#include "Buffer.h"
#include "UniformBufferObject.h"
#include "Mesh.h"
#include "Camera.h"

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	//Returns true if all of the queue families have been set
	bool IsConplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities = {};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class TriangleApp
{
public:
	//Initailizes the window and starts the main loop
	void Run();

	//TODO: Set these up properly when I'm done testing
	static VkPhysicalDevice physicalDevice;
	static VkDevice logicalDevice;
	static VkQueue graphicsQueue;
	static VkQueue presentQueue;

	//Find the type of memory used by the GPU
	static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	const int MAX_FRAMES_IN_FLIGHT = 2;

	std::chrono::steady_clock::time_point currentTime;
	std::chrono::steady_clock::time_point lastTime;
	static float deltaTime;
	static float totalTime;

	Camera* camera;
	std::vector<Mesh> meshes;

	GLFWwindow* window;

	VkInstance instance;
	
	VkDebugUtilsMessengerEXT debugMessenger;

	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;
	bool frameBufferResized = false;

	std::vector<std::shared_ptr<Buffer>> vertexBuffers;
	std::vector<std::shared_ptr<Buffer>> indexBuffers;

	std::vector<std::shared_ptr<Buffer>> uniformBuffers;

	std::vector<VkDescriptorSet> descriptorSets;
	VkDescriptorPool descriptorPool;

	std::vector<VkCommandBuffer> commandBuffers;

	VkSurfaceKHR surface;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageView;
	std::vector<VkFramebuffer> swapChainFrameBuffers;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	VkImage textureImage;
	VkImageView textureImageView;
	VkDeviceMemory textureImageMemory;

	VkImage depthImage;
	VkImageView depthImageView;
	VkDeviceMemory depthImageMemory;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	//Initialize GLFW resources and create the window
	void InitWindow();
	//Initialize Vulkan resources
	void InitVulkan();
	//Cleans up GLFW and Vulkan resources called after the window is closed
	void Cleanup();

	//Called every frame and used to update objects on the screen
	void MainLoop();
	//Called every update loop
	void Update();
	//Draws all of the objects to the screen
	void DrawFrame();
	//Creates the semaphores to manage async frame rendering
	void CreateSyncObjects();

	//Initialize the Vulkan application
	void CreateInstance();
	//Finds out the extensions that are required for the program to run
	std::vector<const char*> GetRequiredExtensions();

	//Setup the physical device
	void PickPhysicalDevice();
	//Finds the graphics queue family associated with this physical device
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	//Checks that a physical device supports the required extensions
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	//Scores devices based on suitability in order to pick the best device
	int RateDevice(VkPhysicalDevice device);
	//Checks whether the device is capable of running the program
	bool IsDeviceSuitable(VkPhysicalDevice device, VkPhysicalDeviceFeatures deviceFeatures);

	//Setup the logical device that interfaces with the physical device
	void CreateLogicalDevice();

	//Creates a surface for Vulkan to render to
	void CreateSurface();

	//Creates a texture image
	void CreateTextureImage();
	//Creates an image view for the texture
	void CreateTextureImageView();
	//Creates an image and binds it to memory
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& memory);
	//Creates an image view
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	//Transitions the image's Image Layout
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	//Copies image data to a buffer
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t imageWidth, uint32_t imageHeight);

	//Creates the depth buffer resources
	void CreateDepthResources();
	//Checks support for required formats
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	//Finds a format that can be used as a depth attachment
	VkFormat FindDepthFormat();
	//Checks whether the given format has a depth stencil attachment
	bool HasDepthStencil(VkFormat format);

	//Creates the swap chain
	void CreateSwapChain();
	//Recreates the swap chain if variables such as window size have changed
	void RecreateSwapChain();
	//Cleanup swap chain resources when the swap chain is recreated
	void CleanupSwapChain();
	//Creates the image views to be used by the swap chain
	void CreateImageViews();
	//Creates the frame buffers to be used by the swap chain
	void CreateFrameBuffers();
	//Populates the swap chain support struct
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	//Chooses one of the available swap chain formats
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	//Chooses one of the available swap chain present modes
	VkPresentModeKHR ChooseSwapSurfacePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	//Chooses the size of the frames in the swap chain
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

	//Creates the graphics pipeline
	void CreateGraphicsPipeline();
	//Create the descriptor set for the Uniform Buffer Object
	void CreateDescriptorSetLayout();
	//Creates the descriptor pool
	void CreateDescriptorPool();
	//Creates the descriptor sets
	void CreateDescriptorSets();
	//Creates the Render Pass
	void CreateRenderPass();
	//Creates the Vulkan shader from the shader data
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	//Creates the vertex buffer
	void CreateVertexBuffer(int index);
	//Creates the index buffer
	void CreateIndexBuffer(int index);
	//Creates the uniform buffer
	void CreateUniformBuffers();
	//Updates the uniform buffers
	void UpdateUniformBuffers(uint32_t currentImage);
	
	//Creates the Command Pool
	void CreateCommandPool();
	//Creates the Command Buffers
	void CreateCommandBuffers();

	//Setup the debug util messenger
	void SetupDebugMessenger();
	//Setup a Debug Util Messenger CreateInfo
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	//Checks that all requested validation layers are supported
	bool CheckValidationLayerSupport();
	//The method that is called when Vulkan throws an error
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
		);

	//Called when the GLFW window is resized
	static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
	//Reads in a file and saves it to a char list
	static std::vector<char> ReadFile(const std::string& filePath);
};
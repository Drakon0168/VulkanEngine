#include "pch.h"
#include "TriangleApp.h"

#include "Buffer.h"
#include "Command.h"

VkPhysicalDevice TriangleApp::physicalDevice = VK_NULL_HANDLE;
VkDevice TriangleApp::logicalDevice = VK_NULL_HANDLE;

VkQueue TriangleApp::graphicsQueue = VK_NULL_HANDLE;
VkQueue TriangleApp::presentQueue = VK_NULL_HANDLE;

float TriangleApp::deltaTime = 0.0f;
float TriangleApp::totalTime = 0.0f;

#pragma region Proxy Functions

//Create the Debug Utils Messenger from the Create Info that is set up in SetupDebugCallback()
VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger
) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr) {
		return func(instance, createInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

//Destroy the Debug Utils Messenger when the window closes in Cleanup()
void DestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator
) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

#pragma endregion

#pragma region Setup

void TriangleApp::Run()
{
	if (enableValidationLayers) {
		std::cout << "Setting up Window . . ." << std::endl;
	}
	
	InitWindow();

	if (enableValidationLayers) {
		std::cout << "Setting up Vulkan . . ." << std::endl;
	}

	//TODO: Remove this once meshes are generated in an init function or loaded from models
	meshes.resize(2);
	meshes[0].GenerateCube();
	meshes[1].GenerateSphere(10);

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			meshes[0].AddInstance(std::make_shared<Transform>(glm::vec3(-1.5f + 1.5f * x, 0.0f, -1.5f + 1.5f * y)));
			meshes[1].AddInstance(std::make_shared<Transform>(glm::vec3(-1.5f + 1.5f * x, 1.0f, -1.5f + 1.5f * y)));
		}
	}

	vertexBuffers.resize(meshes.size());
	indexBuffers.resize(meshes.size());

	//Set starting camera values
	camera = new Camera(glm::vec3(0.0f, 5.0f, 5.0f), glm::quat(glm::vec3(glm::radians(45.0f), 0.0f, 0.0f)), true);
	camera->GetTransform()->LookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	InitVulkan();

	if (enableValidationLayers) {
		std::cout << "Finished Setup" << std::endl;
	}

	MainLoop();
	Cleanup();

	delete camera;
}

void TriangleApp::InitWindow()
{
	//Initialize GLFW
	glfwInit();

	//Prevent GLFW from loading OpenGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Create the window
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan Window", nullptr, nullptr);

	//Setup the frame buffer resized callback
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FrameBufferResizeCallback);
}

void TriangleApp::InitVulkan()
{
	//Create application instance
	CreateInstance();

	//Set the debug callback function
	SetupDebugMessenger();

	//Setup the rendering surface
	CreateSurface();

	//Pick the physical device to use
	PickPhysicalDevice();

	//Create the logical device
	CreateLogicalDevice();

	//Create the swap chain
	CreateSwapChain();

	//Create the image views
	CreateImageViews();

	//Create the render pass
	CreateRenderPass();

	//Create the descriptor set layout
	CreateDescriptorSetLayout();

	//Create Depth Buffer resources
	CreateDepthResources();

	//Create the frame buffers
	CreateFrameBuffers();

	//Create the graphics pipeline
	CreateGraphicsPipeline();

	//Create the command pool
	CreateCommandPool();

	//Create Uniform Buffers
	CreateUniformBuffers();

	for (size_t i = 0; i < meshes.size(); i++) {
		//Create the Vertex Buffer
		CreateVertexBuffer(i);

		//Create the Index Buffer
		CreateIndexBuffer(i);

		//Create Instance Buffer
		meshes[i].CreateInstanceBuffer();
	}

	//Create the descriptor pool
	CreateDescriptorPool();

	//Create the descriptor sets
	CreateDescriptorSets();

	//Create the texture image
	CreateTextureImage();

	//Create the texture's image view
	CreateTextureImageView();

	//Create the Command Buffers
	CreateCommandBuffers();

	//Create the Semaphores and Fences
	CreateSyncObjects();
}

void TriangleApp::Cleanup()
{
	//Destroy Semaphores
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
	}
	
	//Cleanup swap chain and associated resources
	CleanupSwapChain();

	//Destroy Command Pool
	vkDestroyCommandPool(logicalDevice, Command::commandPool, nullptr);

	//Cleanup instance buffer
	for (size_t i = 0; i < meshes.size(); i++) {
		meshes[i].GetInstanceBuffer()->Cleanup();
	}

	//Destroy Descriptor Set Layout
	vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);

	//Cleanup Buffers
	for (size_t i = 0; i < meshes.size(); i++) {
		meshes[i].GetVertexBuffer()->Cleanup();
		meshes[i].GetIndexBuffer()->Cleanup();
	}

	//Destroy Logical Device
	vkDestroyDevice(logicalDevice, nullptr);

	//Destroy Debug Utils Messenger if it was created
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	//Destroy Surface
	vkDestroySurfaceKHR(instance, surface, nullptr);

	//Destroy Vulkan resources
	vkDestroyInstance(instance, nullptr);

	//Destroy GLFW resources
	glfwDestroyWindow(window);

	//Terminate the window
	glfwTerminate();
}

#pragma endregion

#pragma region Main Loop

void TriangleApp::MainLoop()
{
	//Set starting time values
	currentTime = std::chrono::high_resolution_clock::now();
	lastTime = std::chrono::high_resolution_clock::now();

	//Loop until the window is closed
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		//Calculate time change per frame
		currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
		lastTime = currentTime;
		totalTime += deltaTime;

		Update();
		DrawFrame();
	}

	vkDeviceWaitIdle(logicalDevice);
}

void TriangleApp::Update()
{
	float scaledTime = (totalTime * glm::radians(360.0f)) / 7;

	meshes[0].GetActiveInstances()[4]->SetPosition(glm::vec3(0.0f, sinf(scaledTime * 1.0f), 0.0f));
	meshes[0].GetActiveInstances()[0]->Rotate(glm::vec3(0.875f, 1.75f, 0.875f) * 90.0f * deltaTime);
	meshes[1].GetActiveInstances()[4]->SetPosition(glm::vec3(cosf(scaledTime), 1.0f, sinf(scaledTime)));
}

void TriangleApp::DrawFrame()
{
	//Wait for the fence to finish
	vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	//Find the index of the next image
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to aquire next swap chain image!");
	}

	//Make sure the image is not already in use
	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(logicalDevice, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	//Mark the image as being in use
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	//TODO: Re-record command buffer for each object

	//Update uniform buffers
	UpdateUniformBuffers(imageIndex);

	//Update instance buffer
	for (size_t i = 0; i < meshes.size(); i++) {
		meshes[i].UpdateInstanceBuffer();
	}

	//Submit to the graphics queue
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	//Reset fence
	vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	//Present on the screen
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	
	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain();
		frameBufferResized = false;
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swap chain image!");
	}

	//Update the current frame
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void TriangleApp::CreateSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &inFlightFences[i])) {
			throw std::runtime_error("Failed to create synchronization objects!");
		}
	}
}

#pragma endregion

#pragma region Vulkan Setup

void TriangleApp::CreateInstance()
{
	if (enableValidationLayers && !CheckValidationLayerSupport()) {
		throw std::runtime_error("Requested validation layer is not available!");
	}

	/*
	Setup App Info
		Provides information about the application to the graphics drivers for better optimization
	*/
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Triangle App";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	/*
	Setup Instance Info
		Allows GLFW and Vulkan to interface
	*/
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//Check for optional extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	//Find the number of required extensions
	std::vector<const char*> requiredExtensions = GetRequiredExtensions();
	uint32_t requiredExtensionsCount = static_cast<uint32_t>(requiredExtensions.size());

	/*
	//Print out the extensions that have been found and the extensions that are needed
	std::cout << "Found " << extensionCount << " extensions:" << std::endl;
	for (VkExtensionProperties e : extensions) {
		std::cout << "\t" << e.extensionName << std::endl;
	}

	std::cout << "\nRequired " << requiredExtensionsCount << " extensions:" << std::endl;
	for (uint32_t i = 0; i < requiredExtensionsCount; i++) {
		std::cout << "\t" << requiredExtensions[i] << std::endl;
	}
	*/

	//Ensure that required extensions exist
	for (uint32_t i = 0; i < requiredExtensionsCount; i++) {
		bool foundExtension = false;

		for (uint32_t j = 0; j < extensionCount; j++) {
			if (strcmp(requiredExtensions[i], extensions[j].extensionName) == 0) {
				foundExtension = true;
				break;
			}
		}

		if (!foundExtension) {
			char message[256] = "Could not find required extension: ";
			strcat_s(message, sizeof(message), requiredExtensions[i]);

			throw std::runtime_error(message);
		}
	}

	//Add supported extensions to the CreateInfo struct
	createInfo.enabledExtensionCount = requiredExtensionsCount;
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	createInfo.enabledLayerCount = 0;

	//Add validation layers if they are enabled
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	//Create the instance
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance!");
	}
}

std::vector<const char*> TriangleApp::GetRequiredExtensions()
{
	//Find extensions required by GLFW
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//If validation layers are enabled add Debug Utilities to required extension list
	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

#pragma endregion

#pragma region Physical Device Management

void TriangleApp::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPU's with Vulkan support!");
	}

	//Get physical devices
	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

	//Add devices to sorted list based on suitability
	std::multimap<int, VkPhysicalDevice> deviceMap;

	for (auto device : physicalDevices) {
		int score = RateDevice(device);
		deviceMap.insert(std::make_pair(score, device));
	}

	//Check the key of the first element in the map
	if (deviceMap.rbegin()->first > 0) {
		//If the key is valid use this physical device
		physicalDevice = deviceMap.rbegin()->second;
	}
	else {
		throw std::runtime_error("No suitable device found!");
	}
}

QueueFamilyIndices TriangleApp::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = {};

	//Find the queue families
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	//Find a queue that supports graphics operations
	int i = 0;

	for (const auto queueFamily : queueFamilies) {
		//Check for Graphics support
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		//Check for present support
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		//Exit the loop if we've found all of the necessary queue families
		if (indices.IsConplete()) {
			break;
		}

		i++;
	}

	return indices;
}

bool TriangleApp::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	//Ensure that all required extensions are on the available extensions list
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end()); //Make a copy of required extensions list
	for (auto extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName); //If the current extension is on the copied list, erase it
	}

	return requiredExtensions.empty(); //If the copied list is empty all required extensions have been found
}

int TriangleApp::RateDevice(VkPhysicalDevice device)
{
	//Get Device Properties
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	//Get Device Features
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	//Check for base specifications needed to run the program, if these are not met return 0
	if (!IsDeviceSuitable(device, deviceFeatures)) {
		return 0;
	}

	//Set a score based on optional features, initial score of 1 because it meets requirements
	int score = 1;

	/*
	//Output device scoring information
	std::cout << deviceProperties.deviceName << " Score:" << std::endl;
	std::cout << "\tMeets Requirements: 1" << std::endl;
	std::cout << "\tDedicated Graphics Card: " << (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1000 : 0) << std::endl;
	std::cout << "\tMax Image Dimensions 2D: " << deviceProperties.limits.maxImageDimension2D << std::endl;
	*/

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	score += deviceProperties.limits.maxImageDimension2D;

	return score;
}

bool TriangleApp::IsDeviceSuitable(VkPhysicalDevice device, VkPhysicalDeviceFeatures deviceFeatures)
{
	//Device does not support geometry shaders
	if (!deviceFeatures.geometryShader) {
		return false;
	}

	//Device cannot process graphics commands
	QueueFamilyIndices queueFamilies = FindQueueFamilies(device);
	if (!queueFamilies.IsConplete()) {
		return false;
	}

	//Make sure that required extensions are supported
	if (!CheckDeviceExtensionSupport(device)) {
		return false;
	}

	//Make sure the swap chain is supported
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
	if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
		return false;
	}

	return true;
}

#pragma endregion

#pragma region Logical Device Management

void TriangleApp::CreateLogicalDevice()
{
	//Setup Queue Families
	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};
	float queuePriority = 1.0f;

	//Create queue create infos
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	//Set used device features
	VkPhysicalDeviceFeatures deviceFeatures = {};

	//Setup Logical Device
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else{
		createInfo.enabledLayerCount = 0;
	}

	//Create the Logical Device
	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logical device!");
	}

	//Set the queues
	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
}

#pragma endregion

#pragma region Surface Management

void TriangleApp::CreateSurface()
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}

#pragma endregion

#pragma region Image Management

void TriangleApp::CreateTextureImage()
{
	//TODO: figure out how to load the data from an image file
	/*
	int textureWidth, textureHeight, textureChannels;
	stbi_uc* pixels = stbi_load("textures/testImage.png", &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = textureWidth * textureHeight * textureChannels;

	if (!pixels) {
		throw std::runtime_error("Failed to load image!");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;

	vkMapMemory(logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(logicalDevice, stagingBufferMemory);

	stbi_image_free(pixels);

	CreateImage(textureWidth, textureHeight, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage,
		textureImageMemory);
	

	TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));
	TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	*/
}

void TriangleApp::CreateTextureImageView()
{
	//textureImageView = CreateImageView(textureImage, VK_FORMAT_R8G8B8A8_UNORM);
}

void TriangleApp::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& memory)
{
	VkImageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.extent.width = width;
	createInfo.extent.height = height;
	createInfo.extent.depth = 1;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.format = format;
	createInfo.tiling = tiling;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.usage = usage;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(logicalDevice, &createInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image!");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(logicalDevice, image, &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(logicalDevice, &allocateInfo, nullptr, &memory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate image memory!");
	}

	vkBindImageMemory(logicalDevice, image, memory, 0);
}

VkImageView TriangleApp::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.subresourceRange.aspectMask = aspectFlags;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image view!");
	}

	return imageView;
}

void TriangleApp::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = Command::BeginSingleTimeCommand();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags srcStage;
	VkPipelineStageFlags dstStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::runtime_error("Unsupported Layput Transition!");
	}

	vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	Command::EndSingleTimeCommand(commandBuffer);
}

void TriangleApp::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t imageWidth, uint32_t imageHeight)
{
	VkCommandBuffer commandBuffer = Command::BeginSingleTimeCommand();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferImageHeight = 0;
	region.bufferRowLength = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		imageWidth,
		imageHeight,
		1
	};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	Command::EndSingleTimeCommand(commandBuffer);
}

#pragma endregion

#pragma region Depth Buffer Management

void TriangleApp::CreateDepthResources()
{
	VkFormat depthFormat = FindDepthFormat();

	CreateImage(swapChainExtent.width, swapChainExtent.height,
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		depthImage, depthImageMemory);

	depthImageView = CreateImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

VkFormat TriangleApp::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates) {
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

VkFormat TriangleApp::FindDepthFormat()
{
	return FindSupportedFormat({
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT
		},
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool TriangleApp::HasDepthStencil(VkFormat format)
{
	return (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT);
}

#pragma endregion

#pragma region Swap Chain Management

void TriangleApp::CreateSwapChain()
{
	SwapChainSupportDetails details = QuerySwapChainSupport(physicalDevice);

	//Setup swap chain
	VkExtent2D extent = ChooseSwapExtent(details.capabilities);
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(details.formats);
	VkPresentModeKHR presentMode = ChooseSwapSurfacePresentMode(details.presentModes);

	//Set chain length
	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
		imageCount = details.capabilities.maxImageCount;
	}

	//Setup Swap Chain
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//	-Setup Queue Families
	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
	uint32_t queueFamilies[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilies;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = details.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create swap chain!");
	}

	//Setup swap chain images vector
	imageCount = 0;
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

	//Update aspect ratio
	camera->SetAspectRatio(swapChainExtent.width / (float)swapChainExtent.height);
}

void TriangleApp::RecreateSwapChain()
{
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	//Wait for the device to finish any current processes
	vkDeviceWaitIdle(logicalDevice);

	//Cleanup resources
	CleanupSwapChain();

	//Recreate swap chain
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateDepthResources();
	CreateFrameBuffers();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateDescriptorSets();
	CreateCommandBuffers();
}

void TriangleApp::CleanupSwapChain()
{
	//Destroy Frame Buffers
	for (auto frameBuffer : swapChainFrameBuffers) {
		vkDestroyFramebuffer(logicalDevice, frameBuffer, nullptr);
	}

	//Free Command Buffers
	vkFreeCommandBuffers(logicalDevice, Command::commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

	//Destroy the graphics pipeline
	vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);

	//Destroy the Pipeline Layout
	vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);

	//Destroy Render Pass
	vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

	//Destroy Image Views
	for (VkImageView view : swapChainImageView) {
		vkDestroyImageView(logicalDevice, view, nullptr);
	}

	//Destroy Swap Chain
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

	//Destroy Uniform Buffers
	for (size_t i = 0; i < uniformBuffers.size(); i++) {
		uniformBuffers[i]->Cleanup();
	}

	//Destroy Descriptor Pool
	vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);

	//Destroy Depth Image Views
	vkDestroyImageView(logicalDevice, depthImageView, nullptr);

	//Destroy Depth Image
	vkDestroyImage(logicalDevice, depthImage, nullptr);

	//Free Depth Image Memory
	vkFreeMemory(logicalDevice, depthImageMemory, nullptr);
}

void TriangleApp::CreateImageViews()
{
	swapChainImageView.resize(swapChainImages.size());

	//Create Image Views
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		swapChainImageView[i] = CreateImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void TriangleApp::CreateFrameBuffers()
{
	//Resize vector to have enough space
	swapChainFrameBuffers.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainFrameBuffers.size(); i++) {
		std::array<VkImageView, 2> attachments = {
			swapChainImageView[i],
			depthImageView
		};

		//Setup frame buffer create infos
		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = renderPass;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = swapChainExtent.width;
		createInfo.height = swapChainExtent.height;
		createInfo.layers = 1;

		if (vkCreateFramebuffer(logicalDevice, &createInfo, nullptr, &swapChainFrameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Frame Buffer!");
		}
	}
}

SwapChainSupportDetails TriangleApp::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	//Get Surface Capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	//Get Surface Formats
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	//Get Surface Modes
	uint32_t modeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modeCount, nullptr);
	if (modeCount != 0) {
		details.presentModes.resize(modeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR TriangleApp::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	//Check for a format with the preferred settings
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	//If none of the formats have the preferred settings return the first format
	return availableFormats[0];
}

VkPresentModeKHR TriangleApp::ChooseSwapSurfacePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	//Check for a present mode with the preferred settings
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	//If none of the present modes have the preferred settings return FIFO which is guaranteed to be supported
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D TriangleApp::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
	//If possible pick a resolution that best matches the window within the min and max bounds
	if (surfaceCapabilities.currentExtent.width == UINT32_MAX) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = { 
			static_cast<uint32_t>(width), 
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, actualExtent.height));
		return actualExtent;
	}

	return surfaceCapabilities.currentExtent;
}

#pragma endregion

#pragma region Graphics Pipeline Management

void TriangleApp::CreateGraphicsPipeline()
{
	//Read in shader code
	auto vertexShaderCode = ReadFile("shaders/vert.spv");
	auto fragmentShaderCode = ReadFile("shaders/frag.spv");

	//Create shader module
	VkShaderModule vertexShaderModule = CreateShaderModule(vertexShaderCode);
	VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentShaderCode);

	//Setup shader stages
	VkPipelineShaderStageCreateInfo vertexStageCreateInfo = {};
	vertexStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexStageCreateInfo.module = vertexShaderModule;
	vertexStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragmentStageCreateInfo = {};
	fragmentStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentStageCreateInfo.module = fragmentShaderModule;
	fragmentStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertexStageCreateInfo,
		fragmentStageCreateInfo
	};

	//Setup the Vertex input
	std::array<VkVertexInputAttributeDescription, 3> vertexDescriptions = Vertex::getAttributeDescriptions();
	std::array<VkVertexInputAttributeDescription, 4> transformDescriptions = TransformData::getAttributeDescriptions();
	std::array<VkVertexInputAttributeDescription, 7> attributeDescriptions = {
		vertexDescriptions[0],
		vertexDescriptions[1],
		vertexDescriptions[2],
		transformDescriptions[0],
		transformDescriptions[1],
		transformDescriptions[2],
		transformDescriptions[3]
	};

	std::array<VkVertexInputBindingDescription, 2> bindingDescriptions = {
		Vertex::getBindingDescription(),
		TransformData::getBindingDescription()
	};

	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	vertexInputCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputCreateInfo.pVertexBindingDescriptions = bindingDescriptions.data();

	//Setup the Input Assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	//Setup the Viewport
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	//Setup the Scissor Rectangle
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	//Setup Viewport State
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pScissors = &scissor;

	//Setup Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerCreateInfo.depthClampEnable = VK_FALSE;
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerCreateInfo.lineWidth = 1.0f;
	rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizerCreateInfo.depthBiasClamp = 0.0f;
	rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

	//Setup Multisampling
	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = {};
	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleCreateInfo.minSampleShading = 0.0f;
	multisampleCreateInfo.pSampleMask = nullptr;
	multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

	//Setup Color Blending
	VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
	colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachmentState.blendEnable = VK_FALSE;
	colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	
	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendCreateInfo.attachmentCount = 1;
	colorBlendCreateInfo.pAttachments = &colorBlendAttachmentState;
	colorBlendCreateInfo.blendConstants[0] = 0.0f;
	colorBlendCreateInfo.blendConstants[1] = 0.0f;
	colorBlendCreateInfo.blendConstants[2] = 0.0f;
	colorBlendCreateInfo.blendConstants[3] = 0.0f;

	//Setup depth stencil
	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
	depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilCreateInfo.depthTestEnable = VK_TRUE;
	depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
	depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilCreateInfo.minDepthBounds = 0.0f;
	depthStencilCreateInfo.maxDepthBounds = 1.0f;
	depthStencilCreateInfo.stencilTestEnable = VK_FALSE;
	depthStencilCreateInfo.front = {};
	depthStencilCreateInfo.back = {};

	/*
	//Setup Dynamic states
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.dynamicStateCount = 2;
	dynamicStateCreateInfo.pDynamicStates = dynamicStates;
	*/

	//Setup Pipeline Layout
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	//Create the pipeline layout
	if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Graphics Pipeline Layout!");
	}

	//Setup graphics pipeline create info
	VkGraphicsPipelineCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = 2;
	createInfo.pStages = shaderStages;
	createInfo.pVertexInputState = &vertexInputCreateInfo;
	createInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	createInfo.pViewportState = &viewportStateCreateInfo;
	createInfo.pRasterizationState = &rasterizerCreateInfo;
	createInfo.pMultisampleState = &multisampleCreateInfo;
	createInfo.pDepthStencilState = &depthStencilCreateInfo;
	createInfo.pColorBlendState = &colorBlendCreateInfo;
	createInfo.pDynamicState = nullptr;
	createInfo.layout = pipelineLayout;
	createInfo.renderPass = renderPass;
	createInfo.subpass = 0;
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = -1;

	//Create graphics pipeline
	if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Graphics Pipeline!");
	}

	//Cleanup shader modules
	vkDestroyShaderModule(logicalDevice, vertexShaderModule, nullptr);
	vkDestroyShaderModule(logicalDevice, fragmentShaderModule, nullptr);
}

void TriangleApp::CreateDescriptorSetLayout()
{
	std::vector<VkDescriptorSetLayoutBinding> bindings(1);

	bindings[0] = {};
	bindings[0].binding = 0;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bindings[0].pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = 1;
	createInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(logicalDevice, &createInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor set layout");
	}
}

void TriangleApp::CreateDescriptorPool()
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

void TriangleApp::CreateDescriptorSets()
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

void TriangleApp::CreateRenderPass()
{
	//Setup Color Attachment
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//Setup Depth Attachment
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = FindDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//Setup Subpass
	VkAttachmentReference colorAttachmentReference = {};
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentReference = {};
	depthAttachmentReference.attachment = 1;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription = {};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorAttachmentReference;
	subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
	
	//Setup Subpass Dependencies
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	//Setup attachment array
	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

	//Setup Create Info
	VkRenderPassCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	createInfo.pAttachments = attachments.data();
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpassDescription;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependency;

	//Create Render Pass
	if (vkCreateRenderPass(logicalDevice, &createInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Render Pass!");
	}
}

VkShaderModule TriangleApp::CreateShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Shader Module!");
	}

	return shaderModule;
}

#pragma endregion

#pragma region Mesh Management

void TriangleApp::CreateVertexBuffer(int index)
{
	//Create the staging buffer
	VkDeviceSize bufferSize = sizeof(meshes[index].GetVertices()[0]) * meshes[index].GetVertices().size();
	Buffer stagingBuffer;

	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);
	
	//Map vertex data to the buffer
	void* data;
	vkMapMemory(logicalDevice, stagingBuffer.GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, meshes[index].GetVertices().data(), bufferSize);
	vkUnmapMemory(logicalDevice, stagingBuffer.GetBufferMemory());

	//Create the vertex buffer
	vertexBuffers[index] = std::make_shared<Buffer>();
	meshes[index].SetVertexBuffer(vertexBuffers[index]);
	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *vertexBuffers[index]);

	//Copy buffer data
	Buffer::CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffers[index]->GetBuffer(), bufferSize);

	//Cleanup staging buffer
	stagingBuffer.Cleanup();
}

void TriangleApp::CreateIndexBuffer(int index)
{
	//Create the staging buffer
	VkDeviceSize bufferSize = sizeof(meshes[index].GetIndices()[0]) * meshes[index].GetIndices().size();
	Buffer stagingBuffer;

	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	//Map index data to the buffer
	void* data;
	vkMapMemory(logicalDevice, stagingBuffer.GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, meshes[index].GetIndices().data(), bufferSize);
	vkUnmapMemory(logicalDevice, stagingBuffer.GetBufferMemory());

	//Create the index buffer
	indexBuffers[index] = std::make_shared<Buffer>();
	meshes[index].SetIndexBuffer(indexBuffers[index]);
	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *indexBuffers[index]);

	//Copy buffer data
	Buffer::CopyBuffer(stagingBuffer.GetBuffer(), indexBuffers[index]->GetBuffer(), bufferSize);

	//Cleanup staging buffer
	stagingBuffer.Cleanup();
}

void TriangleApp::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(swapChainImages.size());

	for (size_t i = 0; i < uniformBuffers.size(); i++) {
		uniformBuffers[i] = std::make_shared<Buffer>();
		Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *uniformBuffers[i]);
	}
}

void TriangleApp::UpdateUniformBuffers(uint32_t currentImage)
{
	//Setup the view and projection matrices
	UniformBufferObject ubo = {};
	ubo.view = camera->GetView();
	ubo.projection = camera->GetProjection();

	void* data;
	vkMapMemory(logicalDevice, uniformBuffers[currentImage]->GetBufferMemory(), 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(logicalDevice, uniformBuffers[currentImage]->GetBufferMemory());
}

uint32_t TriangleApp::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::exception("Failed to find suitable memory type!");
}

#pragma endregion

#pragma region Command Buffer Management

void TriangleApp::CreateCommandPool()
{
	//Find Queue families
	QueueFamilyIndices queueFamilies = FindQueueFamilies(physicalDevice);

	//Setup Command Pool
	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = queueFamilies.graphicsFamily.value();
	createInfo.flags = 0;

	if (vkCreateCommandPool(logicalDevice, &createInfo, nullptr, &Command::commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Command Pool!");
	}
}

void TriangleApp::CreateCommandBuffers()
{
	commandBuffers.resize(swapChainFrameBuffers.size());

	VkCommandBufferAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = Command::commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(logicalDevice, &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate Command Buffers");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		//Setup command
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording Command Buffer!");
		}

		//Setup render pass
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = swapChainFrameBuffers[i];
		renderPassBeginInfo.renderArea.extent = swapChainExtent;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };

		std::array<VkClearValue, 2> clearColors = {};
		clearColors[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearColors[1].depthStencil = { 1.0f, 0 };

		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearColors.size());
		renderPassBeginInfo.pClearValues = clearColors.data();

		//Setup commands
		vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		//Begin Per Object Commands 
		//TODO: Move this to work with multiple 
		for (size_t j = 0; j < meshes.size(); j++) {
			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);//Per material

			vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);//Per material

			VkBuffer vertexBuffers[] = { meshes[j].GetVertexBuffer()->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);//Per mesh

			VkBuffer instanceBuffers[] = { meshes[j].GetInstanceBuffer()->GetBuffer() };//Per Mesh
			vkCmdBindVertexBuffers(commandBuffers[i], 1, 1, instanceBuffers, offsets);

			VkBuffer indexBuffers[] = { meshes[j].GetIndexBuffer()->GetBuffer() };
			vkCmdBindIndexBuffer(commandBuffers[i], meshes[j].GetIndexBuffer()->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);//Per mesh

			vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(meshes[j].GetIndices().size()), meshes[j].GetActiveInstanceCount(), 0, 0, 0);//Per mesh
		}
		//End Per object commands

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to end Command Buffer!");
		}
	}
}

#pragma endregion

#pragma region Debug Management

void TriangleApp::SetupDebugMessenger()
{
	//Don't run if validation labels are disabled
	if (!enableValidationLayers) {
		return;
	}

	//Setup the CreateInfo
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	//Set the messenger
	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to setup messenger!");
	}
}

void TriangleApp::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr;
}

bool TriangleApp::CheckValidationLayerSupport()
{
	//Find available layers
	uint32_t validationLayerCount = 0;
	vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(validationLayerCount);
	vkEnumerateInstanceLayerProperties(&validationLayerCount, availableLayers.data());

	//Check for requested layers in the available layer list
	for (const char* layer : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layer, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL TriangleApp::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	//TODO: Figure out how to change text colors to make errors and warnings stand out more	
	//Methods for handling different types of messages unused for the moment so it is commented out
	switch (messageSeverity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		//Diagnostic Messages
		std::cout << "Diagnostic Message: " << std::endl;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		//Informational Messages
		std::cout << "Informational Message: " << std::endl;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		//Warnings
		std::cout << "Warning: " << std::endl;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		//Errors
		std::cout << "Error: " << std::endl;
		break;
	}

	/*switch (messageType) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		//Unrelated events
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		//Violation of specifications, or possible mistake
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		//Performance could be improved
		break;
	}*/

	std::cerr << "\tValidation Layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

#pragma endregion

#pragma region Helper Functions

void TriangleApp::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<TriangleApp*>(glfwGetWindowUserPointer(window));
	app->frameBufferResized = true;
}

std::vector<char> TriangleApp::ReadFile(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file!");
	}

	//Allocate space for the contents of the file
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	//Read the contents of the file
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	//Close the file and return
	file.close();
	return buffer;
}
#pragma endregion
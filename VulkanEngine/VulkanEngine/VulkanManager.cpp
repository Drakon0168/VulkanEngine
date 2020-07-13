#include "pch.h"
#include "VulkanManager.h"

#include "GameManager.h"
#include "WindowManager.h"
#include "DebugManager.h"
#include "EntityManager.h"
#include "SwapChain.h"
#include "Camera.h"

#define mainCamera Camera::GetMainCamera()

#pragma region Singleton

VulkanManager* VulkanManager::instance = nullptr;

VulkanManager* VulkanManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new VulkanManager();
    }

    return instance;
}

#pragma endregion

#pragma region Accessors

VkInstance VulkanManager::GetVulkanInstance()
{
    return vulkanInstance;
}

VkPhysicalDevice VulkanManager::GetPhysicalDevice()
{
    return physicalDevice;
}

VkDevice VulkanManager::GetLogicalDevice()
{
    return logicalDevice;
}

VkSurfaceKHR VulkanManager::GetSurface()
{
    return surface;
}

VkQueue VulkanManager::GetGraphicsQueue()
{
    return graphicsQueue;
}

VkQueue VulkanManager::GetPresentQueue()
{
    return presentQueue;
}

#pragma endregion

#pragma region Helper Methods

uint32_t VulkanManager::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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

QueueFamilyIndices VulkanManager::FindQueueFamilies(VkPhysicalDevice physicalDevice)
{
	QueueFamilyIndices indices = {};

	//Find the queue families
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	//Find a queue that supports graphics operations
	int i = 0;

	for (const auto queueFamily : queueFamilies) {
		//Check for Graphics support
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		//Check for present support
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

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

#pragma endregion

#pragma region Run

void VulkanManager::Run()
{
	if (DebugManager::GetInstance()->GetEnableValidationLayers()) {
		std::cout << "Setting up Window . . ." << std::endl;
	}

	WindowManager::GetInstance()->InitWindow();

	if (DebugManager::GetInstance()->GetEnableValidationLayers()) {
		std::cout << "Setting up Vulkan . . ." << std::endl;
	}

	//Setup Camera
	mainCamera->SetPerspective(true);
	mainCamera->GetTransform()->SetPosition(glm::vec3(0.0f, 2.5f, 5.0f));
	mainCamera->GetTransform()->LookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	InitVulkan();

	if (DebugManager::GetInstance()->GetEnableValidationLayers()) {
		std::cout << "Finished Setup" << std::endl;
	}

	MainLoop();
	Cleanup();

	delete mainCamera;
}

#pragma endregion

#pragma region Memory Management

void VulkanManager::InitVulkan()
{
	//Create application instance
	CreateInstance();

	//Set the debug callback function
	DebugManager::GetInstance()->SetupDebugMessenger();

	//Setup the rendering surface
	CreateSurface();

	//Pick the physical device to use
	PickPhysicalDevice();

	//Create the logical device
	CreateLogicalDevice();

	SwapChain::GetInstance()->CreateSwapChainResources();
}

void VulkanManager::CreateInstance()
{
	if (DebugManager::GetInstance()->GetEnableValidationLayers() && !DebugManager::GetInstance()->CheckValidationLayerSupport()) {
		throw std::runtime_error("Requested validation layer is not available!");
	}

	/*
	Setup App Info
		Provides information about the application to the graphics drivers for better optimization
	*/
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Engine";
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

	std::vector<const char*> validationLayers = DebugManager::GetInstance()->GetValidationLayers();

	//Add validation layers if they are enabled
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (validationLayers.size() > 0) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(DebugManager::GetInstance()->GetValidationLayers().size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		DebugManager::GetInstance()->PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	//Create the instance
	if (vkCreateInstance(&createInfo, nullptr, &vulkanInstance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance!");
	}
}

void VulkanManager::CreateLogicalDevice()
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

	std::vector<const char*> enabledLayers = DebugManager::GetInstance()->GetValidationLayers();

	if (enabledLayers.size() > 0) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(DebugManager::GetInstance()->GetValidationLayers().size());
		createInfo.ppEnabledLayerNames = enabledLayers.data();
	}
	else {
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

void VulkanManager::CreateSurface()
{
	if (glfwCreateWindowSurface(vulkanInstance, WindowManager::GetInstance()->GetWindow(), nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}

void VulkanManager::Cleanup()
{
	//Cleanup Swap Chain and associated resources
	SwapChain::GetInstance()->FullCleanup();

	//Cleanup Meshes
	EntityManager::GetInstance()->CleanupMeshes();

	//Destroy Logical Device
	vkDestroyDevice(logicalDevice, nullptr);

	//Cleanup Debug Manager
	DebugManager::GetInstance()->Cleanup();

	//Destroy Surface
	vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);

	//Destroy Vulkan resources
	vkDestroyInstance(vulkanInstance, nullptr);

	//Destroy GLFW resources
	glfwDestroyWindow(WindowManager::GetInstance()->GetWindow());

	//Terminate the window
	glfwTerminate();
}

#pragma endregion

#pragma region MainLoop

void VulkanManager::MainLoop()
{
	//Set starting time values
	Time::Reset();

	//Initialize GameManager
	GameManager::GetInstance()->Init();

	//Loop until the window is closed
	while (!glfwWindowShouldClose(WindowManager::GetInstance()->GetWindow())) {
		glfwPollEvents();

		//Calculate time change per frame
		Time::Update();

		Update();
		Draw();
	}

	vkDeviceWaitIdle(logicalDevice);
}

void VulkanManager::Draw()
{
	uint32_t imageIndex = SwapChain::GetInstance()->BeginDraw();

	if (imageIndex != -1) {
		//Re-record command buffer
		EntityManager::GetInstance()->Draw(imageIndex, SwapChain::GetInstance()->GetCommandBuffer(imageIndex));

		SwapChain::GetInstance()->EndDraw(imageIndex);
	}
}

void VulkanManager::Update()
{
	GameManager::GetInstance()->Update();

	EntityManager::GetInstance()->Update();
}

#pragma endregion

#pragma region Physical Device Management

std::vector<const char*> VulkanManager::GetRequiredExtensions()
{
	//Find extensions required by GLFW
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//If validation layers are enabled add Debug Utilities to required extension list
	if (DebugManager::GetInstance()->GetEnableValidationLayers()) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void VulkanManager::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPU's with Vulkan support!");
	}

	//Get physical devices
	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, physicalDevices.data());

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

bool VulkanManager::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

	//Ensure that all required extensions are on the available extensions list
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end()); //Make a copy of required extensions list
	for (auto extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName); //If the current extension is on the copied list, erase it
	}

	return requiredExtensions.empty(); //If the copied list is empty all required extensions have been found
}

int VulkanManager::RateDevice(VkPhysicalDevice physicalDevice)
{
	//Get Device Properties
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

	//Get Device Features
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

	//Check for base specifications needed to run the program, if these are not met return 0
	if (!IsDeviceSuitable(physicalDevice, deviceFeatures)) {
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

bool VulkanManager::IsDeviceSuitable(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures deviceFeatures)
{
	//Device does not support geometry shaders
	if (!deviceFeatures.geometryShader) {
		return false;
	}

	//Device cannot process graphics commands
	QueueFamilyIndices queueFamilies = FindQueueFamilies(physicalDevice);
	if (!queueFamilies.IsConplete()) {
		return false;
	}

	//Make sure that required extensions are supported
	if (!CheckDeviceExtensionSupport(physicalDevice)) {
		return false;
	}

	//Make sure the swap chain is supported
	SwapChainSupportDetails swapChainSupport = SwapChain::GetInstance()->QuerySwapChainSupport(physicalDevice);
	if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
		return false;
	}

	return true;
}

#pragma endregion
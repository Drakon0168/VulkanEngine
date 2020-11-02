#include "pch.h"
#include "SwapChain.h"

#include "VulkanManager.h"
#include "DebugManager.h"
#include "EntityManager.h"
#include "GameManager.h"
#include "WindowManager.h"
#include "Camera.h"
#include "GuiManager.h"

#define logicalDevice VulkanManager::GetInstance()->GetLogicalDevice()
#define physicalDevice VulkanManager::GetInstance()->GetPhysicalDevice()

#pragma region Singleton

SwapChain* SwapChain::instance = nullptr;

SwapChain* SwapChain::GetInstance()
{
    if (instance == nullptr) {
        instance = new SwapChain();
    }

    return instance;
}

#pragma endregion

#pragma region Accessors

VkExtent2D SwapChain::GetExtents()
{
    return  extent;
}

VkRenderPass SwapChain::GetRenderPass()
{
	return renderPass;
}

std::vector<Buffer> SwapChain::GetUniformBuffers()
{
	return uniformBuffers;
}

Image SwapChain::GetDepthImage()
{
	return depthImage;
}

std::vector<VkImage> SwapChain::GetImages()
{
	return images;
}

std::vector<VkImageView> SwapChain::GetImageViews()
{
	return imageViews;
}

std::vector<VkFramebuffer> SwapChain::GetFrameBuffers()
{
	return frameBuffers;
}

VkFormat SwapChain::GetFormat()
{
	return imageFormat;
}

bool SwapChain::GetFrameBufferResized()
{
	return frameBufferResized;
}

void SwapChain::SetFrameBufferResized(bool value)
{
	frameBufferResized = value;
}

VkCommandPool SwapChain::GetCommandPool()
{
	return commandPool;
}

std::vector<VkCommandBuffer> SwapChain::GetCommandBuffers()
{
	return commandBuffers;
}

VkCommandBuffer* SwapChain::GetCommandBuffer(uint32_t index)
{
	return &commandBuffers[index];
}


#pragma endregion

#pragma region Memory Management

void SwapChain::CreateSwapChainResources()
{
	//Create the swap chain
	CreateSwapChain();

	//Create the image views
	CreateImageViews();

	//Create the render pass
	CreateRenderPass();

	//Create Depth Buffer resources
	CreateDepthResources();

	//Create the frame buffers
	CreateFrameBuffers();

	//Create the command pool
	CreateCommandPool();

	//Create Uniform Buffers
	CreateUniformBuffers();

	//Load Meshes and materials
	EntityManager::GetInstance()->Init();

	//Initialize the debug manager
	DebugManager::GetInstance()->Init();

	//Create material resources
	EntityManager::GetInstance()->CreateMaterialResources();

	//Setup Meshes and Materials
	EntityManager::GetInstance()->CreateMeshResources();

	//Create the Command Buffers
	CreateCommandBuffers();

	//Create the Semaphores and Fences
	CreateSyncObjects();
}

void SwapChain::CreateSwapChain()
{
	SwapChainSupportDetails details = QuerySwapChainSupport(physicalDevice);

	//Setup swap chain
	//VkExtent2D extent = ChooseSwapExtent(details.capabilities);
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(details.formats);
	VkPresentModeKHR presentMode = ChooseSwapSurfacePresentMode(details.presentModes);
	VkExtent2D extent = ChooseSwapExtent(details.capabilities);

	//Set chain length
	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
		imageCount = details.capabilities.maxImageCount;
	}

	//Setup Swap Chain
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = VulkanManager::GetInstance()->GetSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//	-Setup Queue Families
	QueueFamilyIndices indices = VulkanManager::GetInstance()->FindQueueFamilies(physicalDevice);
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

	imageFormat = surfaceFormat.format;
	this->extent = extent;

	if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create swap chain!");
	}

	//Setup swap chain images vector
	imageCount = 0;
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, images.data());

	//Update aspect ratio
	Camera::GetMainCamera()->SetAspectRatio(this->extent.width / (float)this->extent.height);
}

void SwapChain::RecreateSwapChain()
{
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(WindowManager::GetInstance()->GetWindow(), &width, &height);
		glfwWaitEvents();
	}

	//Wait for the device to finish any current processes
	vkDeviceWaitIdle(logicalDevice);

	//Cleanup resources
	Cleanup();

	//Recreate swap chain
	CreateSwapChain();

	CreateImageViews();

	CreateRenderPass();

	CreateDepthResources();

	CreateFrameBuffers();

	CreateUniformBuffers();

	//Re-create materials
	EntityManager::GetInstance()->CreateMaterialResources();

	CreateCommandBuffers();
}

void SwapChain::Cleanup()
{

	//Destroy Frame Buffers
	for (auto frameBuffer : frameBuffers) {
		vkDestroyFramebuffer(logicalDevice, frameBuffer, nullptr);
	}

	//Free Command Buffers
	vkFreeCommandBuffers(logicalDevice, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	//Cleanup Materials
	EntityManager::GetInstance()->CleanupMaterials();

	//Destroy Render Pass
	vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

	//Destroy Image Views
	for (VkImageView view : imageViews) {
		vkDestroyImageView(logicalDevice, view, nullptr);
	}

	//Destroy Swap Chain
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

	//Destroy Uniform Buffers
	for (size_t i = 0; i < uniformBuffers.size(); i++) {
		uniformBuffers[i].Cleanup();
	}

	//Cleanup Depth Image
	depthImage.Cleanup();

}

void SwapChain::FullCleanup()
{
	GuiManager::GetInstance()->FullCleanup();
	Cleanup();

	//Destroy Command Pool
	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

	//Destroy Semaphores
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
	}
}

void SwapChain::CreateImageViews()
{
	imageViews.resize(images.size());

	//Create Image Views
	for (size_t i = 0; i < images.size(); i++) {
		imageViews[i] = Image::CreateImageView(images[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT,1);
	}
	
}

void SwapChain::CreateFrameBuffers()
{
	//Resize vector to have enough space
	frameBuffers.resize(images.size());

	for (size_t i = 0; i < frameBuffers.size(); i++) {
		std::array<VkImageView, 2> attachments = {
			imageViews[i],
			*depthImage.GetView()
		};

		//Setup frame buffer create infos
		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = renderPass;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = extent.width;
		createInfo.height = extent.height;
		createInfo.layers = 1;

		if (vkCreateFramebuffer(logicalDevice, &createInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Frame Buffer!");
		}
	}
}

void SwapChain::CreateDepthResources()
{
	VkFormat depthFormat = FindDepthFormat();

	Image::CreateImage(1,extent.width, extent.height,
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		depthImage);

	Image::CreateImageView(&depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT,1);
}

void SwapChain::CreateSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(images.size(), VK_NULL_HANDLE);

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

void SwapChain::CreateRenderPass()
{
	//Setup Color Attachment
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = GetFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	if (VulkanManager::GetInstance()->initGui)
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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

void SwapChain::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(images.size());

	for (size_t i = 0; i < uniformBuffers.size(); i++) {
		uniformBuffers[i] = Buffer();
		Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i]);
	}
}

void SwapChain::CreateCommandPool()
{
	//Find Queue families
	QueueFamilyIndices queueFamilies = VulkanManager::GetInstance()->FindQueueFamilies(physicalDevice);

	//Setup Command Pool
	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = queueFamilies.graphicsFamily.value();
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(logicalDevice, &createInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Command Pool!");
	}
	/*if (vkCreateCommandPool(logicalDevice, &createInfo, nullptr, &imGuiCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Command Pool!");
	}*/
}


void SwapChain::CreateCommandBuffers()
{
	commandBuffers.resize(GetFrameBuffers().size());
	// imGuiCommandBuffers.resize(GetFrameBuffers().size());

	VkCommandBufferAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(logicalDevice, &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate Command Buffers");
	}


	/*allocateInfo.commandPool = imGuiCommandPool; 
	allocateInfo.commandBufferCount = (uint32_t)imGuiCommandBuffers.size();
	if (vkAllocateCommandBuffers(logicalDevice, &allocateInfo, imGuiCommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate Command Buffers");
	}*/



	for (uint32_t i = 0; i < commandBuffers.size(); i++) {
		EntityManager::GetInstance()->Draw(i, &commandBuffers[i]);
	}
	// initial draw for GUI buffers here?
}

#pragma endregion

#pragma region Game Loop

void SwapChain::UpdateUniformBuffer(uint32_t imageIndex)
{
	//Setup the view and projection matrices
	UniformBufferObject ubo = {};
	ubo.view = Camera::GetMainCamera()->GetView();
	ubo.projection = Camera::GetMainCamera()->GetProjection();
	ubo.cameraPosition = Camera::GetMainCamera()->GetTransform()->GetPosition();

	std::vector<std::shared_ptr<Light>> lights = GameManager::GetInstance()->GetLights();
	for (int i = 0; i < lights.size(); i++) {
		if (i >= 5) {
			break;
		}

		ubo.lights[i] = *lights[i];
	}


	void* data;
	vkMapMemory(logicalDevice, uniformBuffers[imageIndex].GetBufferMemory(), 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(logicalDevice, uniformBuffers[imageIndex].GetBufferMemory());
}

uint32_t SwapChain::BeginDraw()
{
	//Wait for the fence to finish
	vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	//Find the index of the next image
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain();
		return -1;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to aquire next swap chain image!");
	}

	//Update uniform buffers
	UpdateUniformBuffer(imageIndex);

	//Mark the image as being in use
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	return imageIndex;
}

void SwapChain::EndDraw(uint32_t imageIndex)
{
	//Submit to the graphics queue
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	// Check if GUI was initialized
	if (GuiManager::GetInstance()->guiInitialized) {
		std::array<VkCommandBuffer, 2> submitCommandBuffers =
		{ commandBuffers[imageIndex], GuiManager::GetInstance()->GetCommandBuffers()[imageIndex] };
		submitInfo.commandBufferCount = 2;
		submitInfo.pCommandBuffers = submitCommandBuffers.data();
	}
	else {
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
	}

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	//Make sure the image is not already in use before submitting it
	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(logicalDevice, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	//Reset fence
	vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);
	if (vkQueueSubmit(VulkanManager::GetInstance()->GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) { //stops here VK_DEVICE_LOST
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

	VkResult result = vkQueuePresentKHR(VulkanManager::GetInstance()->GetPresentQueue(), &presentInfo);

	if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR || frameBufferResized) {
		RecreateSwapChain();
		GuiManager::GetInstance()->RecreateResources();
		SetFrameBufferResized(false);
	}
	else if (result != VK_SUCCESS) { // stops here
		throw std::runtime_error("Failed to present swap chain image!");
	}

	//Update the current frame
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

#pragma endregion

#pragma region Depth Image

VkFormat SwapChain::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
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

VkFormat SwapChain::FindDepthFormat()
{
	return FindSupportedFormat({
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT
		},
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool SwapChain::HasDepthStencil(VkFormat format)
{
	return (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT);
}

#pragma endregion

#pragma region Support Checks

SwapChainSupportDetails SwapChain::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	//Get Surface Capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, VulkanManager::GetInstance()->GetSurface(), &details.capabilities);

	//Get Surface Formats
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, VulkanManager::GetInstance()->GetSurface(), &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, VulkanManager::GetInstance()->GetSurface(), &formatCount, details.formats.data());
	}

	//Get Surface Modes
	uint32_t modeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, VulkanManager::GetInstance()->GetSurface(), &modeCount, nullptr);
	if (modeCount != 0) {
		details.presentModes.resize(modeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, VulkanManager::GetInstance()->GetSurface(), &modeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
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

VkPresentModeKHR SwapChain::ChooseSwapSurfacePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
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

VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
	//If possible pick a resolution that best matches the window within the min and max bounds
	if (surfaceCapabilities.currentExtent.width == UINT32_MAX) {
		int width, height;
		glfwGetFramebufferSize(WindowManager::GetInstance()->GetWindow(), &width, &height);

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
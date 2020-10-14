#include "pch.h"
#include "SwapChain.h"

#include "VulkanManager.h"
#include "EntityManager.h"
#include "GameManager.h"
#include "WindowManager.h"
#include "Camera.h"
#include "TextureImages.h"
#include "GuiManager.h"

#define logicalDevice VulkanManager::GetInstance()->GetLogicalDevice()
#define physicalDevice VulkanManager::GetInstance()->GetPhysicalDevice()
static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}
void GuiManager::InitImGui()
{
	// SETUP IMGUI!!!
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForVulkan(WindowManager::GetInstance()->GetWindow(), true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = VulkanManager::GetInstance()->GetVulkanInstance();
	init_info.PhysicalDevice = physicalDevice;
	init_info.Device = logicalDevice;
	init_info.QueueFamily = 0;
	init_info.Queue = VulkanManager::GetInstance()->GetGraphicsQueue();
	init_info.PipelineCache = VK_NULL_HANDLE;
	// Create Descriptor Pool
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		vkCreateDescriptorPool(logicalDevice, &pool_info, nullptr, &g_DescriptorPool);
	}
	init_info.DescriptorPool = g_DescriptorPool;
	init_info.Allocator = nullptr;
	init_info.MinImageCount = 2;
	init_info.ImageCount = SwapChain::GetInstance()->GetImages().size();
	init_info.CheckVkResultFn = check_vk_result;

#pragma region RenderPass


	// Create a render pass
	// Create the main pass attachment
	VkAttachmentDescription attachment = {};
	attachment.format = SwapChain::GetInstance()->GetFormat();
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Create the color attachment
	VkAttachmentReference color_attachment = {};
	color_attachment.attachment = 0;
	color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	// Create the subpass
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment;

	// Create a dependency to synchronize the render passes.  We only want this to draw if there are pixels on the screen
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.attachmentCount = 1;
	info.pAttachments = &attachment;
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.dependencyCount = 1;
	info.pDependencies = &dependency;
	if (vkCreateRenderPass(logicalDevice, &info, nullptr, &imGuiRenderPass) != VK_SUCCESS) {
		throw std::runtime_error("Could not create Dear ImGui's render pass");
	}
	ImGui_ImplVulkan_Init(&init_info, imGuiRenderPass);
#pragma endregion

	guiInitialized = true;

	CreateFrameBuffers();
	CreateCommandPool();
	CreateCommandBuffers();
	VkCommandBuffer cb = CommandBuffer::BeginSingleTimeCommand();
	ImGui_ImplVulkan_CreateFontsTexture(cb);
	CommandBuffer::EndSingleTimeCommand(cb);
}
void GuiManager::Draw(uint32_t imageIndex)
{
	VkCommandBuffer* commandBuffer = &(imGuiCommandBuffers[imageIndex]);
	if (vkResetCommandBuffer(*commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) != VK_SUCCESS) {
		throw std::runtime_error("Failed to reset command buffer!");
	}

	//Setup command
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(*commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording Command Buffer!");
	}

	//Setup render pass
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = imGuiRenderPass;
	renderPassBeginInfo.framebuffer = guiFrameBuffers[imageIndex];
	renderPassBeginInfo.renderArea.extent = SwapChain::GetInstance()->GetExtents();
	renderPassBeginInfo.renderArea.offset = { 0, 0 };

	std::array<VkClearValue, 1> clearColors = {};
	clearColors[0] = { 0.0f, 0.0f, 0.0f, 1.0f };

	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearColors.size());
	renderPassBeginInfo.pClearValues = clearColors.data();

	// Gui Render information
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	DrawGUI();
	ImGui::Render();

	//Setup commands
	vkCmdBeginRenderPass(*commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	if (guiInitialized == true) {
		ImDrawData* draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandBuffer);

	}

	vkCmdEndRenderPass(*commandBuffer);

	if (vkEndCommandBuffer(*commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to end Command Buffer!");
	}
}
void GuiManager::DrawGUI()
{
	static ImVec4 v4Color = ImColor(255, 0, 0);
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
	ImGui::SetNextWindowPos(ImVec2(1, 1), 0);
	ImGui::SetNextWindowSize(ImVec2(340, 120), 0);
	// tring sAbout = m_pSystem->GetAppName() + " - About";
	ImGui::Begin("About", (bool*)0, window_flags);
	{
		ImGui::Text("Programmer: \n");
		ImGui::TextColored(v4Color, "Vulkan Team");
		ImGui::Text("FrameRate: %.2f [FPS] -> %.3f [ms/frame]\n",
			ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Separator();
		ImGui::Text("Control:\n");
		ImGui::Text(" WASDQE: Movement\n");
		ImGui::Text(" Right Click: Rotation toggle\n");
	}
	ImGui::End();
}
std::vector<VkFramebuffer> GuiManager::GetFrameBuffers(void)
{
	return guiFrameBuffers;
}
void GuiManager::FullCleanup()
{
	if (guiInitialized) {
		Cleanup();
		vkDestroyRenderPass(logicalDevice, imGuiRenderPass, nullptr);


		
		// ImGui Resources to destroy
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		vkDestroyCommandPool(logicalDevice, imGuiCommandPool, nullptr);
		vkDestroyDescriptorPool(logicalDevice, g_DescriptorPool, nullptr);
	}

	
}
void GuiManager::Cleanup()
{
	//Destroy Frame Buffers
	for (auto frameBuffer : guiFrameBuffers) {
		vkDestroyFramebuffer(logicalDevice, frameBuffer, nullptr);
	}
	vkFreeCommandBuffers(logicalDevice, imGuiCommandPool, static_cast<uint32_t>(imGuiCommandBuffers.size()), imGuiCommandBuffers.data());
}
std::vector<VkCommandBuffer> GuiManager::GetCommandBuffers()
{
	return imGuiCommandBuffers;
}
VkCommandPool GuiManager::GetCommandPool()
{
	return imGuiCommandPool;
}
void GuiManager::CreateCommandPool()
{
	//Find Queue families
	QueueFamilyIndices queueFamilies = VulkanManager::GetInstance()->FindQueueFamilies(physicalDevice);

	//Setup Command Pool
	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = queueFamilies.graphicsFamily.value();
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(logicalDevice, &createInfo, nullptr, &imGuiCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Command Pool!");
	}
}
void GuiManager::CreateCommandBuffers()
{
	imGuiCommandBuffers.resize(GetFrameBuffers().size());

	VkCommandBufferAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandPool = imGuiCommandPool;
	allocateInfo.commandBufferCount = (uint32_t)imGuiCommandBuffers.size();
	if (vkAllocateCommandBuffers(logicalDevice, &allocateInfo, imGuiCommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate Command Buffers");
	}

	// initial draw for GUI buffers here?
}
void GuiManager::RecreateResources()
{
	Cleanup();
	CreateFrameBuffers();
	CreateCommandBuffers();
}
void GuiManager::CreateFrameBuffers()
{
	//Resize vector to have enough space
	guiFrameBuffers.resize(SwapChain::GetInstance()->GetImages().size());

	for (size_t i = 0; i < guiFrameBuffers.size(); i++) {
		std::array<VkImageView, 1> attachments = {
			SwapChain::GetInstance()->GetImageViews()[i]
		};
		//Setup frame buffer create infos
		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = imGuiRenderPass;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();

		createInfo.width = SwapChain::GetInstance()->GetExtents().width;
		createInfo.height = SwapChain::GetInstance()->GetExtents().height;
		createInfo.layers = 1;

		if (vkCreateFramebuffer(VulkanManager::GetInstance()->GetLogicalDevice(), &createInfo, nullptr, &guiFrameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Frame Buffer!");
		}
	}
}
GuiManager* GuiManager::GetInstance(void)
{
    if (instance == nullptr) {
        instance = new GuiManager();
    }

    return instance;
}

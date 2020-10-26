#pragma once
#include "pch.h"

#include "Image.h"
#include "Buffer.h"
class GuiManager
{
private:
	VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> guiFrameBuffers;
	VkRenderPass imGuiRenderPass;

	VkCommandPool imGuiCommandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> imGuiCommandBuffers;


	inline static GuiManager* instance;

	void CreateFrameBuffers();

public:
	bool guiInitialized = false;
	static GuiManager* GetInstance(void);
	// Helper methods
	void InitImGui(void);
	void Draw(uint32_t imageIndex);
	void DrawGUI();

	std::vector<VkFramebuffer> GetFrameBuffers(void);
	void FullCleanup();
	void Cleanup();

	/// <summary>
	/// Returns the list of command buffers used by the swap chain
	/// </summary>
	/// <returns>The list of command buffers</returns>
	std::vector<VkCommandBuffer> GetCommandBuffers();

	/// <summary>
	/// Returns the command pool being used by the application
	/// </summary>
	/// <returns>The command pool that is in use</returns>
	VkCommandPool GetCommandPool();


	/// <summary>
	/// Creates and allocates the command pool
	/// </summary>
	void CreateCommandPool();


	/// <summary>
	/// Creates and allocates the command buffers
	/// </summary>
	void CreateCommandBuffers();

	void RecreateResources();
};


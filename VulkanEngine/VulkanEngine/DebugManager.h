#pragma once
#include "pch.h"

class DebugManager
{
private:

	static DebugManager* instance;

	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

public:
#pragma region Singleton

	/// <summary>
	/// Returns the singleton instance of the debug manager
	/// </summary>
	/// <returns>The instance of the debug manager</returns>
	static DebugManager* GetInstance();

#pragma endregion

#pragma region Memory Management

	/// <summary>
	/// Cleans up Debug manager resources
	/// </summary>
	void Cleanup();

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns whether or not validation layers are enabled
	/// </summary>
	/// <returns>True if validation layers are enabled</returns>
	bool GetEnableValidationLayers();

	/// <summary>
	/// Returns the currently enabled validation layers
	/// </summary>
	/// <returns>The currently enabled validation layers</returns>
	std::vector<const char*> GetValidationLayers();

#pragma endregion

#pragma region Debug Messenger

	/// <summary>
	/// Creates and allocates the Debug Messenger
	/// </summary>
	void SetupDebugMessenger();

	/// <summary>
	/// Fills in the Debug Messenger create info
	/// </summary>
	/// <param name="createInfo">The create info to fill in</param>
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT & createInfo);

	/// <summary>
	/// Checks that the requested validation layers are supported 
	/// </summary>
	/// <returns>True if the requested layers are supported</returns>
	bool CheckValidationLayerSupport();

	/// <summary>
	/// Callback method that is called whenever one of the validation layers sends out a message
	/// </summary>
	/// <param name="messageSeverity">The severity of the message (warning, error, diagnostic, etc)</param>
	/// <param name="messageType">The type of message (general, validation, performance)</param>
	/// <param name="pCallbackData">The message data</param>
	/// <param name="pUserData"></param>
	/// <returns></returns>
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

#pragma endregion
};
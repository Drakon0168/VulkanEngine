#pragma once
#include "pch.h"

#include "Mesh.h"
#include "Buffer.h"

class DebugManager
{
private:

	static DebugManager* instance;

	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	std::map<std::shared_ptr<Mesh>, std::vector<std::shared_ptr<DebugShape>>> debugShapes;
	std::map<std::shared_ptr<Mesh>, std::shared_ptr<Buffer>> instanceBuffers;
	std::map<std::shared_ptr<Mesh>, bool> instanceBufferDirty;
	bool drawHandles = false;
	
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
	/// Initializes the debug manager's resources
	/// </summary>
	void Init();

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

	/// <summary>
	/// Returns the instance buffer map used by the debug shapes
	/// </summary>
	/// <returns>The instance buffer map</returns>
	std::map<std::shared_ptr<Mesh>, std::shared_ptr<Buffer>> GetInstanceBuffers();

	/// <summary>
	/// Returns whether or not to draw handles
	/// </summary>
	/// <returns>True if handles are enabled otherwise false</returns>
	bool GetDrawHandles();

#pragma endregion

#pragma region DebugShapes

	/// <summary>
	/// Creates the instance buffer for the specified mesh
	/// </summary>
	/// <param name="mesh">The mesh to make an instance buffer for</param>
	void CreateInstanceBuffer(std::shared_ptr<Mesh> mesh);

	/// <summary>
	/// Update the instance buffer of the mesh
	/// </summary>
	/// <param name="mesh">The mesh to update the instance buffer</param>
	void UpdateInstanceBuffer(std::shared_ptr<Mesh> mesh);

	/// <summary>
	/// Draws a wireframe sphere at the specified location
	/// </summary>
	/// <param name="position">The position to draw the sphere</param>
	/// <param name="color">The color of the wireframe sphere</param>
	/// <param name="radius">The radius of the wireframe sphere</param>
	/// <param name="duration">The duration to draw the sphere for, -1 to draw indefinetly, 0 for only the current frame</param>
	void DrawWireSphere(glm::vec3 position, glm::vec3 color, float radius = 1.0f, float duration = -1.0f);

	/// <summary>
	/// Draws a wireframe cube at the specified position
	/// </summary>
	/// <param name="position">The position to draw the cube</param>
	/// <param name="color">The color of the wireframe cube</param>
	/// <param name="size">The length width and height of the cube</param>
	/// <param name="duration">The duration to draw the cube for, -1 to draw indefinetly, 0 for only the current frame</param>
	void DrawWireCube(glm::vec3 position, glm::vec3 color, glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f), float duration = -1.0f);

	/// <summary>
	/// Draws a line between the specified points
	/// </summary>
	/// <param name="position1">The position to start the line from</param>
	/// <param name="position2">The position to end the line at</param>
	/// <param name="color">The color to draw the line in</param>
	/// <param name="duration">The duration to draw the line for, -1 to draw indefinetly, 0 for only the current frame</param>
	void DrawLine(glm::vec3 position1, glm::vec3 position2, glm::vec3 color, float duration = -1.0f);

	/// <summary>
	/// Removes a shape from the list of debug shapes
	/// </summary>
	/// <param name="mesh">The mesh used by the shape</param>
	/// <param name="index">The index of the shape to remove</param>
	void RemoveShape(std::shared_ptr<Mesh> mesh, int index);

	/// <summary>
	/// Adds a debug shape the the list for the specified mesh
	/// </summary>
	/// <param name="mesh">The mesh to add the shape to</param>
	/// <param name="shape">The shape to add</param>
	void AddShape(std::shared_ptr<Mesh> mesh, std::shared_ptr<DebugShape> shape);

#pragma endregion

#pragma region Update

	void Update();

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
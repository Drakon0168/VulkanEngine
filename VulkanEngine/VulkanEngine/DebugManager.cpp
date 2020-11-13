#include "pch.h"
#include "DebugManager.h"

#include "EntityManager.h"
#include "InputManager.h"
#include "VulkanManager.h"

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

#pragma region Singleton

DebugManager* DebugManager::instance = nullptr;

DebugManager* DebugManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new DebugManager();
	}

	return instance;
}

#pragma endregion

#pragma region Memory Management

void DebugManager::Init()
{
#ifdef DEBUG 
	std::vector<std::shared_ptr<Mesh>> debugMeshes;
	debugMeshes.push_back(EntityManager::GetInstance()->GetMeshes()[MeshTypes::WireSphere]);
	debugMeshes.push_back(EntityManager::GetInstance()->GetMeshes()[MeshTypes::WireCube]);
	debugMeshes.push_back(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Line]);

	for (int i = 0; i < debugMeshes.size(); i++) {
		debugShapes.insert(std::pair<std::shared_ptr<Mesh>, std::vector<std::shared_ptr<DebugShape>>>(debugMeshes[i], std::vector<std::shared_ptr<DebugShape>>()));
		instanceBuffers.insert(std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Buffer>>(debugMeshes[i], nullptr));
		instanceBufferDirty.insert(std::pair<std::shared_ptr<Mesh>, bool>(debugMeshes[i], true));
	}
#endif
}

void DebugManager::Cleanup()
{
#ifdef DEBUG
	for (std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Buffer>> pair : instanceBuffers) {
		if (pair.second != nullptr) {
			pair.second->Cleanup();
		}
		
	}
#endif

	if (enableValidationLayers) {
		// The ifdef was not working so I moved code here.  Validation error fixed
		for (std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Buffer>> pair : instanceBuffers) {
			if (pair.second != nullptr) {
				pair.second->Cleanup();
			}

		}
		DestroyDebugUtilsMessengerEXT(VulkanManager::GetInstance()->GetVulkanInstance(), debugMessenger, nullptr);
	}
}

#pragma endregion

#pragma region Accessors

bool DebugManager::GetEnableValidationLayers()
{
	return enableValidationLayers;
}

std::vector<const char*> DebugManager::GetValidationLayers()
{
	if (enableValidationLayers) {
		return validationLayers;
	}

	return {};
}

std::map<std::shared_ptr<Mesh>, std::shared_ptr<Buffer>> DebugManager::GetInstanceBuffers()
{
	return instanceBuffers;
}

bool DebugManager::GetDrawHandles()
{
	return drawHandles;
}

#pragma endregion

#pragma region DebugShapes

void DebugManager::CreateInstanceBuffer(std::shared_ptr<Mesh> mesh)
{
	if (enableValidationLayers)
	{
		//Create buffer
		VkDeviceSize bufferSize = sizeof(glm::vec3);
		instanceBuffers[mesh] = std::make_shared<Buffer>(VkBuffer(), VkDeviceMemory());
		Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *instanceBuffers[mesh]);

		//Data will be added to the buffer in UpdateInstanceBuffer method once we have data to add
	}
}

void DebugManager::UpdateInstanceBuffer(std::shared_ptr<Mesh> mesh)
{
	if (enableValidationLayers)
	{
		//If a new object has been spawned or deleted the instance buffer must be re-created to the correct size
		if (instanceBufferDirty[mesh]) {
			vkQueueWaitIdle(VulkanManager::GetInstance()->GetGraphicsQueue());
			instanceBuffers[mesh]->Cleanup();
			instanceBuffers[mesh] = std::make_shared<Buffer>(VkBuffer(), VkDeviceMemory());
		}

		//Get Color data
		std::vector<glm::vec3> bufferData;

		for (size_t i = 0; i < debugShapes[mesh].size(); i++) {
			if (debugShapes[mesh][i] != nullptr) {
				bufferData.push_back(debugShapes[mesh][i]->color);
			}
		}

		//Ensure that buffer size is not 0
		VkDeviceSize bufferSize;
		if (bufferData.size() > 0) {
			bufferSize = sizeof(glm::vec3) * bufferData.size();
		}
		else {
			bufferData.push_back(glm::vec3(0, 0, 0));
			bufferSize = sizeof(glm::vec3);
		}

		//Create the buffer if necessary
		if (instanceBufferDirty[mesh]) {
			Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *instanceBuffers[mesh]);
		}
		//Copy Data
		void* data;
		vkMapMemory(VulkanManager::GetInstance()->GetLogicalDevice(), instanceBuffers[mesh]->GetBufferMemory(), 0, bufferSize, 0, &data);
		memcpy(data, bufferData.data(), bufferSize);
		vkUnmapMemory(VulkanManager::GetInstance()->GetLogicalDevice(), instanceBuffers[mesh]->GetBufferMemory());

		instanceBufferDirty[mesh] = false;
	}
}

void DebugManager::DrawWireSphere(glm::vec3 position, glm::vec3 color, float radius, float duration)
{
	if (enableValidationLayers)
	{
		std::shared_ptr<Mesh> mesh = EntityManager::GetInstance()->GetMeshes()[MeshTypes::WireSphere];

		std::shared_ptr<DebugShape> shape = std::make_shared<DebugShape>();
		shape->transform = std::make_shared<Transform>(position, glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(radius / 0.5f, radius / 0.5f, radius / 0.5f));
		shape->color = color;
		shape->duration = duration;
		shape->meshID = mesh->AddInstance(shape->transform);

		AddShape(mesh, shape);

		if (instanceBuffers[mesh] == nullptr) {
			CreateInstanceBuffer(mesh);
		}
	}
}

void DebugManager::DrawWireCube(glm::vec3 position, glm::vec3 color, glm::vec3 size, float duration)
{
	if (enableValidationLayers)
	{
		std::shared_ptr<Mesh> mesh = EntityManager::GetInstance()->GetMeshes()[MeshTypes::WireCube];

		std::shared_ptr<DebugShape> shape = std::make_shared<DebugShape>();
		shape->transform = std::make_shared<Transform>(position, glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), size);
		shape->color = color;
		shape->duration = duration;
		shape->meshID = mesh->AddInstance(shape->transform);

		AddShape(mesh, shape);

		if (instanceBuffers[mesh] == nullptr) {
			CreateInstanceBuffer(mesh);
		}
	}
}

void DebugManager::DrawLine(glm::vec3 position1, glm::vec3 position2, glm::vec3 color, float duration)
{
	if (enableValidationLayers)
	{
		std::shared_ptr<Mesh> mesh = EntityManager::GetInstance()->GetMeshes()[MeshTypes::Line];

		std::shared_ptr<DebugShape> shape = std::make_shared<DebugShape>();
		float length = glm::distance(position1, position2);
		glm::vec3 direction = (position1 - position2) / length;
		glm::quat orientation;
		if (direction == glm::vec3(0.0f, 1.0f, 0.0f)) {
			orientation = glm::angleAxis((3.14f / 2.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else if (direction == glm::vec3(0.0f, -1.0f, 0.0f)) {
			orientation = glm::angleAxis((-3.14f / 2.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else {
			orientation = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		shape->transform = std::make_shared<Transform>(position1, orientation, glm::vec3(1.0f, 1.0f, 1.0f) * length);
		shape->color = color;
		shape->duration = duration;
		shape->meshID = mesh->AddInstance(shape->transform);

		AddShape(mesh, shape);

		if (instanceBuffers[mesh] == nullptr) {
			CreateInstanceBuffer(mesh);
		}
	}
}

void DebugManager::RemoveShape(std::shared_ptr<Mesh> mesh, int index)
{
	if (enableValidationLayers)
	{
		instanceBufferDirty[mesh] = true;
		mesh->RemoveInstance(debugShapes[mesh][index]->meshID);
		debugShapes[mesh][index] = nullptr;
	}
}

void DebugManager::AddShape(std::shared_ptr<Mesh> mesh, std::shared_ptr<DebugShape> shape)
{
	if (enableValidationLayers)
	{
		instanceBufferDirty[mesh] = true;

		for (size_t i = 0; i < debugShapes[mesh].size(); i++) {
			if (debugShapes[mesh][i] == nullptr) {
				debugShapes[mesh][i] = shape;
				return;
			}
		}

		debugShapes[mesh].push_back(shape);
	}
}

#pragma endregion

#pragma region Update

void DebugManager::Update()
{
	if (enableValidationLayers)
	{
		if (InputManager::GetInstance()->GetKeyPressed(Controls::ToggleDebug)) {
			drawHandles = !drawHandles;
		}

		for (std::pair<std::shared_ptr<Mesh>, std::vector<std::shared_ptr<DebugShape>>> pair : debugShapes) {
			for (int i = 0; i < pair.second.size(); i++) {
				if (pair.second[i] == nullptr) {
					continue;
				}

				//Don't remove or increment if duration is negative one because negative one is used as the key for infinite duration
				if (pair.second[i]->duration != -1) {
					if (pair.second[i]->duration < 0) {
						RemoveShape(pair.first, i);
						continue;
					}

					pair.second[i]->duration -= Time::GetDeltaTime();
				}
			}

			if (instanceBuffers[pair.first] != nullptr) {
				UpdateInstanceBuffer(pair.first);
			}
		}
	}
}

#pragma endregion

#pragma region Debug Messenger

void DebugManager::SetupDebugMessenger()
{
	//Don't run if validation labels are disabled
	if (!enableValidationLayers) {
		return;
	}

	//Setup the CreateInfo
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	//Set the messenger
	if (CreateDebugUtilsMessengerEXT(VulkanManager::GetInstance()->GetVulkanInstance(), &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to setup messenger!");
	}
}

void DebugManager::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr;
}

bool DebugManager::CheckValidationLayerSupport()
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

VKAPI_ATTR VkBool32 VKAPI_CALL DebugManager::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
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
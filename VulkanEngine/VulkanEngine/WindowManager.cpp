#include "pch.h"
#include "WindowManager.h"

#include "SwapChain.h"

#pragma region Singleton

WindowManager* WindowManager::instance = nullptr;

WindowManager* WindowManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new WindowManager();
    }

    return instance;
}

#pragma endregion

#pragma region Accessors

GLFWwindow* WindowManager::GetWindow()
{
    return window;
}

#pragma endregion

#pragma region Initialization

void WindowManager::InitWindow()
{
	//Initialize GLFW
	glfwInit();

	//Prevent GLFW from loading OpenGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Create the window
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan Window", nullptr, nullptr);

	//Setup the frame buffer resized callback
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FrameBufferResizedCallback);
	
}

#pragma endregion

#pragma region ResizeCallback

void WindowManager::FrameBufferResizedCallback(GLFWwindow* window, int width, int height)
{
	SwapChain::GetInstance()->SetFrameBufferResized(true);
}

#pragma endregion
#pragma once
class WindowManager
{
private:
	uint32_t WINDOW_WIDTH = 800;
	uint32_t WINDOW_HEIGHT = 600;

	static WindowManager* instance;

	GLFWwindow* window = nullptr;

public:

#pragma region Singleton

	static WindowManager* GetInstance();

#pragma endregion

#pragma region Accessors

	GLFWwindow* GetWindow();

#pragma endregion

#pragma region Memory Management

	void InitWindow();

#pragma endregion

#pragma region Resize Callback

	/// <summary>
	/// Callback method that is called whenever the frame buffer is resized
	/// </summary>
	/// <param name="window">The window being used by the application</param>
	/// <param name="width">The width of the window</param>
	/// <param name="height">The height of the window</param>
	static void FrameBufferResizedCallback(GLFWwindow* window, int width, int height);

#pragma endregion
};
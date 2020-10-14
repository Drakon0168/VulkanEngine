#include "pch.h"
#include "InputManager.h"

#include "WindowManager.h"

//Windows Includes
#include <Windows.h>

#pragma region Singleton

InputManager* InputManager::instance = nullptr;

InputManager* InputManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new InputManager();
    }

    return instance;
}

#pragma endregion

#pragma region Constructor

InputManager::InputManager()
{
    controls.resize(Controls::ControlCount);
    axes.resize(Controls::AxisCount);

    double mouseX, mouseY;
    glfwGetCursorPos(WindowManager::GetInstance()->GetWindow(), &mouseX, &mouseY);

    mousePosition = glm::vec2(mouseX, mouseY);
    deltaMouse = glm::vec2(0.0f, 0.0f);

    controls[Controls::Up].SetKeyCode('Q');
    controls[Controls::Down].SetKeyCode('E');
    controls[Controls::Forward].SetKeyCode('W');
    controls[Controls::Back].SetKeyCode('S');
    controls[Controls::Left].SetKeyCode('A');
    controls[Controls::Right].SetKeyCode('D');
    controls[Controls::Jump].SetKeyCode(' ');
    controls[Controls::Sprint].SetKeyCode(VK_LSHIFT);
    controls[Controls::Exit].SetKeyCode(VK_ESCAPE);
    controls[Controls::LeftClick].SetKeyCode(VK_LBUTTON);
    controls[Controls::RightClick].SetKeyCode(VK_RBUTTON);
    controls[Controls::ToggleDebug].SetKeyCode(VK_F9);
}

#pragma endregion

#pragma region Accessors

std::vector<Input> InputManager::GetControls()
{
    return controls;
}

std::vector<InputAxis> InputManager::GetAxes()
{
    return axes;
}

glm::vec2 InputManager::GetMousePosition()
{
    return mousePosition;
}

glm::vec2 InputManager::GetDeltaMouse()
{
    return deltaMouse;
}

#pragma endregion

#pragma region Input States

bool InputManager::GetKey(Controls::Controls key)
{
    if (controls[key].GetState() == InputStates::Down || controls[key].GetState() == InputStates::Pressed) {
        return true;
    }

    return false;
}

bool InputManager::GetKeyPressed(Controls::Controls key)
{
    return controls[key].GetState() == InputStates::Pressed;
}

bool InputManager::GetKeyReleased(Controls::Controls key)
{
    return controls[key].GetState() == InputStates::Released;
}

InputStates InputManager::GetKeyState(Controls::Controls key)
{
    return controls[key].GetState();
}

float InputManager::GetAxis(Controls::InputAxes axis)
{
    //return axes[axis].GetValue();
    return 0;
}

#pragma endregion

#pragma region Update

void InputManager::Update()
{
    //Update Mouse
    double mouseX, mouseY;
    glfwGetCursorPos(WindowManager::GetInstance()->GetWindow(), &mouseX, &mouseY);

    deltaMouse = glm::vec2(mouseX - mousePosition.x, mouseY - mousePosition.y);
    mousePosition = glm::vec2(mouseX, mouseY);

    //Update Inputs
    for (size_t i = 0; i < controls.size(); i++) {
        controls[i].Update();
    }

    //Update Axes
    /*for (size_t i = 0; i < axes.size(); i++) {
        axes[i].Update();
    }*/
}

#pragma endregion
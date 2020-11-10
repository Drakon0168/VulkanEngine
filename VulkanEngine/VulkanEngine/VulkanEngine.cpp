#include "pch.h"

#include "VulkanManager.h"
#include "DebugManager.h"
#include "EntityManager.h"
#include "GameManager.h"
#include "GuiManager.h"
#include "InputManager.h"
#include "PhysicsManager.h"
#include "WindowManager.h"

//Memory leak detection
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main()
{
	try {
		VulkanManager::GetInstance()->Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;

		//Check for memory leaks
		_CrtDumpMemoryLeaks();
		return EXIT_FAILURE;
	}

	//Cleanup singletons
	delete VulkanManager::GetInstance();
	delete DebugManager::GetInstance();
	delete GuiManager::GetInstance();
	delete EntityManager::GetInstance();
	delete GameManager::GetInstance();
	delete InputManager::GetInstance();
	delete PhysicsManager::GetInstance();
	delete WindowManager::GetInstance();

	//Check for memory leaks
	_CrtDumpMemoryLeaks();
	return EXIT_SUCCESS;
}
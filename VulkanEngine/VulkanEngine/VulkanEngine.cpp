#include "pch.h"

#include "VulkanManager.h"

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

	//Check for memory leaks
	_CrtDumpMemoryLeaks();
	return EXIT_SUCCESS;
}
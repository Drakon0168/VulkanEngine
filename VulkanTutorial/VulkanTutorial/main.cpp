#include "pch.h"

#include "TriangleApp.h"

//Memory leak detection
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//Check Vulkan Lib and Include paths if there are linker errors, these need to be installed separately as they are too large for default github file storage

int main() {
	TriangleApp app;

	try {
		app.Run();
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
#include "pch.h"

#include "TriangleApp.h"

//Check Vulkan Lib and Include paths if there are linker errors, these need to be installed separately as they are too large for default github file storage

int main() {
	TriangleApp app;

	try {
		app.Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;

		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
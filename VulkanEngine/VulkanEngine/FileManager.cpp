#include "pch.h"
#include "FileManager.h"

std::vector<char> FileManager::ReadFile(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		std::cout << "File Manager Line " << __LINE__ << ": " << std::endl;
		throw std::runtime_error("Failed to open file!");
	}

	//Allocate space for the contents of the file
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	//Read the contents of the file
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	//Close the file and return
	file.close();
	return buffer;
}
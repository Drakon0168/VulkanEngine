#pragma once
#include "pch.h"

class FileManager
{
public:
	static std::vector<char> ReadFile(const std::string& filePath);
};
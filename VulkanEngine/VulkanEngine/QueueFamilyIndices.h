#pragma once
#include "pch.h"

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	//Returns true if all of the queue families have been set
	bool IsComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};
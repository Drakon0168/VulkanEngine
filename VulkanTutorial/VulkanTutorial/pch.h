#ifndef PCH_H
#define PCH_H

//GLFW Includes
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//GLM Includes
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

//Standard Library Includes
#include <stdexcept>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <chrono>
#include <functional>
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <array>
#include <optional>

#endif //PCH_H
#ifndef SPHERE_INCLUDES_H
#define SPHERE_INCLUDES_H

#define GLFW_INCLUDE_VULKAN
#define VK_ENABLE_BETA_EXTENSIONS

#include "GLFW/glfw3.h"
#include <vulkan/vk_enum_string_helper.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include "vk_mem_alloc.h"
#pragma clang diagnostic pop

#include <vector>
#include <iostream>
#include <cassert>

#endif //SPHERE_INCLUDES_H

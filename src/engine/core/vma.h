#ifndef SPHERE_VMA_H
#define SPHERE_VMA_H

#define VMA_DEBUG_LOG_FORMAT(format, ...) do { \
   printf((format), __VA_ARGS__); \
   printf("\n"); \
} while(false)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include "vk_mem_alloc.h"
#pragma clang diagnostic pop

#endif //SPHERE_VMA_H

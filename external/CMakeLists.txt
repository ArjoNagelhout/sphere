# Add GLFW and GLM dependency
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)
add_subdirectory(glm)

target_include_directories(sphere
        PRIVATE glfw/include
        PRIVATE glm)
target_link_libraries(sphere PUBLIC glfw glm)

# Add Vulkan dependency
find_package(Vulkan REQUIRED)
target_link_libraries(sphere PUBLIC Vulkan::Vulkan)
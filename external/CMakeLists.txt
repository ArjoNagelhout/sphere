find_package(Vulkan REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

add_library(vma INTERFACE)
target_include_directories(vma INTERFACE VulkanMemoryAllocator/include)

add_subdirectory(glm)

add_library(freetype INTERFACE)
target_include_directories(freetype INTERFACE freetype/include)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

add_library(imgui STATIC)
target_include_directories(imgui PUBLIC imgui imgui/backends)
target_sources(imgui PRIVATE
        imgui/imgui.h
        imgui/imgui.cpp

        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp

        imgui/imgui_demo.cpp
        imgui/imgui_tables.cpp

        imgui/backends/imgui_impl_vulkan.cpp
        imgui/backends/imgui_impl_glfw.cpp
)

target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)

add_library(tinyobj INTERFACE)
target_include_directories(tinyobj INTERFACE tinyobjloader)
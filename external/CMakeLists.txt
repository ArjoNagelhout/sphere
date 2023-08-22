# External libraries

# --------------------- Vulkan -------------------
find_package(Vulkan REQUIRED)
add_library(vulkan INTERFACE)
target_link_libraries(vulkan INTERFACE Vulkan::Vulkan)

# ------------ Vulkan Memory Allocator -----------
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE VulkanMemoryAllocator/include)

# ----------------------- GLFW -------------------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# ---------------------- GLM ---------------------
add_subdirectory(glm)

# ------------------- Freetype -------------------
set(FREETYPE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/freetype)
find_package(Freetype REQUIRED)

add_library(freetype INTERFACE)
target_link_libraries(freetype INTERFACE ${FREETYPE_LIBRARIES})
target_include_directories(freetype INTERFACE ${FREETYPE_INCLUDE_DIRS})

# ---------------------- STB ---------------------
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

# --------------------- IMGUI --------------------
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
target_link_libraries(imgui PUBLIC vulkan glfw)

# --------------------- TinyOBJ --------------------
add_library(tinyobj INTERFACE)
target_include_directories(tinyobj INTERFACE tinyobjloader)
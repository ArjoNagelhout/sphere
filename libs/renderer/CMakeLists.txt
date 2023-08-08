set(SOURCES
        window.hpp
        utils.hpp
        utils.cpp
        engine.hpp
        engine.cpp
        engine_instance.cpp
        engine_debug.cpp
        engine_device.cpp engine_surface.cpp)

add_library(renderer "${SOURCES}")

target_link_libraries(renderer Vulkan::Vulkan glfw vma tinyobj imgui)

target_include_directories(renderer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
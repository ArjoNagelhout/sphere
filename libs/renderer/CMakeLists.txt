set(SOURCES
        device.hpp reference.cpp renderer.hpp swapchain.hpp utils.hpp graphics_pipeline.hpp render_pass.hpp memory_allocator.hpp vertex_data.hpp)

add_library(renderer "${SOURCES}")

find_package(Vulkan REQUIRED)

target_link_libraries(renderer Vulkan::Vulkan glfw vma tinyobj)

target_include_directories(renderer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
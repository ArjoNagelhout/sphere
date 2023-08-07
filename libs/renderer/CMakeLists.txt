set(SOURCES
        device.hpp reference.cpp renderer.hpp swapchain.hpp utils.hpp graphics_pipeline.hpp render_pass.hpp memory_allocator.hpp vertex_data.hpp texture.hpp camera.hpp engine.hpp engine.hpp engine.cpp)

add_library(renderer "${SOURCES}")

target_link_libraries(renderer Vulkan::Vulkan glfw vma tinyobj imgui)

target_include_directories(renderer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
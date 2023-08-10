set(SOURCES
        window.hpp window.cpp

        types.hpp

        utils.hpp utils.cpp

        engine.hpp
        engine.cpp
        engine_instance.cpp
        engine_debug.cpp
        engine_device.cpp
        engine_surface.cpp

        swapchain.hpp swapchain.cpp

        render_pass.hpp render_pass.cpp

        graphics_pipeline.hpp graphics_pipeline.cpp

        memory_allocator.hpp memory_allocator.cpp

        renderer.hpp renderer.cpp

        )

add_library(renderer "${SOURCES}")

target_link_libraries(renderer Vulkan::Vulkan glfw vma tinyobj imgui glm)

target_include_directories(renderer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
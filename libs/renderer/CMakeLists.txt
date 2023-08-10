set(SOURCES
        window.h window.cpp

        types.h

        utils.h utils.cpp

        engine.h
        engine.cpp
        engine_instance.cpp
        engine_debug.cpp
        engine_device.cpp
        engine_surface.cpp

        swapchain.h swapchain.cpp

        render_pass.h render_pass.cpp

        graphics_pipeline.h graphics_pipeline.cpp

        memory_allocator.h memory_allocator.cpp

        renderer.h renderer.cpp

        camera.h camera.cpp)

add_library(renderer "${SOURCES}")

target_link_libraries(renderer Vulkan::Vulkan glfw vma tinyobj imgui glm)

target_include_directories(renderer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
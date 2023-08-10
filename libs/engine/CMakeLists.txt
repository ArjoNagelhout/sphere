set(SOURCES
        window.h window.cpp

        types.h

        utils.h utils.cpp

        vulkan_context.h
        vulkan_context.cpp
        vulkan_context_instance.cpp
        vulkan_context_debug.cpp
        vulkan_context_device.cpp
        vulkan_context_surface.cpp

        swapchain.h swapchain.cpp

        render_pass.h render_pass.cpp

        graphics_pipeline.h graphics_pipeline.cpp

        memory_allocator.h memory_allocator.cpp

        engine.h engine.cpp

        camera.h camera.cpp)

add_library(engine "${SOURCES}")

target_link_libraries(engine Vulkan::Vulkan glfw vma tinyobj imgui glm)

target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
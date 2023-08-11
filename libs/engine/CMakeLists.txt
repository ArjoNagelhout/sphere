set(SOURCES
        core/window.h core/window.cpp
        core/utils.h core/utils.cpp
        core/vulkan_context.h
        core/vulkan_context.cpp
        core/vulkan_context_instance.cpp
        core/vulkan_context_debug.cpp
        core/vulkan_context_device.cpp
        core/vulkan_context_surface.cpp

        types.h

        swapchain.h swapchain.cpp
        render_pass.h render_pass.cpp
        graphics_pipeline.h graphics_pipeline.cpp
        memory_allocator.h memory_allocator.cpp
        engine.h engine.cpp
        camera.h camera.cpp

        scene/scene.h
        scene/mesh.h
        scene/object.h
        scene/material.h scene/material.cpp scene/mesh.cpp scene/object.cpp scene/scene.cpp descriptor_set_manager.h scene/material_system.h)

add_library(engine "${SOURCES}")

target_link_libraries(engine Vulkan::Vulkan glfw vma tinyobj imgui glm)

target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
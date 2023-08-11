set(SOURCES
        window.h window.cpp
        utils.h utils.cpp

        types.h

        swapchain.h swapchain.cpp
        render_pass.h render_pass.cpp
        graphics_pipeline.h graphics_pipeline.cpp
        memory_allocator.h memory_allocator.cpp

        camera.h camera.cpp

        engine.h engine.cpp vulkan_setup.cpp

        scene/scene.h
        scene/mesh.h
        scene/object.h
        scene/material.h scene/material.cpp scene/mesh.cpp scene/object.cpp scene/scene.cpp descriptor_set_manager.h scene/material_system.h)

add_library(engine "${SOURCES}")

target_link_libraries(engine Vulkan::Vulkan glfw vma tinyobj imgui glm)

target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
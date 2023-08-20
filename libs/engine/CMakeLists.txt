set(SOURCES
        window.h window.cpp
        utils.h utils.cpp

        types.h

        camera.h camera.cpp

        engine.h engine.cpp vulkan/vulkan_setup.cpp
        vulkan_create.h vulkan_create.cpp

        swapchain.h swapchain.cpp
        render_pass.h render_pass.cpp
        memory.h memory.cpp

        descriptor_set_builder.h descriptor_set_builder.cpp
        pipeline_builder.h pipeline_builder.cpp

        texture.h texture.cpp

        scene/scene.h scene/scene.cpp
        scene/object.h scene/object.cpp
        scene/material.h scene/material.cpp
        scene/mesh.h scene/mesh.cpp
        scene/shader.h scene/shader.cpp

        ui/text_rendering.h ui/text_rendering.cpp
        os/filesystem.h
        vulkan/vulkan_context.cpp
        vulkan/vulkan_context.h
        vulkan/imgui_context.h
        vulkan/imgui_context.cpp)

add_library(engine "${SOURCES}")

target_link_libraries(engine Vulkan::Vulkan glfw vma tinyobj imgui glm stb freetype)

target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
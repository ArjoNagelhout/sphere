set(SOURCES
        vulkan_context.h vulkan_context.cpp vulkan_setup.cpp vulkan_create.cpp
        imgui_context.h imgui_context.cpp

        buffer.h buffer.cpp
        camera.h camera.cpp
        texture.h texture.cpp

        scene.h scene.cpp
        mesh.h mesh.cpp
        object.h object.cpp
        shader.h shader.cpp
        material.h material.cpp

        descriptor_set_builder.h descriptor_set_builder.cpp
        pipeline_builder.h pipeline_builder.cpp

        render_pass.h render_pass.cpp
        swapchain.h swapchain.cpp

        utils.h utils.cpp
        )

add_library(renderer ${SOURCES})
target_include_directories(renderer PUBLIC .)
target_link_libraries(renderer core stb imgui tinyobj)
set(SOURCES
        includes.h
        types.h

        window.h window.cpp
        engine.h engine.cpp
        )

set(RENDERER_SOURCES
        renderer/buffer.h renderer/buffer.cpp
        )


add_library(engine "${SOURCES}:${RENDERER_SOURCES}")

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/renderer)

target_link_libraries(engine Vulkan::Vulkan glfw vma tinyobj imgui glm stb freetype)
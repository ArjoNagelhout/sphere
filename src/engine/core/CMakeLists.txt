set(SOURCES
        includes.h
        ../renderer/types.h
        window.h window.cpp
        )

add_library(core ${SOURCES})
target_include_directories(core PUBLIC .)
target_link_libraries(core vulkan glfw vma glm)
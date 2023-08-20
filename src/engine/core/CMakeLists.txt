set(SOURCES
        includes.h includes.cpp
        types.h
        window.h window.cpp
        )

add_library(core ${SOURCES})
target_include_directories(core PUBLIC .)
target_link_libraries(core Vulkan::Vulkan glfw vma glm)
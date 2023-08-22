set(SOURCES
        window.h window.cpp

        # annoying includes that are required because otherwise
        # we have to add the proper macros such as VK_ENABLE_BETA_EXTENSIONS,
        # and silence clang warnings,
        # each time we include the headers.
        glfw.h
        vma.h
        vulkan.h
        glm.h)

add_library(core ${SOURCES})
target_include_directories(core PUBLIC .)
target_link_libraries(core vulkan glfw vma glm)
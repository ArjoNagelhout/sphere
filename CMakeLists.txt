cmake_minimum_required(VERSION 3.25)
project(sphere)
set(CMAKE_CXX_STANDARD 20)

set(SPHERE_BUNDLE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents)
set(SPHERE_RESOURCES_DIR ${SPHERE_BUNDLE_DIR}/Resources)
set(SPHERE_FRAMEWORKS_DIR ${SPHERE_BUNDLE_DIR}/Frameworks)

# Generate macOS icons file
set(SPHERE_ICON_NAME icon.icns)
set(SPHERE_ICON_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/data/icons/icon.png)
set(SPHERE_ICON_TARGET ${SPHERE_RESOURCES_DIR}/${SPHERE_ICON_NAME})
file(REMOVE ${SPHERE_ICON_TARGET})

# add_custom_command() should be used instead of execute_process as execute_process is
# executed at configure time, instead of at build time. cmake is hard...
add_custom_command(OUTPUT ${SPHERE_ICON_TARGET}
        COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate-icons.sh
        ${SPHERE_ICON_SOURCE} # source
        ${SPHERE_ICON_TARGET} # target
        DEPENDS ${SPHERE_ICON_SOURCE}
        VERBATIM)

set(MACOSX_BUNDLE_ICON_FILE ${SPHERE_ICON_NAME})

# Compile shaders and include in bundle
set(SPHERE_SHADERS_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/data/shaders)
set(SPHERE_SHADERS_TARGET ${SPHERE_RESOURCES_DIR}/shaders)
add_custom_command(OUTPUT ${SPHERE_SHADERS_TARGET}
        COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile-shaders.sh
        ${SPHERE_SHADERS_SOURCE} # source
        ${SPHERE_SHADERS_TARGET} # target
        DEPENDS ${SPHERE_SHADERS_SOURCE}
        VERBATIM)

# Create executable
add_executable(
        sphere
        MACOSX_BUNDLE
        ${SPHERE_ICON_TARGET}
        ${SPHERE_SHADERS_TARGET}
        src/main.cpp src/HelloWorld.h)

set_target_properties(sphere PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/data/Info.plist)

set(MOLTENVK_DIR /Users/arjonagelhout/VulkanSDK/1.3.250.1)
message(STATUS MoltenVK Dir: ${MOLTENVK_DIR})

add_custom_command(TARGET sphere POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${MOLTENVK_DIR}/MoltenVK/dylib/macOS/libMoltenVK.dylib"
        ${CMAKE_CURRENT_BINARY_DIR}/sphere.app/Contents/Frameworks/libMoltenVK.dylib
        DEPENDS vulkan)

# Add GLFW and GLM dependency
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
add_subdirectory(external/glm)

target_include_directories(sphere
        PRIVATE external/glfw/include
        PRIVATE external/glm)
target_link_libraries(sphere glfw glm)

# Add Vulkan dependency
find_package(Vulkan REQUIRED)
target_link_libraries(sphere Vulkan::Vulkan)

get_filename_component(Vulkan_LIBRARY_DIR ${Vulkan_LIBRARY} DIRECTORY)

message(STATUS "Vulkan Library dir: ${Vulkan_LIBRARY_DIR}")

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/build/install)

# Keep RPATH so fixup_bundle can use it to find libraries
set_target_properties(sphere PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
install(TARGETS sphere BUNDLE DESTINATION "sphere")
# Fix up the library references to be self-contained within the bundle.
install(CODE "
        include(BundleUtilities)
        fixup_bundle(\${CMAKE_INSTALL_PREFIX}/sphere/sphere.app \"\" \"${Vulkan_LIBRARY_DIR}\")
        ")

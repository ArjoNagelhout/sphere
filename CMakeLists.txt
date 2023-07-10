# https://developer.apple.com/documentation/xcode/embedding-nonstandard-code-structures-in-a-bundle

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

# Copy the molten VK and Vulkan .dylib dynamic libraries to the Frameworks directory
set(SPHERE_VULKAN_SDK_PATH /Users/arjonagelhout/VulkanSDK/1.3.250.1/macOS)
set(SPHERE_VULKAN_ICD ${SPHERE_VULKAN_SDK_PATH}/share/vulkan/icd.d/MoltenVK_icd.json)

add_custom_command(TARGET sphere POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${SPHERE_VULKAN_SDK_PATH}/lib/libvulkan.dylib
        ${SPHERE_FRAMEWORKS_DIR}/libvulkan.dylib)

add_custom_command(TARGET sphere POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${SPHERE_VULKAN_SDK_PATH}/lib/libMoltenVK.dylib
        ${SPHERE_FRAMEWORKS_DIR}/libMoltenVK.dylib)

# Add Vulkan dependency
find_package(Vulkan REQUIRED)
target_link_libraries(sphere Vulkan::Vulkan)

# set rpath

# set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH};@executable_path/../Frameworks/)
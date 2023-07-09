cmake_minimum_required(VERSION 3.25)
project(sphere)
set(CMAKE_CXX_STANDARD 20)

# Generate macOS icons file
set(SPHERE_ICON_NAME icon.icns)
set(SPHERE_ICON_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/data/icons/icon.png)
set(SPHERE_ICON_TARGET ${CMAKE_CURRENT_BINARY_DIR}/${SPHERE_ICON_NAME})
file(REMOVE ${SPHERE_ICON_TARGET})
execute_process(
        COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate-icons.sh
        ${SPHERE_ICON_SOURCE} # source
        ${SPHERE_ICON_TARGET} #target
)
# add_custom_command() should be used instead of execute_process as execute_process is
# executed at configure time, instead of at build time. cmake is hard...

set(MACOSX_BUNDLE_ICON_FILE ${SPHERE_ICON_NAME})
set_source_files_properties(${SPHERE_ICON_TARGET} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")

# Create executable
add_executable(
        sphere
        MACOSX_BUNDLE
        ${SPHERE_ICON_TARGET}
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

# Add Vulkan dependency
set(SPHERE_VULKAN_SDK_PATH /Users/arjonagelhout/VulkanSDK/1.3.250.1/macOS/)
set(ENV{VK_ICD_FILENAMES} ${SPHERE_VULKAN_SDK_PATH}share/vulkan/icd.d/MoltenVK_icd.json)
set(ENV{VULKAN_SDK} SPHERE_VULKAN_SDK_PATH)

find_package(Vulkan REQUIRED)
target_link_libraries(sphere Vulkan::Vulkan)
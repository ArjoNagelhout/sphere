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
set(MACOSX_BUNDLE_ICON_FILE ${SPHERE_ICON_NAME})
set_source_files_properties(${SPHERE_ICON_TARGET} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")

# Create executable
FILE(GLOB SOURCE_FILES src/*.cpp)
add_executable(
        sphere
        MACOSX_BUNDLE
        ${SPHERE_ICON_TARGET}
        ${SOURCE_FILES}
)

# Add GLFW dependency
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
target_include_directories(sphere PRIVATE external/glfw/include)
target_link_libraries(sphere glfw)

# Add Vulkan dependancy
target_include_directories(sphere PRIVATE /Users/arjonagelhout/VulkanSDK/1.3.250.1/macOS/include)
target_link_libraries(sphere /Users/arjonagelhout/VulkanSDK/1.3.250.1/macOS/lib/libvulkan.dylib)
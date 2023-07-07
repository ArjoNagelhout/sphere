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
add_executable(
        sphere
        MACOSX_BUNDLE
        ${SPHERE_ICON_TARGET}
        src/main.cpp)

# Add GLFW dependency
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
target_include_directories(sphere PRIVATE external/glfw/include)
target_link_libraries(sphere glfw)

# Add Vulkan dependency
# https://cmake.org/cmake/help/v3.7/command/set.html#command:set

set(SPHERE_VULKAN_SDK_PATH /Users/arjonagelhout/VulkanSDK/1.3.250.1/macOS/)
set(ENV{VK_ICD_FILENAMES} ${SPHERE_VULKAN_SDK_PATH}share/vulkan/icd.d/MoltenVK_icd.json)
set(ENV{VULKAN_SDK} SPHERE_VULKAN_SDK_PATH)

find_package(vulkan REQUIRED)
target_link_libraries(sphere Vulkan::Vulkan)


# file(COPY )

#target_include_directories(sphere PRIVATE ${SPHERE_VULKAN_SDK_PATH}/macOS/include)
#target_link_libraries(sphere ${SPHERE_VULKAN_SDK_PATH}/macOS/lib/libvulkan.dylib)

#set(SPHERE_FRAMEWORKS_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Frameworks)
#message("${SPHERE_FRAMEWORKS_DIRECTORY}")

message(${CMAKE_INSTALL_PREFIX})

# install(CODE "include(BundleUtilities) fixup_bundle(\"${APPS})

# https://github.com/ionyshch/cmake-bundle-macos/blob/master/CMakeLists.txt

# https://cmake.cmake.narkive.com/vDC4zPc8/installing-osx-frameworks

#install(FILES "${SPHERE_VULKAN_SDK_PATH}/macOS/lib/libvulkan.dylib"
#        DESTINATION "${SPHERE_FRAMEWORKS_DIRECTORY}")

#install(DIRECTORY "${SPHERE_VULKAN_SDK_PATH}/macOS/lib/"
#        DESTINATION "${SPHERE_FRAMEWORKS_DIRECTORY}")

# Don't use install, not used for building, only for install / configuration once
# https://stackoverflow.com/questions/27665939/cmake-installfiles-doesnt-seem-to-be-working

# add_custom_target
# https://stackoverflow.com/questions/68310342/how-to-get-cmake-to-embed-a-private-framework-into-a-macos-app-bundle
# https://stackoverflow.com/questions/44421278/how-to-include-libraries-in-osx-bundles-with-cmake
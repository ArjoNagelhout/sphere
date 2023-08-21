cmake_minimum_required(VERSION 3.25)
project(sphere)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/build)

# ---------- create target -----------------

set(SOURCES
        src/main.cpp
        )

add_executable(sphere ${SOURCES})

# --------- create resources directory that should be included in the app bundle --------------
set(SPHERE_RESOURCES_DIR ${CMAKE_BINARY_DIR}/resources)
add_custom_target(resources ALL
        COMMAND mkdir -p ${SPHERE_RESOURCES_DIR}
        VERBATIM)

# ---------- compile shaders ---------------
set(SPHERE_SHADERS_SOURCE ${CMAKE_SOURCE_DIR}/data/shaders)
set(SPHERE_SHADERS_TARGET ${SPHERE_RESOURCES_DIR}/shaders)

add_custom_target(shaders ALL
        COMMAND bash ${CMAKE_SOURCE_DIR}/scripts/compile-shaders.sh "${SPHERE_SHADERS_SOURCE}" "${SPHERE_SHADERS_TARGET}"
        VERBATIM
        DEPENDS resources
        DEPENDS "${SPHERE_SHADERS_SOURCE}"
        BYPRODUCTS "${SPHERE_SHADERS_TARGET}"
        )
target_sources(shaders PRIVATE "${SPHERE_SHADERS_TARGET}")

# ---------- add subdirectories -------------

add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(src)

# ---------- include build files -----------
# list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/build_files/platform")

# ---------- platform specific -------------
if (APPLE)
    include(build_files/platform/platform_macos.cmake)
endif ()

cmake_minimum_required(VERSION 3.25)
project(sphere)

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB SOURCE_FILES src/*.cpp)

add_executable(
        sphere
        ${SOURCE_FILES}
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
include_directories(external/glfw/include)
target_link_libraries(sphere glfw)
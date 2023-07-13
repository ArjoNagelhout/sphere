set(SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/context.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/context.cpp")

add_library(renderer "${SOURCES}")

target_include_directories(renderer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
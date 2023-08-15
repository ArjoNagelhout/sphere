set(SOURCES
        editor.h editor.cpp
        )

add_library(editor "${SOURCES}")

target_link_libraries(editor imgui)

target_include_directories(editor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCES
        editor.h editor.cpp
        panels/project_browser.h panels/project_browser.cpp)

add_library(editor "${SOURCES}")

target_link_libraries(editor imgui engine glm)

target_include_directories(editor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
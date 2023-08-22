add_subdirectory(editor)
add_subdirectory(engine)

set(SOURCES
        application.h
        )

target_sources(sphere PUBLIC ${SOURCES})
target_link_libraries(sphere PUBLIC glfw glm engine editor)
add_subdirectory(core)
add_subdirectory(renderer)

set(SOURCES
        engine.h engine.cpp
        )

add_library(engine ${SOURCES})
target_include_directories(engine PUBLIC .)
target_link_libraries(engine core renderer scene ui)
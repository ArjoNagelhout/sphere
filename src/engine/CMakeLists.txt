add_subdirectory(core)
add_subdirectory(renderer)
# add_subdirectory(scene)
#add_subdirectory(ui)

set(SOURCES
        engine.h engine.cpp
        )

add_library(engine ${SOURCES})
target_include_directories(engine PUBLIC .)
target_link_libraries(engine core renderer scene ui)
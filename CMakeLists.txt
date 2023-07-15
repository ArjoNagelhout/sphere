cmake_minimum_required(VERSION 3.25)
project(sphere)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/build/install)

set(SPHERE_BUNDLE_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents)
set(SPHERE_RESOURCES_DIR ${SPHERE_BUNDLE_DIR}/Resources)
set(SPHERE_FRAMEWORKS_DIR ${SPHERE_BUNDLE_DIR}/Frameworks)

# Compile shaders and include in bundle
set(SPHERE_SHADERS_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/data/shaders)
set(SPHERE_SHADERS_TARGET ${SPHERE_RESOURCES_DIR}/shaders)
add_custom_command(OUTPUT ${SPHERE_SHADERS_TARGET}
        COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile-shaders.sh
        ${SPHERE_SHADERS_SOURCE} # source
        ${SPHERE_SHADERS_TARGET} # target
        DEPENDS ${SPHERE_SHADERS_SOURCE}
        VERBATIM)

if (APPLE)
        set(MOLTENVK_DIR /Users/arjonagelhout/VulkanSDK/1.3.250.1)
        message(STATUS "MoltenVK Dir: ${MOLTENVK_DIR}")

        set(SPHERE_ICD_SOURCE ${MOLTENVK_DIR}/MoltenVK/dylib/macOS/MoltenVK_icd.json)
        set(SPHERE_ICD_TARGET ${CMAKE_BINARY_DIR}/staging-json/MoltenVK_icd.json)

        # MoltenVK JSON File
        execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/staging-json)
        execute_process(COMMAND sed -e "/\"library_path\":/s$:[[:space:]]*\"[[:space:]]*[\\.\\/]*$: \"..\\/..\\/..\\/Frameworks\\/$"
                ${SPHERE_ICD_SOURCE}
                OUTPUT_FILE ${SPHERE_ICD_TARGET})

        # ~~~
        # Modify the ICD JSON file to adjust the library path.
        # The ICD JSON file goes in the Resources/vulkan/icd.d directory, so adjust the
        # library_path to the relative path to the Frameworks directory in the bundle.
        # The regex does: substitute ':<whitespace>"<whitespace><all occurrences of . and />' with:
        # ': "../../../Frameworks/'
        # ~~~
        add_custom_target(MoltenVK_icd-staging-json ALL
                COMMAND mkdir -p ${CMAKE_BINARY_DIR}/staging-json
                COMMAND sed -e "/\"library_path\":/s$:[[:space:]]*\"[[:space:]]*[\\.\\/]*$: \"..\\/..\\/..\\/Frameworks\\/$"
                ${SPHERE_ICD_SOURCE} > ${SPHERE_ICD_TARGET}
                VERBATIM
                DEPENDS "${SPHERE_ICD_SOURCE}")
        set_source_files_properties(${SPHERE_ICD_TARGET} PROPERTIES GENERATED TRUE)

        # Generate macOS icons file
        set(SPHERE_ICON_NAME icon.icns)
        set(SPHERE_ICON_SOURCE ${CMAKE_SOURCE_DIR}/data/icons/icon.png)
        set(SPHERE_ICON_TARGET ${SPHERE_RESOURCES_DIR}/${SPHERE_ICON_NAME})

        add_custom_command(OUTPUT ${SPHERE_ICON_TARGET}
                COMMAND bash ${CMAKE_SOURCE_DIR}/scripts/generate-icons.sh
                ${SPHERE_ICON_SOURCE} # source
                ${SPHERE_ICON_TARGET} # target
                DEPENDS ${SPHERE_ICON_SOURCE}
                VERBATIM)

        set(MACOSX_BUNDLE_ICON_FILE ${SPHERE_ICON_NAME})
endif()

add_executable(sphere
        MACOSX_BUNDLE
        ${SPHERE_ICON_TARGET}
        ${SPHERE_SHADERS_TARGET}
        ${SPHERE_ICD_TARGET}
        src/main.cpp
        src/app.hpp
        src/commands/undo_stack.hpp
        src/commands/command.hpp
        src/scene.hpp
        src/camera.hpp
        src/mesh.hpp
        src/node.hpp
        src/commands/add_nodes_command.hpp
        src/texture.hpp
        src/selection.hpp
        src/commands/select_nodes_command.hpp
        src/commands/transform_nodes_command.hpp src/commands/remove_nodes_command.hpp src/shader.hpp src/mesh_renderer_node.hpp src/material.hpp)

target_link_libraries(sphere PUBLIC renderer)

add_subdirectory(external)
add_subdirectory(libs)

if(APPLE)
        set_target_properties(sphere PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/data/Info.plist)

        add_dependencies(sphere MoltenVK_icd-staging-json)
        set_source_files_properties("${CMAKE_BINARY_DIR}/staging-json/MoltenVK_icd.json"
                PROPERTIES
                MACOSX_PACKAGE_LOCATION
                "Resources/vulkan/icd.d")

        message(STATUS "Current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")
        add_custom_command(TARGET sphere POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy "${MOLTENVK_DIR}/MoltenVK/dylib/macOS/libMoltenVK.dylib"
                ${CMAKE_CURRENT_BINARY_DIR}/sphere.app/Contents/Frameworks/libMoltenVK.dylib
                DEPENDS vulkan)

        get_filename_component(Vulkan_LIBRARY_DIR ${Vulkan_LIBRARY} DIRECTORY)
        message(STATUS "Vulkan Library dir: ${Vulkan_LIBRARY_DIR}")

        # Keep RPATH so fixup_bundle can use it to find libraries
        set_target_properties(sphere PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
        install(TARGETS sphere BUNDLE DESTINATION "sphere")
        # Fix up the library references to be self-contained within the bundle.
        install(CODE "
                include(BundleUtilities)
                fixup_bundle(\${CMAKE_INSTALL_PREFIX}/sphere/sphere.app \"\" \"${Vulkan_LIBRARY_DIR}\")
                ")
endif()
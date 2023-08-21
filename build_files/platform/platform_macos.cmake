set(SPHERE_BUNDLE_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents)

set(MOLTENVK_DIR /Users/arjonagelhout/VulkanSDK/1.3.250.1)
message(STATUS "MoltenVK Dir: ${MOLTENVK_DIR}")

# --------------- fix MoltenVK ICD file -------------------

set(SPHERE_ICD_SOURCE ${MOLTENVK_DIR}/MoltenVK/dylib/macOS/MoltenVK_icd.json)
set(SPHERE_ICD_TARGET ${SPHERE_RESOURCES_DIR}/MoltenVK_icd.json)

# ~~~
# Modify the ICD JSON file to adjust the library path.
# The ICD JSON file goes in the Resources/vulkan/icd.d directory, so adjust the
# library_path to the relative path to the Frameworks directory in the bundle.
# The regex does: substitute ':<whitespace>"<whitespace><all occurrences of . and />' with:
# ': "../../../Frameworks/'
# ~~~
add_custom_target(icd ALL
        COMMAND sed -e "/\"library_path\":/s$:[[:space:]]*\"[[:space:]]*[\\.\\/]*$: \"..\\/..\\/..\\/Frameworks\\/$" ${SPHERE_ICD_SOURCE} > ${SPHERE_ICD_TARGET}
        VERBATIM
        DEPENDS resources
        DEPENDS "${SPHERE_ICD_SOURCE}"
        BYPRODUCTS "${SPHERE_ICD_TARGET}"
        )
set_source_files_properties(${SPHERE_ICD_TARGET} PROPERTIES GENERATED TRUE)

# ------------------- create icon -------------------------

set(SPHERE_ICON_NAME icon.icns)
set(SPHERE_ICON_SOURCE ${CMAKE_SOURCE_DIR}/data/icons/icon.png)
set(SPHERE_ICON_TARGET ${SPHERE_RESOURCES_DIR}/${SPHERE_ICON_NAME})

add_custom_target(icon ALL
        COMMAND bash ${CMAKE_SOURCE_DIR}/scripts/generate-icons.sh ${SPHERE_ICON_SOURCE} ${SPHERE_ICON_TARGET}
        VERBATIM
        DEPENDS resources
        DEPENDS "${SPHERE_ICON_SOURCE}"
        BYPRODUCTS "${SPHERE_ICON_TARGET}"
        )

set(MACOSX_BUNDLE_ICON_FILE ${SPHERE_ICON_NAME})

# -------------------- add plist file ---------------------------
set_target_properties(sphere PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/data/Info.plist
        MACOSX_BUNDLE TRUE)

# -------------------- function for copying to Resources in app bundle ----------------------
# from https://stackoverflow.com/questions/41121000/cmake-os-x-bundle-recursively-copy-directory-to-resources

function(resource VAR SOURCE_PATH DESTINATION PATTERN)
    file(GLOB_RECURSE _LIST CONFIGURE_DEPENDS ${SOURCE_PATH}/${PATTERN})
    foreach (RESOURCE ${_LIST})
        get_filename_component(_PARENT ${RESOURCE} DIRECTORY)
        if (${_PARENT} STREQUAL ${SOURCE_PATH})
            set(_DESTINATION ${DESTINATION})
        else ()
            file(RELATIVE_PATH _DESTINATION ${SOURCE_PATH} ${_PARENT})
            set(_DESTINATION ${DESTINATION}/${_DESTINATION})
        endif ()
        set_property(SOURCE ${RESOURCE} PROPERTY MACOSX_PACKAGE_LOCATION ${_DESTINATION})
    endforeach (RESOURCE)
    set(${VAR} ${_LIST} PARENT_SCOPE)
endfunction()

# --------------------- add custom target dependencies to sphere ----------------------------
# https://gitlab.kitware.com/cmake/cmake/-/issues/21768

add_dependencies(sphere icd icon shaders)

set_source_files_properties("${SPHERE_ICON_TARGET}" PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")

set_source_files_properties("${SPHERE_ICD_TARGET}" PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources/vulkan/icd.d")

set(SPHERE_SOURCES
        "${SPHERE_ICD_TARGET}"
        "${SPHERE_ICON_TARGET}"
        "${SPHERE_SHADERS_TARGET}")
target_sources(sphere PUBLIC "${SPHERE_SOURCES}")

# ---------------- shaders don't get copied ffs ---------------

add_custom_command(TARGET sphere POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${SPHERE_SHADERS_TARGET}" "${SPHERE_BUNDLE_DIR}/Resources/shaders"
        DEPENDS shaders
        )

# ---------------- fix bundle --------------------

add_custom_command(TARGET sphere POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${MOLTENVK_DIR}/MoltenVK/dylib/macOS/libMoltenVK.dylib" "${SPHERE_BUNDLE_DIR}/Frameworks/libMoltenVK.dylib"
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
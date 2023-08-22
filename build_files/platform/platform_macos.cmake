# https://gitlab.kitware.com/cmake/cmake/-/issues/21768

set(SPHERE_BUNDLE_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents)

set(MOLTENVK_DIR /Users/arjonagelhout/VulkanSDK/1.3.250.1)
message(STATUS "MoltenVK Dir: ${MOLTENVK_DIR}")

# --------------- fix MoltenVK ICD file -------------------

set(SPHERE_ICD_SOURCE ${MOLTENVK_DIR}/MoltenVK/dylib/macOS/MoltenVK_icd.json)
set(SPHERE_ICD_TARGET ${SPHERE_RESOURCES_DIR}/MoltenVK_icd.json)

# Modify the ICD JSON file to adjust the library path.
# The ICD JSON file goes in the Resources/vulkan/icd.d directory, so adjust the
# library_path to the relative path to the Frameworks directory in the bundle.
# The regex does: substitute ':<whitespace>"<whitespace><all occurrences of . and />' with:
# ': "../../../Frameworks/'

add_custom_command(OUTPUT "${SPHERE_ICD_TARGET}"
        COMMAND sed -e "/\"library_path\":/s$:[[:space:]]*\"[[:space:]]*[\\.\\/]*$: \"..\\/..\\/..\\/Frameworks\\/$" ${SPHERE_ICD_SOURCE} > ${SPHERE_ICD_TARGET}
        VERBATIM
        DEPENDS "${SPHERE_ICD_SOURCE}"
        )

set_source_files_properties(${SPHERE_ICD_TARGET} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources/vulkan/icd.d"
        GENERATED TRUE)

# ------------------- create icon -------------------------

set(SPHERE_ICON_NAME icon.icns)
set(SPHERE_ICON_SOURCE ${CMAKE_SOURCE_DIR}/data/icons/icon.png)
set(SPHERE_ICON_TARGET ${SPHERE_RESOURCES_DIR}/${SPHERE_ICON_NAME})

add_custom_command(OUTPUT "${SPHERE_ICON_TARGET}"
        COMMAND bash ${CMAKE_SOURCE_DIR}/scripts/generate-icons.sh ${SPHERE_ICON_SOURCE} ${SPHERE_ICON_TARGET}
        DEPENDS "${SPHERE_ICON_SOURCE}"
        VERBATIM
        )

set_source_files_properties("${SPHERE_ICON_TARGET}" PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
        GENERATED TRUE)

set(MACOSX_BUNDLE_ICON_FILE ${SPHERE_ICON_NAME})

# -------------------- add plist file ---------------------------
set_target_properties(sphere PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/data/Info.plist
        MACOSX_BUNDLE TRUE)

# --------------------- add dependencies to sphere target ----------------------------

set(SPHERE_SOURCES
        "${SPHERE_ICD_TARGET}"
        "${SPHERE_ICON_TARGET}"
        "${SPHERE_SHADERS_TARGET}")
target_sources(sphere PUBLIC "${SPHERE_SOURCES}")

# ---------------- copy compiled shaders into bundle ---------------
#
# we couldn't use set_source_file_properties because the shaders are compiled at build time
# and you can't use the command on a directory
#
# we could use file(GLOB_RECURSE ...) to get a list of the generated files, but this is a command that gets run on
# configure time. That means you'd have to run the build twice.
#
# Simply copying the directory to the bundle is the easiest implementation I've been able to come up with,
# but this might be considered hacky in modern Cmake.

add_custom_command(TARGET sphere POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${SPHERE_SHADERS_TARGET}" "${SPHERE_BUNDLE_DIR}/Resources/shaders"
        DEPENDS "${SPHERE_SHADERS_TARGET}"
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
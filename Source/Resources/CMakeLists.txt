cmake_minimum_required(VERSION 3.25)

# Compile shaders to SPIR-V with GLSLANG

find_program(GLSLANG_EXECUTABLE NAMES glslang glslangValidator HINTS "$ENV{VULKAN_SDK}/bin" REQUIRED)
if (NOT GLSLANG_EXECUTABLE)
    message(FATAL_ERROR "glslang not found! Please ensure that the VULKAN_SDK environment variable is set correctly.")
endif()
message(STATUS "Found glslang: ${GLSLANG_EXECUTABLE}")
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
set(SHADER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Shaders)
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})
file(GLOB_RECURSE SHADER_SOURCE_FILES CONFIGURE_DEPENDS RELATIVE ${SHADER_SOURCE_DIR} ${SHADER_SOURCE_DIR}/*.vert ${SHADER_SOURCE_DIR}/*.frag ${SHADER_SOURCE_DIR}/*.comp ${SHADER_SOURCE_DIR}/*.tesc ${SHADER_SOURCE_DIR}/*.tese ${SHADER_SOURCE_DIR}/*.geom)
set(SHADER_BINARY_FILES)
foreach(SHADER_SOURCE_FILE ${SHADER_SOURCE_FILES})
    set(SHADER_BINARY_FILE ${SHADER_BINARY_DIR}/${SHADER_SOURCE_FILE}.spv)
    get_filename_component(SHADER_BINARY_PARENT "${SHADER_BINARY_FILE}" DIRECTORY)
    add_custom_command(
        OUTPUT ${SHADER_BINARY_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_BINARY_PARENT}"
        COMMAND ${GLSLANG_EXECUTABLE}
            --target-env vulkan1.3
            -o ${SHADER_BINARY_FILE}
            ${SHADER_SOURCE_DIR}/${SHADER_SOURCE_FILE}
        DEPENDS ${SHADER_SOURCE_DIR}/${SHADER_SOURCE_FILE}
        COMMENT "Compiling shader: ${SHADER_SOURCE_FILE} to ${SHADER_BINARY_FILE}"
        VERBATIM
    )
    list(APPEND SHADER_BINARY_FILES ${SHADER_BINARY_FILE})
endforeach()
add_custom_target(compile_shaders ALL DEPENDS ${SHADER_BINARY_FILES})
install(DIRECTORY "${SHADER_BINARY_DIR}" DESTINATION "${POLOS_INSTALL_DIR}/Resources" COMPONENT Runtime)

if (NOT BUILD_EXAMPLES)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources.json" DESTINATION "${POLOS_INSTALL_DIR}/Resources" COMPONENT Runtime)
endif()

# Each executable resolves resources beside itself, independent of the working directory.
set_property(GLOBAL PROPERTY POLOS_SHADER_BINARY_DIR "${SHADER_BINARY_DIR}")

function(polos_stage_resources TARGET_NAME)
    get_property(POLOS_SHADER_BINARY_DIR GLOBAL PROPERTY POLOS_SHADER_BINARY_DIR)
    set(ENGINE_CATALOG "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/resources.json")
    set(APP_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
    set(APP_CATALOG "${APP_RESOURCES}/resources.json")
    set(STAGED_CATALOG "${CMAKE_CURRENT_BINARY_DIR}/resources.json")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${ENGINE_CATALOG}" "${APP_CATALOG}")
    file(READ "${ENGINE_CATALOG}" catalog)
    file(READ "${APP_CATALOG}" appCatalog)
    string(JSON nextIndex LENGTH "${catalog}")
    string(JSON appCount LENGTH "${appCatalog}")
    set(appIndex 0)
    while (appIndex LESS appCount)
        string(JSON entry GET "${appCatalog}" ${appIndex})
        string(JSON catalog SET "${catalog}" ${nextIndex} "${entry}")
        math(EXPR appIndex "${appIndex} + 1")
        math(EXPR nextIndex "${nextIndex} + 1")
    endwhile()
    file(CONFIGURE OUTPUT "${STAGED_CATALOG}" CONTENT "${catalog}\n" @ONLY)

    add_custom_target(${TARGET_NAME}-resources ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/Resources/Shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${POLOS_SHADER_BINARY_DIR}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Resources/Shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${APP_RESOURCES}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Resources"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${STAGED_CATALOG}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Resources/resources.json"
        DEPENDS compile_shaders
        VERBATIM
    )
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}-resources)
    install(DIRECTORY "${APP_RESOURCES}/" DESTINATION "${POLOS_INSTALL_DIR}/Resources" COMPONENT Runtime
        PATTERN "resources.json" EXCLUDE)
    install(FILES "${STAGED_CATALOG}" DESTINATION "${POLOS_INSTALL_DIR}/Resources" COMPONENT Runtime)
endfunction()

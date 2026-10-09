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
            --discard-is-terminate # otherwise discard needs shaderDemoteToHelperInvocation 1.3 feature
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
install(DIRECTORY "${SHADER_BINARY_DIR}" DESTINATION "${POLOS_INSTALL_DIR}/Resources")

# Preserve subdirectories, including textures with the same filename.
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Textures" DESTINATION "${POLOS_INSTALL_DIR}/Resources")

# Optional model resources are packaged alongside textures when present.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Models")
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Models" DESTINATION "${POLOS_INSTALL_DIR}/Resources")
endif()

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources.json" DESTINATION "${POLOS_INSTALL_DIR}/Resources")

# Each executable resolves resources beside itself, independent of the working directory.
set_property(GLOBAL PROPERTY POLOS_SHADER_BINARY_DIR "${SHADER_BINARY_DIR}")

function(polos_stage_resources TARGET_NAME)
    get_property(POLOS_SHADER_BINARY_DIR GLOBAL PROPERTY POLOS_SHADER_BINARY_DIR)
    set(POLOS_RESOURCE_SOURCE_DIR "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")
    add_dependencies(${TARGET_NAME} compile_shaders)
    add_custom_target(${TARGET_NAME}-resources ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/Resources/Shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${POLOS_SHADER_BINARY_DIR}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Resources/Shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${POLOS_RESOURCE_SOURCE_DIR}/Textures" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Resources/Textures"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${POLOS_RESOURCE_SOURCE_DIR}/resources.json" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Resources/resources.json"
        DEPENDS compile_shaders
        VERBATIM
    )
    if (EXISTS "${POLOS_RESOURCE_SOURCE_DIR}/Models")
        add_custom_command(TARGET ${TARGET_NAME}-resources POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${POLOS_RESOURCE_SOURCE_DIR}/Models" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Resources/Models"
            VERBATIM)
    endif()
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}-resources)
endfunction()

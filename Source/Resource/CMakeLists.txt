# Compile shaders to SPIR-V with GLSLANG

find_program(GLSLANG_EXECUTABLE glslang HINTS "$ENV{VULKAN_SDK}/bin/glslang" REQUIRED)
if (NOT GLSLANG_EXECUTABLE)
    message(FATAL_ERROR "glslang not found! Please ensure that the VULKAN_SDK environment variable is set correctly.")
endif()
message(STATUS "Found glslang: ${GLSLANG_EXECUTABLE}")
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
set(SHADER_BINARY_DIR ${POLOS_INSTALL_DIR}/Resource/Shaders)
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})
file(GLOB_RECURSE SHADER_SOURCE_FILES RELATIVE ${SHADER_SOURCE_DIR} ${SHADER_SOURCE_DIR}/*.vert ${SHADER_SOURCE_DIR}/*.frag ${SHADER_SOURCE_DIR}/*.comp ${SHADER_SOURCE_DIR}/*.tesc ${SHADER_SOURCE_DIR}/*.tese ${SHADER_SOURCE_DIR}/*.geom)
set(SHADER_BINARY_FILES)
foreach(SHADER_SOURCE_FILE ${SHADER_SOURCE_FILES})
    set(SHADER_BINARY_FILE ${SHADER_BINARY_DIR}/${SHADER_SOURCE_FILE}.spv)
    add_custom_command(
        OUTPUT ${SHADER_BINARY_FILE}
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

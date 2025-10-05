# Compile shaders to SPIR-V with SLANG

find_program(SLANG_EXECUTABLE slangc HINTS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/shader-slang" REQUIRED)
if (NOT SLANG_EXECUTABLE)
    message(FATAL_ERROR "slang not found! Please ensure that the SLANG_DIR environment variable is set correctly.")
endif()
message(STATUS "Found slang: ${SLANG_EXECUTABLE}")
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
set(SHADER_BINARY_DIR ${POLOS_INSTALL_DIR}/Resource/Shaders)
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})
file(GLOB_RECURSE SHADER_SOURCE_FILES RELATIVE ${SHADER_SOURCE_DIR} ${SHADER_SOURCE_DIR}/*.slang)
set(SHADER_BINARY_FILES)
foreach(SHADER_SOURCE_FILE ${SHADER_SOURCE_FILES})
    set(SHADER_BINARY_FILE ${SHADER_BINARY_DIR}/${SHADER_SOURCE_FILE}.spv)
    add_custom_command(
        OUTPUT ${SHADER_BINARY_FILE}
        COMMAND ${SLANG_EXECUTABLE}
            -target spirv
            -profile spirv_1_6
            -fvk-use-entrypoint-name
            -o ${SHADER_BINARY_FILE}
            ${SHADER_SOURCE_DIR}/${SHADER_SOURCE_FILE}
        DEPENDS ${SHADER_SOURCE_DIR}/${SHADER_SOURCE_FILE}
        COMMENT "Compiling shader: ${SHADER_SOURCE_FILE} to ${SHADER_BINARY_FILE}"
        VERBATIM
    )
    list(APPEND SHADER_BINARY_FILES ${SHADER_BINARY_FILE})
endforeach()
add_custom_target(Shaders ALL DEPENDS ${SHADER_BINARY_FILES})

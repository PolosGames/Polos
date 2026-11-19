function(polos_compile_shaders TARGET_NAME SOURCE_DIR OUTPUT_DIR)
    cmake_parse_arguments(SHADERS "INSTALL" "" "" ${ARGN})

    file(GLOB_RECURSE shader_sources CONFIGURE_DEPENDS RELATIVE "${SOURCE_DIR}"
        "${SOURCE_DIR}/*.vert" "${SOURCE_DIR}/*.frag" "${SOURCE_DIR}/*.comp"
        "${SOURCE_DIR}/*.tesc" "${SOURCE_DIR}/*.tese" "${SOURCE_DIR}/*.geom")
    file(MAKE_DIRECTORY "${OUTPUT_DIR}")
    if (shader_sources)
        find_program(GLSLANG_EXECUTABLE NAMES glslang glslangValidator HINTS "$ENV{VULKAN_SDK}/bin" REQUIRED)
    endif()

    set(shader_outputs)
    foreach(shader IN LISTS shader_sources)
        set(output "${OUTPUT_DIR}/${shader}.spv")
        get_filename_component(output_dir "${output}" DIRECTORY)
        add_custom_command(
            OUTPUT "${output}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${output_dir}"
            COMMAND "${GLSLANG_EXECUTABLE}" --target-env vulkan1.3
                -o "${output}" "${SOURCE_DIR}/${shader}"
            DEPENDS "${SOURCE_DIR}/${shader}"
            COMMENT "Compiling shader: ${shader}"
            VERBATIM
        )
        list(APPEND shader_outputs "${output}")

        if (SHADERS_INSTALL)
            get_filename_component(relative_dir "${shader}" DIRECTORY)
            install(FILES "${output}" DESTINATION "${POLOS_INSTALL_DIR}/Resources/${relative_dir}" COMPONENT Runtime)
        endif()
    endforeach()

    add_custom_target(${TARGET_NAME} DEPENDS ${shader_outputs})
endfunction()

find_program(GLSLANG_EXECUTABLE NAMES glslang glslangValidator HINTS "$ENV{VULKAN_SDK}/bin" REQUIRED)

set(engine_shader_sources forward_shading.vert forward_shading.frag main_compute.comp debug_draw.vert debug_draw.frag)
set(shader_generator "${CMAKE_CURRENT_SOURCE_DIR}/EmbedShader.cmake")
set(embedded_shaders)
foreach(shader IN LISTS engine_shader_sources)
    set(source "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/${shader}")
    set(spirv "${CMAKE_CURRENT_BINARY_DIR}/${shader}.spv")
    set(output "${POLOS_COMPOSITOR_SHADER_DIR}/${shader}.cpp")
    string(REPLACE "." "_" shader_namespace "${shader}")
    if (shader STREQUAL "main_compute.comp")
        set(shader_namespace main_compute)
    endif()

    add_custom_command(
        OUTPUT "${output}"
        BYPRODUCTS "${spirv}"
        COMMAND "${GLSLANG_EXECUTABLE}" --target-env vulkan1.3 -o "${spirv}" "${source}"
        COMMAND "${CMAKE_COMMAND}"
            "-DINPUT=${spirv}" "-DOUTPUT=${output}" "-DSHADER_NAMESPACE=${shader_namespace}"
            -P "${shader_generator}"
        DEPENDS "${source}" "${shader_generator}" "${GLSLANG_EXECUTABLE}"
        COMMENT "Embedding engine shader: ${shader}"
        VERBATIM
    )
    list(APPEND embedded_shaders "${output}")
endforeach()

add_custom_target(compile_shaders DEPENDS ${embedded_shaders})

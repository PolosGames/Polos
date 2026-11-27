set(engine_shader_sources forward_shading.vert forward_shading.frag main_compute.comp debug_draw.vert debug_draw.frag)
set(shader_generator "${CMAKE_CURRENT_SOURCE_DIR}/EmbedShader.cmake")
set(embedded_shaders)
foreach (shader IN LISTS engine_shader_sources)
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
        COMMAND "${POLOS_GLSLANG_EXECUTABLE}" --target-env vulkan1.3 -o "${spirv}" "${source}"
        COMMAND "${CMAKE_COMMAND}"
            "-DINPUT=${spirv}" "-DOUTPUT=${output}" "-DSHADER_NAMESPACE=${shader_namespace}"
            -P "${shader_generator}"
        DEPENDS "${source}" "${shader_generator}" "${POLOS_GLSLANG_EXECUTABLE}"
        COMMENT "Embedding engine shader: ${shader}"
        VERBATIM
    )
    list(APPEND embedded_shaders "${output}")
endforeach()

add_custom_target(
    compile_shaders
        DEPENDS ${embedded_shaders}
)

include_guard(GLOBAL)

function(polos_app)
    cmake_parse_arguments(PARSE_ARGV 0 app "" "NAME;ENTITY_CAPACITY" "SOURCES")
    if (NOT app_NAME OR app_UNPARSED_ARGUMENTS OR app_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "Usage: polos_app(NAME <target> SOURCES <files> [ENTITY_CAPACITY <count>])")
    endif()
    if (NOT TARGET polos::polos)
        message(FATAL_ERROR "polos_app requires the polos::polos target. Load the SDK with find_package(Polos CONFIG REQUIRED).")
    endif()

    get_target_property(polos_imported polos::polos IMPORTED)
    if (polos_imported)
        get_target_property(sdk_capacity polos::polos POLOS_ENTITY_CAPACITY)
        if (DEFINED app_ENTITY_CAPACITY AND NOT app_ENTITY_CAPACITY STREQUAL sdk_capacity)
            message(FATAL_ERROR "${app_NAME} requests ENTITY_CAPACITY ${app_ENTITY_CAPACITY}, but this Polos SDK was built with ${sdk_capacity}.")
        endif()
        set(app_install_dir bin)
        if (DEFINED CMAKE_INSTALL_BINDIR)
            set(app_install_dir "${CMAKE_INSTALL_BINDIR}")
        endif()
    else()
        if (POLOS_EXAMPLE AND NOT POLOS_EXAMPLE STREQUAL app_NAME)
            return()
        endif()
        set(app_install_dir "${POLOS_INSTALL_DIR}")
    endif()

    add_executable(
        ${app_NAME}
            ${app_SOURCES}
    )
    if (DEFINED app_ENTITY_CAPACITY)
        set_property(TARGET ${app_NAME} PROPERTY POLOS_ENTITY_CAPACITY "${app_ENTITY_CAPACITY}")
    endif()

    if (WIN32)
        set_target_properties(
            ${app_NAME}
                PROPERTIES
                    WIN32_EXECUTABLE TRUE
        )
        if (MSVC)
            target_link_options(
                ${app_NAME}
                    PRIVATE /ENTRY:mainCRTStartup
            )
        endif()
    endif()

    target_link_libraries(
        ${app_NAME}
            PRIVATE polos::polos
    )
    target_include_directories(
        ${app_NAME}
            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    target_compile_definitions(
        ${app_NAME}
            PRIVATE PL_LOGGER_TYPE=App
    )
    set_target_properties(
        ${app_NAME}
            PROPERTIES
                POSITION_INDEPENDENT_CODE True
                CXX_VISIBILITY_PRESET     hidden
                VISIBILITY_INLINES_HIDDEN True
    )
    if (LINUX)
        set_target_properties(
            ${app_NAME}
                PROPERTIES
                    INSTALL_RPATH "$ORIGIN"
        )
    endif()

    if (polos_imported)
        polos_stage_sdk_runtime(
            ${app_NAME}
                "${app_install_dir}"
        )
    else()
        polos_build_options(
            ${app_NAME}
        )
        if (TARGET install-runtime)
            add_dependencies(
                install-runtime
                    ${app_NAME}
            )
        endif()
    endif()

    install(TARGETS ${app_NAME}
        RUNTIME DESTINATION "${app_install_dir}" COMPONENT Runtime
    )
    if (MSVC)
        install(FILES "$<TARGET_PDB_FILE:${app_NAME}>"
            DESTINATION "${app_install_dir}" COMPONENT Runtime OPTIONAL
        )
    endif()

    set(app_resources "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
    if (IS_DIRECTORY "${app_resources}")
        polos_app_resources(
            ${app_NAME}
                "${app_resources}" "${app_install_dir}/Resources"
        )
    endif()
endfunction()

function(polos_stage_sdk_runtime target install_dir)
    if (LINUX)
        set_property(TARGET ${target} APPEND PROPERTY BUILD_RPATH "$ORIGIN")
    endif()

    set(copy_commands)
    foreach (library IN ITEMS polos::polos polos::glfw polos::TracyClient)
        if (NOT TARGET "${library}")
            continue()
        endif()

        # The Linux dynamic loader uses the SONAME, which may differ from the file name.
        if (LINUX)
            set(runtime_name "$<TARGET_SONAME_FILE_NAME:${library}>")
        else()
            set(runtime_name "$<TARGET_FILE_NAME:${library}>")
        endif()
        list(APPEND copy_commands
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:${library}>" "$<TARGET_FILE_DIR:${target}>/${runtime_name}"
        )
        install(FILES "$<TARGET_FILE:${library}>"
            DESTINATION "${install_dir}" RENAME "${runtime_name}" COMPONENT Runtime
        )
    endforeach()

    set(sdk_dir "$<TARGET_FILE_DIR:polos::polos>")
    install(FILES "${sdk_dir}/LICENSE" "${sdk_dir}/LICENSE-ImGui.txt" "${sdk_dir}/LICENSE-Quill.txt"
        DESTINATION "${install_dir}/polos-licenses" COMPONENT Runtime
    )
    install(DIRECTORY "${sdk_dir}/licenses/"
        DESTINATION "${install_dir}/polos-licenses" COMPONENT Runtime
        PATTERN "vulkan" EXCLUDE
        PATTERN "tracy-libbacktrace" EXCLUDE
    )
    if (TARGET polos::TracyClient)
        install(FILES "${sdk_dir}/LICENSE-Tracy.txt"
            DESTINATION "${install_dir}/polos-licenses" COMPONENT Runtime
        )
    endif()

    add_custom_target(
        ${target}-sdk-runtime
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>"
            ${copy_commands}
            VERBATIM
    )
    add_dependencies(
        ${target}
            ${target}-sdk-runtime
    )
endfunction()

function(polos_app_resources target source_dir install_dir)
    file(GLOB_RECURSE shaders CONFIGURE_DEPENDS RELATIVE "${source_dir}"
        "${source_dir}/*.vert" "${source_dir}/*.frag" "${source_dir}/*.comp"
        "${source_dir}/*.tesc" "${source_dir}/*.tese" "${source_dir}/*.geom"
    )
    if (shaders)
        find_program(POLOS_GLSLANG_EXECUTABLE
            NAMES glslang glslangValidator
            HINTS "$ENV{VULKAN_SDK}/bin"
            NO_CMAKE_FIND_ROOT_PATH
            REQUIRED
        )
    endif()

    set(compiled_dir "${CMAKE_CURRENT_BINARY_DIR}/compiled-resources/${target}")
    file(MAKE_DIRECTORY "${compiled_dir}")
    set(shader_outputs)
    foreach (shader IN LISTS shaders)
        set(output "${compiled_dir}/${shader}.spv")
        get_filename_component(output_dir "${output}" DIRECTORY)
        add_custom_command(
            OUTPUT "${output}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${output_dir}"
            COMMAND "${POLOS_GLSLANG_EXECUTABLE}" --target-env vulkan1.3
                -o "${output}" "${source_dir}/${shader}"
            DEPENDS "${source_dir}/${shader}" "${POLOS_GLSLANG_EXECUTABLE}"
            COMMENT "Compiling shader: ${shader}"
            VERBATIM
        )
        list(APPEND shader_outputs "${output}")
        get_filename_component(relative_dir "${shader}" DIRECTORY)
        install(FILES "${output}" DESTINATION "${install_dir}/${relative_dir}" COMPONENT Runtime)
    endforeach()

    add_custom_target(
        ${target}-resources
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${source_dir}" "$<TARGET_FILE_DIR:${target}>/Resources"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${compiled_dir}" "$<TARGET_FILE_DIR:${target}>/Resources"
            DEPENDS ${shader_outputs}
            VERBATIM
    )
    add_dependencies(
        ${target}
            ${target}-resources
    )

    install(DIRECTORY "${source_dir}/" DESTINATION "${install_dir}" COMPONENT Runtime
        PATTERN "*.vert" EXCLUDE
        PATTERN "*.frag" EXCLUDE
        PATTERN "*.comp" EXCLUDE
        PATTERN "*.tesc" EXCLUDE
        PATTERN "*.tese" EXCLUDE
        PATTERN "*.geom" EXCLUDE
        PATTERN "*.glsl" EXCLUDE
    )
endfunction()

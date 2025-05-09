macro(generate_versioning_info)
    set(oneValueArgs
        NAME
        VERSION_MAJOR
        VERSION_MINOR
        VERSION_PATCH
        ICON
    )

    cmake_parse_arguments(LIB "" "${oneValueArgs}" "" ${ARGN})
endmacro()

include(GenerateExportHeader)

macro(define_polos_module)
    set(oneValueArgs NAME TYPE)
    set(multiValueArgs SOURCES PUBLIC_DEPS PRIVATE_DEPS INTERFACE_DEPS PUBLIC_DEFINES PRIVATE_DEFINES)
    cmake_parse_arguments(MODULE
        "" "${oneValueArgs}" "${multiValueArgs}"
        ${ARGN}
    )

    set(MODULE_NAME_ORIG ${MODULE_NAME})
    set(MODULE_NAME "polos_${MODULE_NAME_ORIG}")

    if (MODULE_TYPE STREQUAL "SHARED")
        add_library(${MODULE_NAME} SHARED ${MODULE_SOURCES})
    elseif (MODULE_TYPE STREQUAL "STATIC")
        add_library(${MODULE_NAME} STATIC ${MODULE_SOURCES})
    elseif (MODULE_TYPE STREQUAL "INTERFACE")
        add_library(${MODULE_NAME} INTERFACE ${MODULE_SOURCES})
    endif ()

    set_target_properties(
        ${MODULE_NAME} PROPERTIES
        PUBLIC_HEADER             "${${MODULE_NAME}_INC}"
        CXX_STANDARD              ${POLOS_CXX_STANDARD}
        CXX_STANDARD_REQUIRED     True
        POSITION_INDEPENDENT_CODE True
        OUTPUT_NAME               "${MODULE_NAME}"
        CXX_VISIBILITY_PRESET     hidden
        VISIBILITY_INLINES_HIDDEN True
        LINKER_LANGUAGE           CXX
    )

    if (MODULE_PUBLIC_DEPS)
        target_link_libraries(${MODULE_NAME} PUBLIC ${MODULE_PUBLIC_DEPS})
    endif ()
    if (MODULE_PRIVATE_DEPS)
        target_link_libraries(${MODULE_NAME} PRIVATE ${MODULE_PRIVATE_DEPS})
    endif ()
    if (MODULE_INTERFACE_DEPS)
        target_link_libraries(${MODULE_NAME} INTERFACE ${MODULE_INTERFACE_DEPS})
    endif ()

    target_compile_definitions(${MODULE_NAME} PRIVATE ${MODULE_PRIVATE_DEFINES})
    target_compile_definitions(${MODULE_NAME} PRIVATE ${MODULE_PUBLIC_DEFINES})

    if (NOT MODULE_TYPE STREQUAL "INTERFACE")
        build_options(${MODULE_NAME} true)
        generate_export_header(${MODULE_NAME}
            BASE_NAME ${MODULE_NAME_ORIG}
            EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/polos/${MODULE_NAME_ORIG}/module_macros.hpp
        )
        target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
        target_include_directories(${MODULE_NAME} PRIVATE ${POLOS_INC_DIR})
        target_compile_definitions(${MODULE_NAME} PRIVATE QUILL_DLL_IMPORT)
        target_compile_definitions(${MODULE_NAME} PRIVATE PL_LOGGER_TYPE=Polos)
    elseif ()
        target_include_directories(${MODULE_NAME} INTERFACE ${POLOS_INC_DIR})
    endif ()

    install(
        TARGETS ${MODULE_NAME}
        LIBRARY       DESTINATION "${POLOS_INSTALL_LIB_DIR}"
        ARCHIVE       DESTINATION "${POLOS_INSTALL_LIB_DIR}"
        RUNTIME       DESTINATION "${POLOS_INSTALL_LIB_DIR}"
        PUBLIC_HEADER DESTINATION "${POLOS_INSTALL_INC_DIR}/${MODULE_NAME_ORIG}"
    )

    add_library(polos::${MODULE_NAME} ALIAS ${MODULE_NAME})
endmacro()

# NAME: Should be the same as the folder name where this macro is called as well.
# DIRECTORY: Relative to root CMakeLists.txt
macro (define_polos_app)
    set(oneValueArgs NAME DIRECTORY LOGGER_TYPE)
    set(multiValueArgs SOURCES DEFINES)
    cmake_parse_arguments(app "" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    add_executable(${app_NAME} ${app_SOURCES})

    target_link_libraries(${app_NAME} PUBLIC polos::polos)

    set_target_properties(
        ${app_NAME} PROPERTIES
        CXX_STANDARD              ${POLOS_CXX_STANDARD}
        CXX_STANDARD_REQUIRED     True
        POSITION_INDEPENDENT_CODE True
        LINKER_LANGUAGE           CXX
        CXX_VISIBILITY_PRESET     hidden
        VISIBILITY_INLINES_HIDDEN True
    )

    if (NOT app_LOGGER_TYPE)
        set(LOGGER_TYPE App)
    endif ()

    target_compile_definitions(${app_NAME} PRIVATE PL_LOGGER_TYPE=${LOGGER_TYPE})
    target_compile_definitions(${app_NAME} PRIVATE QUILL_DLL_IMPORT)

    install(
        TARGETS ${app_NAME}
        RUNTIME DESTINATION ${POLOS_INSTALL_DIR}
    )
endmacro ()

macro (define_polos_test)
    set(options DISABLED)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES DEFINES DEPS EXTRA_ARGS)
    cmake_parse_arguments(test "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    add_executable(${test_NAME} ${test_SOURCES})

    set_target_properties(
        ${test_NAME} PROPERTIES
        CXX_STANDARD              ${POLOS_CXX_STANDARD}
        CXX_STANDARD_REQUIRED     True
        POSITION_INDEPENDENT_CODE True
        OUTPUT_NAME               "${MODULE_NAME}"
        CXX_VISIBILITY_PRESET     hidden
        VISIBILITY_INLINES_HIDDEN True
        LINKER_LANGUAGE           CXX
    )

    if (test_EXTRA_ARGS)
        set(${test_NAME}_EXTRA_ARGS ${test_EXTRA_ARGS})
    else ()
        set(${test_NAME}_EXTRA_ARGS "")
    endif ()

    target_include_directories(${test_NAME} PRIVATE ${POLOS_INC_DIR})

    target_link_libraries(${test_NAME} PRIVATE GTest::gtest)

    if (test_DEPS)
        target_link_libraries(${test_NAME} PRIVATE ${test_DEPS})
    endif ()

    target_compile_definitions(${test_NAME} PRIVATE QUILL_DLL_IMPORT)

    gtest_discover_tests(${test_NAME}
        EXTRA_ARGS ${test_NAME}_EXTRA_ARGS
    )

    # TODO: Not cross platform. Only for Windows for now.
    add_custom_command(TARGET ${test_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_CURRENT_BINARY_DIR} $<TARGET_RUNTIME_DLLS:${test_NAME}>
        COMMAND_EXPAND_LISTS
    )
endmacro ()

macro (define_polos_fixture)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES DEFINES DEPS)
    cmake_parse_arguments(test "" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    add_library(${test_NAME} STATIC ${test_SOURCES})

    set_target_properties(
        ${test_NAME} PROPERTIES
        CXX_STANDARD              ${POLOS_CXX_STANDARD}
        CXX_STANDARD_REQUIRED     True
        POSITION_INDEPENDENT_CODE True
        LINKER_LANGUAGE           CXX
    )

    target_include_directories(${test_NAME} PRIVATE ${POLOS_INC_DIR})

    target_link_libraries(${test_NAME} PRIVATE GTest::gtest)
    if (test_DEPS)
        target_link_libraries(${test_NAME} PUBLIC ${test_DEPS})
    endif ()

endmacro ()

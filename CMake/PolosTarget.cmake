cmake_minimum_required(VERSION 3.26)

include(GoogleTest)

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
    set(multiValueArgs SOURCES
                       PUBLIC_DEPS PRIVATE_DEPS INTERFACE_DEPS
                       PUBLIC_DEFINES PRIVATE_DEFINES
                       TEST_SOURCES TEST_DEPS TEST_DEFINES TEST_EXTRA_ARGS TEST_DATA)
    cmake_parse_arguments(MODULE
        "" "${oneValueArgs}" "${multiValueArgs}"
        ${ARGN}
    )

    if (MODULE_TYPE STREQUAL "SHARED")
        add_library(${MODULE_NAME} SHARED)
    elseif (MODULE_TYPE STREQUAL "STATIC")
        add_library(${MODULE_NAME} STATIC)
    elseif (MODULE_TYPE STREQUAL "INTERFACE")
        add_library(${MODULE_NAME} INTERFACE)
        if (MODULE_SOURCES)
            message(FATAL_ERROR "Interface target cannot have source files!")
        endif()
    endif ()

    file(GLOB_RECURSE ${MODULE_NAME}_INC "${CMAKE_CURRENT_LIST_DIR}/include/polos/${MODULE_NAME}/*.hpp")

    if (NOT MODULE_TYPE STREQUAL "INTERFACE")
        target_sources(${MODULE_NAME} PRIVATE ${MODULE_SOURCES} ${${MODULE_NAME}_INC})
    endif()

    set_target_properties(
        ${MODULE_NAME} PROPERTIES
        #PUBLIC_HEADER             "${${MODULE_NAME}_INC}"
        CXX_STANDARD              ${POLOS_CXX_STANDARD}
        CXX_STANDARD_REQUIRED     True
        POSITION_INDEPENDENT_CODE True
        OUTPUT_NAME               "polos_${MODULE_NAME}"
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

    if (NOT MODULE_TYPE STREQUAL "INTERFACE")
        target_compile_definitions(${MODULE_NAME} PRIVATE ${MODULE_PRIVATE_DEFINES})
        target_compile_definitions(${MODULE_NAME} PRIVATE ${MODULE_PUBLIC_DEFINES})
    endif()

    if (NOT MODULE_TYPE STREQUAL "INTERFACE")
        build_options(${MODULE_NAME} true)
        generate_export_header(${MODULE_NAME}
            BASE_NAME ${MODULE_NAME}
            EXPORT_FILE_NAME ${POLOS_INSTALL_INC_DIR}/polos/${MODULE_NAME}/module_macros.hpp
        )
        target_include_directories(${MODULE_NAME} PUBLIC ${POLOS_INSTALL_INC_DIR})
        target_compile_definitions(${MODULE_NAME} PRIVATE QUILL_DLL_IMPORT)
        target_compile_definitions(${MODULE_NAME} PRIVATE PL_LOGGER_TYPE=Polos)

        target_include_directories(${MODULE_NAME}
            PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        )
    endif ()

    target_include_directories(${MODULE_NAME}
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    )

    target_include_directories(${MODULE_NAME}
        INTERFACE
            $<INSTALL_INTERFACE:include>
    )

    install(
        TARGETS ${MODULE_NAME}
        LIBRARY       DESTINATION "${POLOS_INSTALL_LIB_DIR}"
        ARCHIVE       DESTINATION "${POLOS_INSTALL_LIB_DIR}"
        RUNTIME       DESTINATION "${POLOS_INSTALL_LIB_DIR}"
        #PUBLIC_HEADER DESTINATION "${POLOS_INSTALL_INC_DIR}/polos/${MODULE_NAME}"
    )

    add_library(polos::${MODULE_NAME} ALIAS ${MODULE_NAME})

    if (${BUILD_TESTS} AND MODULE_TEST_SOURCES)
        message(STATUS "[POLOS] Building tests for: ${MODULE_NAME}")

        set(MODULE_TEST_NAME ut-${MODULE_NAME})
        set(MODULE_TEST_NAME ${MODULE_TEST_NAME} PARENT_SCOPE)

        add_executable(${MODULE_TEST_NAME} ${MODULE_TEST_SOURCES} ${MODULE_TEST_DATA})

        set_target_properties(
            ${MODULE_TEST_NAME} PROPERTIES
            CXX_STANDARD              ${POLOS_CXX_STANDARD}
            CXX_STANDARD_REQUIRED     True
            POSITION_INDEPENDENT_CODE True
            OUTPUT_NAME               "${MODULE_TEST_NAME}"
            LINKER_LANGUAGE           CXX
            CXX_VISIBILITY_PRESET     hidden
            VISIBILITY_INLINES_HIDDEN True
        )

        target_include_directories(${MODULE_TEST_NAME} PRIVATE src)
        target_link_libraries(${MODULE_TEST_NAME}
            PRIVATE
                GTest::gtest
                ${MODULE_NAME}
        )

        if (MODULE_TEST_DEPS)
            target_link_libraries(${MODULE_TEST_NAME} PRIVATE ${MODULE_TEST_DEPS})
        endif ()

        target_compile_definitions(${MODULE_TEST_NAME} PRIVATE PL_LOGGER_TYPE=App)
        target_compile_definitions(${MODULE_TEST_NAME} PRIVATE QUILL_DLL_IMPORT)

        gtest_add_tests(
            TARGET ${MODULE_TEST_NAME}
            TEST_LIST MODULE_TEST_LIST
            EXTRA_ARGS ${MODULE_TEST_EXTRA_ARGS}
        )

        set_tests_properties(${MODULE_TEST_LIST}
            PROPERTIES
                RUN_SERIAL ON
                TIMEOUT 60
                LABELS ut
        )

        # TODO: Not cross platform. Only for Windows for now.
        add_custom_command(TARGET ${MODULE_TEST_NAME} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_CURRENT_BINARY_DIR} $<TARGET_RUNTIME_DLLS:${MODULE_TEST_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_TEST_DATA}
            COMMAND_EXPAND_LISTS
        )
    endif()
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

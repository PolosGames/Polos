cmake_minimum_required(VERSION 3.25)

# Called from directories
function(polos_sources)
    file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    )
    set(SOURCES)
    foreach(SOURCE IN LISTS ARGN)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}")
    endforeach()
    target_sources(polos
        PRIVATE ${SOURCES}
        PUBLIC FILE_SET runtime_headers TYPE HEADERS FILES ${HEADERS}
    )
endfunction()

function(polos_tests NAME)
    if (NOT BUILD_TESTS)
        return()
    endif()
    cmake_parse_arguments(TEST "" "" "SOURCES;DATA;LABELS" ${ARGN})
    include(GoogleTest)
    add_executable(ut-${NAME} ${TEST_SOURCES})
    target_link_libraries(ut-${NAME} PRIVATE polos::polos GTest::gtest_main)
    target_compile_definitions(ut-${NAME} PRIVATE PL_LOGGER_TYPE=App QUILL_DLL_IMPORT BUILD_TESTS)
    foreach(DATA_FILE IN LISTS TEST_DATA)
        get_filename_component(DATA_NAME "${DATA_FILE}" NAME)
        configure_file("${DATA_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/${DATA_NAME}" COPYONLY)
    endforeach()
    gtest_add_tests(TARGET ut-${NAME} TEST_LIST REGISTERED_TESTS
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    if (NOT TEST_LABELS)
        set(TEST_LABELS ut)
    endif()
    set_tests_properties(${REGISTERED_TESTS} PROPERTIES RUN_SERIAL ON TIMEOUT 60 LABELS "${TEST_LABELS}")
endfunction()

# no-op build does not advance the local counter.
function(polos_attach_build_info TARGET_NAME SOURCES)
    set(GEN_FILE "${CMAKE_CURRENT_BINARY_DIR}/build_info.generated.cpp")
    set(GIT_STATE_DEPS "")
    find_package(Git QUIET)
    if (GIT_FOUND)
        foreach(GIT_FILE HEAD index)
            execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse --git-path ${GIT_FILE}
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                OUTPUT_VARIABLE GIT_PATH OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            get_filename_component(GIT_PATH "${GIT_PATH}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")
            if (EXISTS "${GIT_PATH}")
                list(APPEND GIT_STATE_DEPS "${GIT_PATH}")
            endif()
        endforeach()
    endif()
    add_custom_command(OUTPUT "${GEN_FILE}"
        COMMAND ${CMAKE_COMMAND}
            "-DBUILD_NUMBER_FILE=${CMAKE_BINARY_DIR}/build_number.txt"
            "-DTEMPLATE_FILE=${POLOS_DIR}/Config/build_info.cpp.in"
            "-DOUTPUT_FILE=${GEN_FILE}"
            "-DSOURCE_DIR=${CMAKE_SOURCE_DIR}"
            -P "${CMAKE_SOURCE_DIR}/CMake/GenerateBuildInfo.cmake"
        DEPENDS ${SOURCES} ${GIT_STATE_DEPS}
            "${CMAKE_SOURCE_DIR}/VERSION"
            "${POLOS_DIR}/Config/build_info.cpp.in"
            "${CMAKE_SOURCE_DIR}/CMake/GenerateBuildInfo.cmake"
        COMMENT "Stamping Polos build info"
        VERBATIM
    )
    target_sources(${TARGET_NAME} PRIVATE "${GEN_FILE}")
endfunction()

# NAME: Should be the same as the folder name where this macro is called as well.
# DIRECTORY: Relative to root CMakeLists.txt
macro (define_polos_app)
    set(oneValueArgs NAME DIRECTORY LOGGER_TYPE)
    set(multiValueArgs SOURCES DEFINES)
    cmake_parse_arguments(app "" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    file(GLOB_RECURSE ${app_NAME}_INC "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

    list(APPEND app_SOURCES ${${app_NAME}_INC})

    add_executable(${app_NAME} ${app_SOURCES})

    target_link_libraries(${app_NAME} PUBLIC polos::polos)
    polos_stage_resources(${app_NAME})

    target_include_directories(${app_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    set_target_properties(${app_NAME}
        PROPERTIES
            CXX_STANDARD              ${POLOS_CXX_STANDARD}
            CXX_STANDARD_REQUIRED     True
            POSITION_INDEPENDENT_CODE True
            LINKER_LANGUAGE           CXX
            CXX_VISIBILITY_PRESET     hidden
            VISIBILITY_INLINES_HIDDEN True
    )

    if (LINUX)
        set_target_properties(${app_NAME}
            PROPERTIES
                INSTALL_RPATH "$ORIGIN"
        )
    endif()

    if (NOT app_LOGGER_TYPE)
        set(LOGGER_TYPE App)
    endif ()

    target_compile_definitions(${app_NAME} PRIVATE PL_LOGGER_TYPE=App)
    target_compile_definitions(${app_NAME} PRIVATE QUILL_DLL_IMPORT)

    target_compile_features(${app_NAME} PRIVATE cxx_std_23)

    build_options(${app_NAME} true)

    if (MSVC)
        install(FILES "$<TARGET_PDB_FILE:${app_NAME}>"
            DESTINATION "${POLOS_INSTALL_DIR}" OPTIONAL)
    endif()

    install(
        TARGETS ${app_NAME}
        RUNTIME DESTINATION ${POLOS_INSTALL_DIR}
    )
endmacro ()

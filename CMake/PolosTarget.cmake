# Called from directories
function(polos_sources)
    file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    )
    set(SOURCES)
    foreach (SOURCE IN LISTS ARGN)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}")
    endforeach()
    target_sources(
        polos
            PRIVATE ${SOURCES}
            PUBLIC FILE_SET runtime_headers TYPE HEADERS FILES ${HEADERS}
    )
endfunction()

function(polos_tests NAME)
    if (NOT BUILD_TESTS)
        return()
    endif()
    cmake_parse_arguments(PARSE_ARGV 1 TEST "" "" "SOURCES;DATA;LABELS;LIBRARIES")
    add_executable(
        ut-${NAME}
            ${TEST_SOURCES}
    )
    target_link_libraries(
        ut-${NAME}
            PRIVATE polos::polos GTest::gtest_main ${TEST_LIBRARIES}
    )
    target_compile_definitions(
        ut-${NAME}
            PRIVATE PL_LOGGER_TYPE=App BUILD_TESTS
    )
    foreach (data_file IN LISTS TEST_DATA)
        get_filename_component(data_name "${data_file}" NAME)
        configure_file("${data_file}" "${CMAKE_CURRENT_BINARY_DIR}/${data_name}" COPYONLY)
    endforeach()
    gtest_add_tests(TARGET ut-${NAME} TEST_LIST REGISTERED_TESTS
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    if (NOT TEST_LABELS)
        set(TEST_LABELS ut)
    endif()
    set_tests_properties(${REGISTERED_TESTS} PROPERTIES RUN_SERIAL ON TIMEOUT 60 LABELS "${TEST_LABELS}")
endfunction()

# Keep the local build counter stable during no-op builds.
function(polos_attach_build_info TARGET_NAME SOURCES)
    set(GEN_FILE "${CMAKE_CURRENT_BINARY_DIR}/build_info.generated.cpp")
    set(GIT_STATE_DEPS "")
    if (Git_FOUND)
        foreach (GIT_FILE HEAD index)
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
            "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
            -P "${CMAKE_SOURCE_DIR}/CMake/GenerateBuildInfo.cmake"
        DEPENDS ${SOURCES} ${GIT_STATE_DEPS}
            "${CMAKE_SOURCE_DIR}/VERSION"
            "${POLOS_DIR}/Config/build_info.cpp.in"
            "${CMAKE_SOURCE_DIR}/CMake/GenerateBuildInfo.cmake"
        COMMENT "Stamping Polos build info"
        VERBATIM
    )
    target_sources(
        ${TARGET_NAME}
            PRIVATE "${GEN_FILE}"
    )
endfunction()

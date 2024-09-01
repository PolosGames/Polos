cmake_minimum_required(VERSION 3.24)

file(GLOB_RECURSE PolosPlatform_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE PolosPlatform_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_library(
    PolosPlatform STATIC
        ${PolosPlatform_SOURCES} ${PolosPlatform_HEADERS}
)

target_include_directories(
    PolosPlatform
        PRIVATE
            ${POLOS_INCLUDE_DIR}
)

target_link_libraries(
    PolosPlatform
        PRIVATE
            glfw
            glad::glad
            glm::glm
)

set_target_properties(
    PolosPlatform
        PROPERTIES
            PUBLIC_HEADER             "${PolosPlatform_HEADERS}"
            CXX_STANDARD              ${POLOS_CPP_STANDARD}
            CXX_STANDARD_REQUIRED     True
            POSITION_INDEPENDENT_CODE True
            OUTPUT_NAME               "PolosPlatform"
)

target_precompile_headers(
    PolosPlatform
        REUSE_FROM
            ${PCH_LIBRARY_NAME}
)

build_options(PolosPlatform true)

install(
    TARGETS PolosPlatform 
    LIBRARY       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    ARCHIVE       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    RUNTIME       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    PUBLIC_HEADER DESTINATION "${POLOS_INSTALL_INCLUDES_DIR}/platform"
)

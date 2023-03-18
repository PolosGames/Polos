cmake_minimum_required(VERSION 3.24)

file(GLOB_RECURSE PolosGraphics_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE PolosGraphics_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_library(
    PolosGraphics STATIC
        ${PolosGraphics_SOURCES} ${PolosGraphics_HEADERS}
)

target_include_directories(
    PolosGraphics
        PRIVATE
            ${POLOS_INCLUDE_DIR}
)

target_link_libraries(
    PolosGraphics
        PRIVATE
            glm::glm_interface
            glad::glad_interface
            glfw::glfw_interface
            stb_image::stb_image_interface
)

set_target_properties(
    PolosGraphics
        PROPERTIES
            PUBLIC_HEADER             "${PolosGraphics_HEADERS}"
            CXX_STANDARD              20
            CXX_STANDARD_REQUIRED     True
            POSITION_INDEPENDENT_CODE TRUE
            OUTPUT_NAME               "PolosGraphics"
)

target_precompile_headers(
    PolosGraphics
        REUSE_FROM
            ${PCH_LIBRARY_NAME}
)

build_options(PolosGraphics true)

install(
    TARGETS PolosGraphics 
    LIBRARY       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    ARCHIVE       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    RUNTIME       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    PUBLIC_HEADER DESTINATION "${POLOS_INSTALL_INCLUDES_DIR}/graphics"
)


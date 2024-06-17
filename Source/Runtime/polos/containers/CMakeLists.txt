cmake_minimum_required(VERSION 3.24)

file(GLOB_RECURSE PolosContainers_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_library(PolosContainers INTERFACE ${PolosContainers_HEADERS})

target_include_directories(
    PolosContainers
        INTERFACE
            ${POLOS_INCLUDE_DIR}
)

target_link_libraries(
    PolosContainers
        INTERFACE
            glm::glm
)

set_target_properties(
    PolosContainers
        PROPERTIES
            PUBLIC_HEADER             "${PolosContainers_HEADERS}"
            CXX_STANDARD              20
            CXX_STANDARD_REQUIRED     True
            POSITION_INDEPENDENT_CODE True
            OUTPUT_NAME               "PolosContainers"
)

target_precompile_headers(
    PolosContainers
        REUSE_FROM
            ${PCH_LIBRARY_NAME}
)

#build_options(PolosContainers true)

install(
    TARGETS PolosContainers 
    LIBRARY       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    ARCHIVE       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    RUNTIME       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    PUBLIC_HEADER DESTINATION "${POLOS_INSTALL_INCLUDES_DIR}/containers"
)

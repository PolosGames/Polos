cmake_minimum_required(VERSION 3.24)

add_executable(Polos)

target_sources(
    Polos
        PRIVATE
            editor.cpp
)

target_include_directories(
    Polos
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    Polos
        PRIVATE
            ${POLOS_INCLUDE_LIB}
            PolosThirdParty
            Runtime
)

set_target_properties(
    Polos
        PROPERTIES
            CXX_STANDARD ${POLOS_CPP_STANDARD}
            CXX_STANDARD_REQUIRED True
            PRECOMPILE_HEADERS_REUSE_FROM ${PCH_LIBRARY_NAME}
)

if(LINUX)
    set_target_properties(
        Polos
            PROPERTIES
                INSTALL_RPATH "\$ORIGIN"
                BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

target_compile_options(Polos PRIVATE -fPIC)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Polos)
endif()

build_options(Polos true)

install(
    TARGETS Polos
    DESTINATION ${INSTALL_DIR}
)
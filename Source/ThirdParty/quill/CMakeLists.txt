cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

find_package(quill CONFIG REQUIRED)

add_library(quill_shared SHARED quill_shared.hpp quill_shared.cpp)

target_include_directories(quill_shared
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(quill_shared PRIVATE -fvisibility=hidden)
endif ()

# Required - Define QUILL_DLL_EXPORT when building the library on Windows
if (WIN32)
    target_compile_definitions(quill_shared PRIVATE QUILL_DLL_EXPORT)
endif ()

target_compile_definitions(quill_shared
    PUBLIC
        QUILL_DISABLE_NON_PREFIXED_MACROS
        QUILL_ENABLE_IMMEDIATE_FLUSH
        QUILL_NO_EXCEPTIONS
)

# Link quill dependency
target_link_libraries(quill_shared PUBLIC quill::quill)

set_target_properties(
    quill_shared PROPERTIES
        CXX_STANDARD              ${POLOS_CXX_STANDARD}
        CXX_STANDARD_REQUIRED     True
        POSITION_INDEPENDENT_CODE True
        OUTPUT_NAME               "quill_shared"
        CXX_VISIBILITY_PRESET     hidden
        VISIBILITY_INLINES_HIDDEN True
        LINKER_LANGUAGE           CXX
)

add_library(quill::quill_shared ALIAS quill_shared)

install(
    TARGETS quill_shared
    LIBRARY       DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    ARCHIVE       DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    RUNTIME       DESTINATION "${POLOS_INSTALL_LIB_DIR}"
)

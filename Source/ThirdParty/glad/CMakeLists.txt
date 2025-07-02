project(glad VERSION 0.1.36 LANGUAGES C)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME}
    PRIVATE
        src/glad.c
        include/glad/glad.h
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        GLAD_GLAPI_EXPORT
    PRIVATE
        GLAD_GLAPI_EXPORT_BUILD
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        POSITION_INDEPENDENT_CODE True
        LINKER_LANGUAGE           C
        PUBLIC_HEADER             "include/glad/glad.h;include/KHR/khrplatform.h"
        VERSION                   ${PROJECT_VERSION}
        C_VISIBILITY_PRESET     default
        VISIBILITY_INLINES_HIDDEN OFF
        SOVERSION                 1
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    ARCHIVE DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    RUNTIME DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    PUBLIC_HEADER DESTINATION ${POLOS_INSTALL_DIR}
)

add_library(glad::glad ALIAS glad)

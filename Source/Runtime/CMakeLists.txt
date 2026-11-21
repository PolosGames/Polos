add_library(
    polos
        SHARED
)
add_library(
    polos::polos
        ALIAS polos
)
target_compile_features(
    polos
        PUBLIC cxx_std_23
)
target_compile_definitions(
    polos
        PRIVATE POLOS_ENGINE_EXPORTS PL_LOGGER_TYPE=Polos
)
target_sources(
    polos
        PUBLIC
            FILE_SET runtime_headers TYPE HEADERS
                BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
            FILE_SET generated_headers TYPE HEADERS
                BASE_DIRS "${POLOS_CONFIG_INCLUDE_DIR}"
                FILES
                    "${POLOS_CONFIG_INCLUDE_DIR}/polos/polos_api.hpp"
                    "${POLOS_CONFIG_INCLUDE_DIR}/polos/polos_config.hpp"
)

target_link_libraries(
    polos
        PUBLIC
            glfw::glfw
            glm::glm
            Vulkan::Vulkan
            GPUOpen::VulkanMemoryAllocator
            stb_image::stb_image
            fastgltf::fastgltf
        PRIVATE
            ryml::ryml
            quill::quill
)
if (ENABLE_PROFILING)
    target_link_libraries(
        polos
            PUBLIC Tracy::TracyClient
    )
endif()

set_target_properties(
    polos
        PROPERTIES
            CXX_VISIBILITY_PRESET     hidden
            VISIBILITY_INLINES_HIDDEN ON
            VERSION                   ${PROJECT_VERSION}
            SOVERSION                 ${PROJECT_VERSION_MAJOR}
            INSTALL_RPATH             "$ORIGIN"
)
polos_build_options(
    polos
)
polos_link_print_support(
    polos
)

add_subdirectory(polos/communication)
add_subdirectory(polos/core)
add_subdirectory(polos/ecs)
add_subdirectory(polos/filesystem)
add_subdirectory(polos/gui)
add_subdirectory(polos/logging)
add_subdirectory(polos/platform)
add_subdirectory(polos/rendering)
add_subdirectory(polos/resource)
add_subdirectory(polos/utils)

get_target_property(POLOS_SOURCES polos SOURCES)
get_target_property(POLOS_RUNTIME_HEADERS polos HEADER_SET_runtime_headers)
get_target_property(POLOS_GENERATED_HEADERS polos HEADER_SET_generated_headers)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX Runtime
    FILES ${POLOS_SOURCES} ${POLOS_RUNTIME_HEADERS})
source_group(Generated FILES ${POLOS_GENERATED_HEADERS})
polos_attach_build_info(
    polos
        "${POLOS_SOURCES};${POLOS_RUNTIME_HEADERS};${POLOS_GENERATED_HEADERS}"
)

add_dependencies(
    polos
        compile_shaders
)
install(TARGETS polos
    LIBRARY DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT Runtime NAMELINK_COMPONENT SDK
    ARCHIVE DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT SDK
    RUNTIME DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT Runtime
    FILE_SET runtime_headers DESTINATION "${POLOS_INSTALL_INC_DIR}" COMPONENT SDK
    FILE_SET generated_headers DESTINATION "${POLOS_INSTALL_INC_DIR}" COMPONENT SDK
)
if (MSVC)
    install(FILES "$<TARGET_PDB_FILE:polos>" DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT Runtime OPTIONAL)
endif()
install(FILES "${CMAKE_SOURCE_DIR}/VERSION" "${CMAKE_SOURCE_DIR}/LICENSE"
    DESTINATION "${POLOS_INSTALL_DIR}" COMPONENT Runtime)

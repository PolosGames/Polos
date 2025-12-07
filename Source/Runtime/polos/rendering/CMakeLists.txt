cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

generate_versioning_info(
    NAME rendering
    VERSION_MAJOR 1
    VERSION_MINOR 0
    VERSION_PATCH 0
)

project(
    rendering
        LANGUAGES CXX
        VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH}
)

set(RENDERING_IMPL_SRC
    "src/render_pass/lambda_pass.cpp"
    "src/system/geometry_render_system.cpp"
    "src/common.cpp"
    "src/dll_main.cpp"
    "src/pipeline_cache.cpp"
    "src/render_context.cpp"
    "src/render_graph.cpp"
    "src/render_pass_resolver.cpp"
    "src/shader_cache.cpp"
    "src/vertex.cpp"
    "src/vulkan_context.cpp"
    "src/vulkan_device.cpp"
    "src/vulkan_resource_manager.cpp"
    "src/vulkan_swapchain.cpp"
)

set(RENDERING_THIRD_PARTY_DEPS
    glfw
    glm::glm
    GPUOpen::VulkanMemoryAllocator
    Vulkan::Vulkan
)

if (HOT_RELOAD)
    set(impl_lib polos::rendering_impl)
    set(RENDERING_SRC "")

    define_polos_module(
        NAME
            rendering_impl

        ENABLE_HOT_RELOAD

        TYPE
            SHARED

        SOURCES
            ${RENDERING_IMPL_SRC}

        PUBLIC_DEPS
            ${RENDERING_THIRD_PARTY_DEPS}
            polos::logging

        PRIVATE_DEPS
            polos::communication
            polos::filesystem
    )
else()
    set(impl_lib "")
    set(RENDERING_SRC ${RENDERING_IMPL_SRC})
endif()

define_polos_module(
    NAME
        rendering

    TYPE
        SHARED

    SOURCES
        "src/scene.cpp"
        "src/vulkan_rendering_api.cpp"
        ${RENDERING_SRC}

    PUBLIC_DEPS
        ${impl_lib}
        ${RENDERING_THIRD_PARTY_DEPS}
        polos::logging

    PRIVATE_DEPS
        polos::communication
        polos::filesystem

    TEST_SOURCES
        "test/mock/mock_vulkan_device.hpp"
        "test/common_test.cpp"
        "test/pipeline_cache_test.cpp"
        "test/render_graph_test.cpp"
)

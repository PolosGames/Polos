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
    "src/general_pass.cpp"
    "src/pipeline_cache.cpp"
    "src/render_context.cpp"
    "src/shader_cache.cpp"
    "src/vertex.cpp"
    "src/vulkan_context.cpp"
    "src/vulkan_device.cpp"
    "src/vulkan_resource_manager.cpp"
    "src/vulkan_swapchain.cpp"
    "src/vulkan_util.cpp"
)

set(RENDERING_THIRD_PARTY_DEPS
    glfw
    glm::glm
    GPUOpen::VulkanMemoryAllocator
    Vulkan::Vulkan
)

set(RENDERING_IMPL_DEPS
    stb_image::stb_image
)

if (HOT_RELOAD)
    set(RENDERING_SRC "")
    set(RENDERING_SRC_DEPS "")

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

        PRIVATE_DEPS
            ${RENDERING_IMPL_DEPS}
    )
else()
    set(RENDERING_SRC ${RENDERING_IMPL_SRC})
    set(RENDERING_SRC_DEPS ${RENDERING_IMPL_DEPS})
endif()

define_polos_module(
    NAME
        rendering

    TYPE
        OBJECT

    SOURCES
        "src/scene.cpp"
        "src/vulkan_rendering_api.cpp"
        ${RENDERING_SRC}

    PUBLIC_DEPS
        ${RENDERING_THIRD_PARTY_DEPS}
        polos::logging

    PRIVATE_DEPS
        polos::communication
        polos::filesystem
        ${RENDERING_SRC_DEPS}
)

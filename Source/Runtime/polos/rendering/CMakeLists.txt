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
    "src/cache/pipeline_cache.cpp"
    "src/cache/shader_cache.cpp"
    "src/compositor/render_compositor.cpp"
    "src/passes/general_pass.cpp"
    "src/resources/vertex.cpp"
    "src/scene/camera3d.cpp"
    "src/vk/render_context.cpp"
    "src/vk/vulkan_context.cpp"
    "src/vk/vulkan_device.cpp"
    "src/vk/vulkan_resource_manager.cpp"
    "src/vk/vulkan_swapchain.cpp"
    "src/vk/vulkan_util.cpp"
)

set(RENDERING_THIRD_PARTY_DEPS
    glm::glm
    Vulkan::Vulkan
)

set(RENDERING_IMPL_DEPS
    GPUOpen::VulkanMemoryAllocator
    stb_image::stb_image
)

if (HOT_RELOAD)
    set(RENDERING_SRC "")
    set(RENDERING_SRC_DEPS "")

    define_polos_module(
        NAME
            rendering_impl

        ENABLE_HOT_RELOAD

        WITH_BUILD_INFO

        TYPE
            SHARED

        SOURCES
            ${RENDERING_IMPL_SRC}

        PRIVATE_DEPS
            ${RENDERING_THIRD_PARTY_DEPS}
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
        "src/scene/scene.cpp"
        "src/vulkan_rendering_api.cpp"
        ${RENDERING_SRC}

    PUBLIC_DEPS
        ${RENDERING_THIRD_PARTY_DEPS}
        polos::communication

    PRIVATE_DEPS
        polos::logging
        polos::utils
        polos::filesystem
        ${RENDERING_SRC_DEPS}
)
